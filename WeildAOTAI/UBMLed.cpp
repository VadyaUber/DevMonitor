#include "UBMLed.h"




UBMLed::UBMLed(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin,bool WG36, uint8_t Led1Pin, uint8_t Led2Pin, uint8_t Led3Pin, uint8_t Led4Pin, uint8_t WG24_36Pin, uint8_t BeperPin):ShiftRegister(Ds_pin, SHcp_pin, STcp_pin)

{

	WG24_36 = WG24_36Pin;
	Led1 = Led1Pin;
	Led2 = Led2Pin;
	Led3 = Led3Pin;
	Led4 = Led4Pin;
	Time.IntevralSleep = 500;
	if (WG36) {
		Write((uint8_t*)WG24_36, 1);
	}
	else {
		ValueReg = 0;
		Write(&ValueReg, 1);
	}

}

void UBMLed::Loop()
{

	if(Time.CheckTimeEvent()) {
		if (*Inerfece == "eth0") {
			if (!Status->StatusIterfece)blink(Led4, 1);
			else if (!Status->StatusSocet)blink(Led4, 2);
			else SetPin(Led4);
			
		}
		else {
			if (!Status->StatusIterfece)blink(Led3, 1);
			else if (!Status->StatusSocet)blink(Led3, 2);
			else SetPin(Led3);
		}
		if (Status->StatusBloc)SetPin(Led2);
		else ResetPin(Led2);
		if (Status->StatusRfid1 == Wait)blink(Led1, 2);
		else if (Status->StatusRfid1 == SendServer)SetPin(Led1);
		else if (Status->StatusRfid1 == EroorRead)blink(Led1, 3);

	}
	usleep(100);
}

void UBMLed::blink(uint8_t pin, uint8_t count)
{
	for (int i = 0; i < count; i++) {
	
		SetPin(pin);
		delay(100);
		ResetPin(pin);
	}
}

void UBMLed::ResetPin(uint8_t pin)
{
	ValueReg &=~ pin;
	Write(&ValueReg, 1);
}

void UBMLed::SetPin(uint8_t pin)
{
	ValueReg |= pin;
	Write(&ValueReg, 1);
}
