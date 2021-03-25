#include "UBMLed.h"




UBMLed::UBMLed(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin,bool WG36):ShiftRegister(Ds_pin, SHcp_pin, STcp_pin)

{
	Time.IntevralSleep = 500;
	if (WG36) {
		Write((uint8_t*)WG24_36, 1);
	}
	else {
		Write(0, 1);
	}

}

void UBMLed::Loop()
{

	if(Time.CheckTimeEvent()) {
		if (*Inerfece == "eth0") {
			if (!Status->StatusIterfece)blink(LED4, 1);		
			else if (!Status->StatusSocet)blink(LED4, 2);
			else SetLed(LED4);
			
		}
		else {
			if (!Status->StatusIterfece)blink(LED3, 1);
			else if (!Status->StatusSocet)blink(LED3, 2);
			else SetLed(LED3);
		}

	}
	
}

void UBMLed::blink(uint8_t pin, uint8_t count)
{
	for (int i = 0; i < count; i++) {
		ValueReg |= pin;
		Write(&ValueReg, 1);
		delay(100);
		ValueReg &= ~(1 << pin);
		Write(&ValueReg, 1);
	}
}

void UBMLed::SetLed(uint8_t pin)
{
	ValueReg |= pin;
	Write(&ValueReg, 1);
}
