#include "ShiftRegister.h"

ShiftRegister::ShiftRegister(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin)
{
	Ds = Ds_pin;
	SHcp = SHcp_pin;
	STcp =STcp_pin;
	wiringPiSetup();
	pinMode(Ds, OUTPUT);
	pinMode(SHcp, OUTPUT);
	pinMode(STcp, OUTPUT);
}

void ShiftRegister::Write(uint8_t * value, uint8_t size)
{
	for (int i; i < size; i++) {
		for (int k; k < 8; k++) {
			digitalWrite(Ds, (value[i] >> k) & 0x01);
			Clock();
		}
	}

}

void ShiftRegister::Clock()
{
	digitalWrite(SHcp, HIGH);
	delayMicroseconds(500);
	digitalWrite(SHcp, LOW);
	delayMicroseconds(500);
}

void ShiftRegister::Strobe()
{
	digitalWrite(STcp, HIGH);
	delayMicroseconds(500);
	digitalWrite(STcp, LOW);

}
