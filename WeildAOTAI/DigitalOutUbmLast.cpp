#include "DigitalOutUbmLast.h"

DigitalOutUbmLast::DigitalOutUbmLast(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin, uint8_t PowerOnPin, uint8_t BeepPin, uint8_t RST_Pin, uint8_t WG_36Pin) :ShiftRegister(Ds_pin, SHcp_pin, STcp_pin)
{
	this->PowerOnPin = PowerOnPin;
	this->PowerOn = BeepPin;
	this->RST_Pin = RST_Pin;
	this->WG_36Pin = WG_36Pin;


}

void DigitalOutUbmLast::Loop()
{
	if (this->Beep)this->value |= this->BeepPin;
	else this->value &=~this->BeepPin;

	if(this->PowerOn)this->value |= this->PowerOnPin;
	else this->value &= ~this->PowerOnPin;

	if (this->RST_CLK)this->value |= this->RST_Pin;
	else this->value &= ~this->RST_Pin;

	if (this->WG_36)this->value |= this->WG_36Pin;
	else this->value &= ~this->WG_36Pin;
	if (this->LastVal != this->value) {
		this->LastVal = this->value;
		Write((uint8_t *)&this->value, 2);
	}
	
}
