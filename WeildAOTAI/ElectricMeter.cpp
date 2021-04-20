#include "ElectricMeter.h"

ElectricMeter::ElectricMeter(uint8_t cs_pin,uint32_t Cicle, string FileConfig, string NameConfig)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	spi_dev  = SPI("/dev/spidev1.0", 5000000, 8, 2);;
	CS_PIN = cs_pin;
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);
	Write8bit(MMODE, 0xFC | PHASE_A);
	Write8bit(LCYCMODE, 0x3F);
	Write16bit(LINECYC, Cicle);
	Write8bit(APHCAL, 114);
	Write8bit(BPHCAL, 114);
	Write8bit(CPHCAL, 114);
	CicleMeter = Cicle;
	GetConfig(FileConfig, NameConfig);
}

void ElectricMeter::ReadValue()
{
	status= ReadReg24(STATUS);
	
	if ((status&(1 << LENERGY)) != 0) {
		ValueRead= ((double)((int16_t)Read16bit(AWATTHR+PHASE_A))*KF.AWhLSB);
		PowerWath = ValueRead;
		if (ValueRead > 0) SumWatA += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AWATTHR + PHASE_B))*KF.BWhLSB);
		PowerWath+= ValueRead;
		if (ValueRead > 0) SumWatB += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AWATTHR + PHASE_C))*KF.CWhLSB);
		PowerWath += ValueRead;
		if (ValueRead > 0) SumWatC += ValueRead;
		
		ValueRead = ((double)((int16_t)Read16bit(AVARHR + PHASE_A))*KF.AVARhLSB);
		PowerVar = ValueRead;
		if (ValueRead > 0) SumVarA += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVARHR + PHASE_B))*KF.BVARhLSB);
		PowerVar += ValueRead;
		if (ValueRead > 0) SumVarB += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVARHR + PHASE_C))*KF.CVARhLSB);
		PowerVar += ValueRead;
		if (ValueRead > 0) SumVarC += ValueRead;

		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_A))*KF.AVAhLSB);
		PowerVa = ValueRead;
		if (ValueRead > 0) SumVaA += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_B))*KF.BVAhLSB);
		PowerVa += ValueRead;
		if (ValueRead > 0) SumVaB += ValueRead;
		ValueRead = ((double)((int16_t)Read16bit(AVAHR + PHASE_C))*KF.CVAhLSB);
		PowerVa += ValueRead;
		if (ValueRead > 0) SumVaC += ValueRead;


		double accumTime = ((double)CicleMeter / (2 * 3 * (double)Read16bit(FREQ)*0.0625));
		SumEnergyWat =(int) SumWatA + SumWatB + SumWatC;
		SumEnergyVar = (int)SumVarA + SumVarB + SumVarC;
		SumEnergyVa = (int)SumVaA + SumVaB + SumVaC;
		SumPowerWat = (int)(PowerWath * 3600 / accumTime);
		SumPowerVar = (int)(PowerVar * 3600 / accumTime);
		SumPowerVa =(int)(PowerVa * 3600 / accumTime);
		ReadReg24(RSTATUS);

	}
}
void ElectricMeter::Write8bit(uint8_t adr, uint8_t value) {
	uint8_t tx[2] = { adr | 0x80,value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev.SpiWriteRead(tx, rx, 2);
	digitalWrite(CS_PIN, HIGH);

}
void ElectricMeter::Write16bit(uint8_t adr, uint16_t value) {
	uint8_t tx[3] = { adr | 0x80,(uint8_t)(value >> 8),(uint8_t)value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev.SpiWriteRead(tx, rx, 3);
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


}
uint16_t ElectricMeter::Read16bit(uint8_t adr) {
	uint8_t tx[3] = { adr,0,0 };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev.SpiWriteRead(tx, rx, 3);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 8 | rx[2];
}
uint32_t ElectricMeter::ReadReg24(uint8_t adr) {
	uint8_t tx[4] = { adr,0,0,0 };
	uint8_t rx[4] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev.SpiWriteRead(tx, rx, 4);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 16 | rx[2] << 8 | rx[3];
}