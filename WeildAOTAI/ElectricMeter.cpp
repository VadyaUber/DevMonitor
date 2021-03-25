#include "ElectricMeter.h"

ElectricMeter::ElectricMeter(uint8_t cs_pin, SPI * interfece,uint32_t Cicle)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	spi_dev = interfece;
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
}

void ElectricMeter::ReadValue()
{
	status= ReadReg24(STATUS);
	if ((status&(1 << LENERGY)) != 0) {
		ValueRead= ((double)(Read16bit(AWATTHR+PHASE_A))*AWhLSB);
		if (ValueRead > 0) SumWatA += ValueRead;
		ValueRead = ((double)(Read16bit(AWATTHR + PHASE_B))*BWhLSB);
		if (ValueRead > 0) SumWatB += ValueRead;
		ValueRead = ((double)(Read16bit(AWATTHR + PHASE_C))*CWhLSB);
		if (ValueRead > 0) SumWatC += ValueRead;
		ValueRead = ((double)(Read16bit(AVARHR + PHASE_A))*AVARhLSB);
		if (ValueRead > 0) SumVarA += ValueRead;
		ValueRead = ((double)(Read16bit(AVARHR + PHASE_B))*BVARhLSB);
		if (ValueRead > 0) SumVarB += ValueRead;
		ValueRead = ((double)(Read16bit(AVARHR + PHASE_C))*CVARhLSB);
		if (ValueRead > 0) SumVarC += ValueRead;
		ValueRead = ((double)(Read16bit(AVAHR + PHASE_A))*AVAhLSB);
		if (ValueRead > 0) SumVaA += ValueRead;
		ValueRead = ((double)(Read16bit(AVAHR + PHASE_B))*BVAhLSB);
		if (ValueRead > 0) SumVaB += ValueRead;
		ValueRead = ((double)(Read16bit(AVAHR + PHASE_C))*CVAhLSB);
		if (ValueRead > 0) SumVaC += ValueRead;
		double accumTime = ((double)CicleMeter / (2 * 3 * (double)Read16bit(FREQ)*0.0625));
		SumEnergyWat =(int) SumWatA + SumWatB + SumWatC;
		SumEnergyVar = (int)SumVarA + SumVarB + SumVarC;
		SumEnergyVa = (int)SumVaA + SumVaB + SumVaC;
		SumPowerWat = (int)SumEnergyWat * 3600 / accumTime;
		SumPowerVar = (int)SumEnergyVar * 3600 / accumTime;
		SumPowerVa =(int) SumEnergyVa * 3600 / accumTime;
		ReadReg24(RSTATUS);

	}
}
void ElectricMeter::Write8bit(uint8_t adr, uint8_t value) {
	uint8_t tx[2] = { adr | 0x80,value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev->SpiWriteRead(tx, rx, 2);
	digitalWrite(CS_PIN, HIGH);

}
void ElectricMeter::Write16bit(uint8_t adr, uint16_t value) {
	uint8_t tx[3] = { adr | 0x80,(uint8_t)(value >> 8),(uint8_t)value };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev->SpiWriteRead(tx, rx, 3);
	digitalWrite(CS_PIN, HIGH);
}
uint16_t ElectricMeter::Read16bit(uint8_t adr) {
	uint8_t tx[3] = { adr,0,0 };
	uint8_t rx[3] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev->SpiWriteRead(tx, rx, 3);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 8 | rx[2];
}
uint32_t ElectricMeter::ReadReg24(uint8_t adr) {
	uint8_t tx[4] = { adr,0,0,0 };
	uint8_t rx[4] = { 0 };
	digitalWrite(CS_PIN, LOW);
	spi_dev->SpiWriteRead(tx, rx, 4);
	digitalWrite(CS_PIN, HIGH);
	return rx[1] << 16 | rx[2] << 8 | rx[3];
}