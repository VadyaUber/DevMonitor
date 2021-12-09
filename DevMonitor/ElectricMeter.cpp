#include "ElectricMeter.h"

ElectricMeter::ElectricMeter(uint8_t cs_pin,uint32_t Cicle, string FileConfig, string NameConfig)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	
	CS_PIN = cs_pin;
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);
	init_SPI("/dev/spidev1.0", CLK_Meter, 8, 2, dev);
	digitalWrite(CS_PIN, LOW);
	usleep(1000);
	digitalWrite(CS_PIN, HIGH);
	Write8bit(MMODE, 0xFD);
	Write8bit(LCYCMODE, 0x3F);
	Write16bit(LINECYC, Cicle);
	CicleMeter = Cicle;
	GetConfig(FileConfig, NameConfig);
}

void ElectricMeter::ReadValue()
{
	if (NameSPI != dev) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", CLK_Meter, 8, 2, dev);
	}
	status= ReadReg24(STATUS);
	
	if ((status&(1 << (LENERGY))) != 0) {

		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_A))*KF.AVAhLSB);
		PowerVa = ValueRead;
		SumVaA += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_B))*KF.BVAhLSB);
		PowerVa += ValueRead;
		SumVaB += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_C))*KF.CVAhLSB);
		PowerVa += ValueRead;
		SumVaC += ValueRead;

		freq = Read16bit(FREQ);
		double accumTime = ((double)CicleMeter / (2 * (1 / (freq * 0.0000096))*3));
		SumEnergyVa = (int)(SumVaA + SumVaB + SumVaC)* 2.6;
		SumPowerVa =(int)(PowerVa * 3600 / accumTime);
		ReadReg24(RSTATUS);

	}
}
void ElectricMeter::Write8bit(uint8_t adr, uint8_t value) {
	uint8_t tx[2] = { adr | 0x80,value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	SpiWriteRead(tx, rx, 2);
	digitalWrite(CS_PIN, HIGH);

}
void ElectricMeter::Write16bit(uint8_t adr, uint16_t value) {
	uint8_t tx[3] = { adr | 0x80,(uint8_t)(value >> 8),(uint8_t)value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	SpiWriteRead(tx, rx, 3);
	digitalWrite(CS_PIN, HIGH);
}
void ElectricMeter::GetConfig(string FileConfig, string NameConfig)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(FileConfig.c_str());

	if (!result)
		return;
	KF.AWhLSB = doc.child("config").child(NameConfig.c_str()).child("AWhLSB").attribute("value").as_double();
	KF.BWhLSB = doc.child("config").child(NameConfig.c_str()).child("BWhLSB").attribute("value").as_double();
	KF.CWhLSB = doc.child("config").child(NameConfig.c_str()).child("CWhLSB").attribute("value").as_double();

	KF.AVARhLSB = doc.child("config").child(NameConfig.c_str()).child("AVARhLSB").attribute("value").as_double();
	KF.BVARhLSB = doc.child("config").child(NameConfig.c_str()).child("BVARhLSB").attribute("value").as_double();
	KF.CVARhLSB = doc.child("config").child(NameConfig.c_str()).child("CVARhLSB").attribute("value").as_double();

	KF.AVAhLSB = doc.child("config").child(NameConfig.c_str()).child("AVAhLSB").attribute("value").as_double();
	KF.BVAhLSB = doc.child("config").child(NameConfig.c_str()).child("BVAhLSB").attribute("value").as_double();
	KF.CVAhLSB = doc.child("config").child(NameConfig.c_str()).child("CVAhLSB").attribute("value").as_double();

	KF.AVLSB = doc.child("config").child(NameConfig.c_str()).child("AVLSB").attribute("value").as_double();
	KF.BVLSB = doc.child("config").child(NameConfig.c_str()).child("BVLSB").attribute("value").as_double();
	KF.CVLSB = doc.child("config").child(NameConfig.c_str()).child("CVLSB").attribute("value").as_double();

	KF.AILSB = doc.child("config").child(NameConfig.c_str()).child("AILSB").attribute("value").as_double();
	KF.BILSB = doc.child("config").child(NameConfig.c_str()).child("BILSB").attribute("value").as_double();
	KF.CILSB = doc.child("config").child(NameConfig.c_str()).child("CILSB").attribute("value").as_double();
}

void ElectricMeter::power_quality_analyze()
{
	uint16_t u_min = 500;
	uint16_t u_max = 0;

	uint32_t VRMSA = getVRMS(AVRMS) / KF.AVLSB;
	u_min = (u_min > VRMSA) ? VRMSA : u_min;
	u_max = (u_max < VRMSA) ? VRMSA : u_max;
	uint32_t VRMSB = getVRMS(BVRMS) / KF.BVLSB;
	u_min = (u_min > VRMSB) ? VRMSB : u_min;
	u_max = (u_max < VRMSB) ? VRMSB : u_max;
	uint32_t VRMSC = getVRMS(CVRMS) / KF.CVLSB;
	u_min = (u_min > VRMSC) ? VRMSC : u_min;
	u_max = (u_max < VRMSC) ? VRMSC : u_max;

	double asymmetry = 100 * (1 - (u_min / u_max));
	if (asymmetry > VOLTAGE_ASYMMETRY_MAX)
		power_quality |= 1;
	else
		power_quality |= 0;
	if (u_min > (NOMINAL_SUPPLY_VOLTAGE + VOLTAGE_DEVIATION_MAX))
		power_quality |= 1 << 1;
	else
		power_quality |= 0 << 1;
	if (u_max < (NOMINAL_SUPPLY_VOLTAGE - VOLTAGE_DEVIATION_MAX))
		power_quality |= 1 << 2;
	else
		power_quality |= 0 << 2;
	if((freq * HzLSB > (NOMINAL_POWER_FREQUENCY + FREQUENCY_DEVIATION_MAX))|| (freq * HzLSB < (NOMINAL_POWER_FREQUENCY - FREQUENCY_DEVIATION_MAX)))
		power_quality |= 1 << 3;
	else
		power_quality |= 0 << 3;
}

uint32_t ElectricMeter::getVRMS(uint8_t Phase)
{
	uint32_t MathWait = 0;
	uint32_t TekValue = 0;
	uint8_t size = 20;
	uint32_t aray[size];
	
	int n = 0;
	for (int i = 0; i < size; i++) {
		aray[i]= ReadReg24(Phase);
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
	return  TekValue / n;
}

uint16_t ElectricMeter::Read16bit(uint8_t adr) {
	uint8_t tx[3] = { adr,0,0 };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	SpiWriteRead(tx, rx, 3);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 8 | rx[2];
}
uint32_t ElectricMeter::ReadReg24(uint8_t adr) {
	uint8_t tx[4] = { adr,0,0,0 };
	uint8_t rx[4] = { 0 };
	digitalWrite(CS_PIN, LOW);
	SpiWriteRead(tx, rx, 4);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 16 | rx[2] << 8 | rx[3];
}