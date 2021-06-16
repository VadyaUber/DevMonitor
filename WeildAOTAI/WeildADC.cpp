#include "WeildADC.h"

WeildADC::WeildADC(uint8_t cs_pin, bool filter, string FileConfig, string NameConfig)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	CS_PIN = cs_pin;
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);

	init_SPI("/dev/spidev1.0", FREQ_CLK, 8, 2, dev);
	digitalWrite(CS_PIN, LOW);
	usleep(1000);
	digitalWrite(CS_PIN, HIGH);
	GetConfig(FileConfig, NameConfig);
	//mat = param;
}



void WeildADC::ReadValue()
{

	if (NameSPI != dev) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", FREQ_CLK, 8, 2, dev);
	}
	if (MeasureEnable) {
		PriznReadAdc = true;
		uint8_t tx[3] = { 0 };
		uint16_t adc_out = 0;
		uint8_t rx[3] = { 0 };
		int tmp = 0;
		digitalWrite(CS_PIN, LOW);
		//tmp=spi_dev.SpiWriteRead(tx, rx, 3);
		SpiWriteRead(tx, rx, 3);
		digitalWrite(CS_PIN, HIGH);
		
		adc_out = (((uint32_t)((rx[0] & 0x07) << 16) | (rx[1] << 8) | (rx[2])) >> 2);
		if (rx[0] != 0) {
		
				if (FilterADC.Cnt >= FILTER_SIZE) {
					FilterADC.Cnt = 0;
				}
			 //	printf("adc val %f\n\r", (float)(adc_out*REF));
				FilterADC.Array[FilterADC.Cnt] = pow(abs((adc_out*REF) - mat.offset) * mat.divisor, 2);
				FilterADC.Cnt++;

		}
		PriznReadAdc = false;
	}
}

void WeildADC::CalculateAdc()
{
	while (PriznReadAdc);
	MeasureEnable = false;
	if (filter_on) {
		Value16Bit = (uint16_t)(sqrt(FiltringADC(FilterADC.Array, FilterADC.Cnt))) * mat.multiplier;
		FilterADC.Cnt = 0;
	}
	else {
		for (int i = 0; i < FilterADC.Cnt; i++) {
			SummArray += FilterADC.Array[i];
		}
		Value16Bit = (uint16_t)(sqrt(SummArray /FilterADC.Cnt)) * mat.multiplier;
	}
	

	MeasureEnable = true;


}

void WeildADC::GetConfig(string FileConfig, string NameConfig)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(FileConfig.c_str());

	if (!result)
		return;
	mat.multiplier= doc.child("config").child(NameConfig.c_str()).child("multiplier").attribute("value").as_double();
	mat.offset = doc.child("config").child(NameConfig.c_str()).child("offset").attribute("value").as_double();
	mat.divisor = doc.child("config").child(NameConfig.c_str()).child("divisor").attribute("value").as_double();
}

double WeildADC::FiltringADC(double * aray, int size) {
	double MathWait = 0;
	double TekValue = 0;
	int n = 0;
	for (int i = 0; i < size; i++) {
		MathWait += aray[i];
	}
	MathWait = MathWait / size;
	for (int i = 0; i < size; i++) {
		if (aray[i] < (MathWait + MathWait * 0.3) && aray[i] > (MathWait - MathWait * 0.3)) {
			TekValue += aray[i];
			n++;
		}
	}
	return  TekValue / n;
}

