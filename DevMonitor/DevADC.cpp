#include "DevADC.h"
DevADC::DevADC(uint8_t cs_pin, bool filter, string FileConfig, string NameConfig)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	CS_PIN = cs_pin;
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);

	if (NameSPI != dev) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", FREQ_CLK, 8, 2, dev);
	}
	digitalWrite(CS_PIN, LOW);
	usleep(1000);
	digitalWrite(CS_PIN, HIGH);
	GetConfig(FileConfig, NameConfig);
	//mat = param;
	filter_on = false;
}



void DevADC::ReadValue()
{
	if (MeasureEnable) {
		if (NameSPI != dev) {
			DeInitSPI();
			init_SPI("/dev/spidev1.0", FREQ_CLK, 8, 2, dev);
		}
		PriznReadAdc = true;
		uint8_t tx[3] = { 0 };
		uint32_t adc_out = 0;
		uint8_t rx[3] = { 0 };
		digitalWrite(CS_PIN, LOW);
		SpiWriteRead(tx, rx, 3);
		digitalWrite(CS_PIN, HIGH);
		
		adc_out = (((uint32_t)((rx[0] & 0x07) << 16) | (rx[1] << 8) | (rx[2])) >> 2);
		//if (rx[0] != 0) {
		//if (adc_out != 0) {
		if (FilterADC.Cnt >= FILTER_SIZE) {
			FilterADC.Cnt = 0;
		}
		//printf("adc val*ref %f\n\r", (float)(adc_out*REF));
		//printf("adc val %d\n\r", (adc_out));
		//FilterADC.Array[FilterADC.Cnt] = pow(abs((adc_out*REF) - mat.offset) * mat.divisor, 2);
		FilterADC.Array[FilterADC.Cnt] = adc_out;
		FilterADC.Cnt++;

		//}
		PriznReadAdc = false;
	}
}

void DevADC::CalculateAdc()
{
	while (PriznReadAdc);
	MeasureEnable = false;
	double tmp;
	double tmpval;
	if (filter_on) {
		//uint32_t tm;
		//tm = (FiltringADC(FilterADC.Array, FilterADC.Cnt));
		//tmp = (tm * REF);
		//tmp = (abs(tmp - mat.offset));
		//tmp = ((tmp * mat.divisor));
		//

		//tmp = tmp * mat.multiplier;
		tmp = (abs(((FiltringADC(FilterADC.Array, FilterADC.Cnt)) * REF) - mat.offset)) *mat.divisor * mat.multiplier;
		//tm = (FiltringADC(FilterADC.Array, FilterADC.Cnt));
		Value16Bit = (uint16_t)(tmp + 0.5);
	}
	else {
		for (int i = 0; i < FilterADC.Cnt; i++) {
			SummArray += FilterADC.Array[i];
		}
		tmp = abs((SummArray / FilterADC.Cnt) * REF);
		tmpval = tmp;
		tmp = tmp - mat.offset;
		tmp = tmp * mat.divisor * mat.multiplier;
		tmp = (abs(((SummArray / FilterADC.Cnt) * REF) - mat.offset)) * mat.divisor * mat.multiplier;
		Value16Bit = (uint16_t)(tmp + 0.5);
		//printf("%s%s", CS_PIN == 11 ? "I= " : "", CS_PIN == 10 ? "V= " : ""); printf("Val_adc %f", tmpval); printf(" Val %d \n", Value16Bit);
		SummArray = 0;
	}
	FilterADC.Cnt = 0;
	MeasureEnable = true;
}

void DevADC::GetConfig(string FileConfig, string NameConfig)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(FileConfig.c_str());

	if (!result)
		return;
	mat.multiplier= doc.child("config").child(NameConfig.c_str()).child("multiplier").attribute("value").as_double();
	mat.offset = doc.child("config").child(NameConfig.c_str()).child("offset").attribute("value").as_double();
	mat.divisor = doc.child("config").child(NameConfig.c_str()).child("divisor").attribute("value").as_double();
}

uint32_t DevADC::FiltringADC(uint32_t* aray, int size) {
	uint64_t MathWait = 0;
	uint32_t TekValue = 0;
	int n = 0;
	if (size == 0) return 0;
	for (int i = 0; i < size; i++) {
		MathWait += aray[i];
	}
	MathWait = MathWait / size;
	for (int i = 0; i < size; i++) {
		if (aray[i] < (MathWait + MathWait * 0.3) && aray[i] > (MathWait - MathWait * 0.3)) 
		{
			TekValue += aray[i];
			n++;
		}
	}
	//for (int i = 0; i < size; i++) {
	//		TekValue += aray[i];
	//		n++;
	//}
	if (n == 0) return 0;
	return  TekValue / n;
}

