#include "LedGpio.h"

LedGpio::LedGpio(uint8_t LedPin1, uint8_t LedPin2, uint8_t LedPin3, uint8_t LedPin4)
{
	wiringPiSetup();
	Led1 = LedPin1;
	Led2 = LedPin2;
	Led3 = LedPin3;
	Led4 = LedPin4;
	pinMode(Led1, OUTPUT);
	pinMode(Led2, OUTPUT);
	pinMode(Led3, OUTPUT);
	pinMode(Led4, OUTPUT);
	Time = new MyTime();
	Time->IntevralSleep = 1000;
}

void LedGpio::LedLoop()
{
	if (Time->CheckTimeEvent()) {
		if (*Inerfece == "eth0") {
			if (Status->StatusIterfece == NOT_CONNECTED)blinc(Led4, 1);
			else if (Status->StatusSocet == NOT_CONNECTED)blinc(Led4, 2);
			else digitalWrite(Led4, 1);

		}
		else {
			if (Status->StatusIterfece == NOT_CONNECTED)blinc(Led3, 1);
			else if (Status->StatusSocet == NOT_CONNECTED)blinc(Led3, 2);
			else digitalWrite(Led3, 1);
		}
		if (Status->StatusBloc)digitalWrite(Led2, HIGH);
		else digitalWrite(Led2, LOW);
		if(Status->StatusRfid1==Wait)blinc(Led1, 2);
		else if(Status->StatusRfid1 == SendServer)digitalWrite(Led1, HIGH);
		else if(Status->StatusRfid1 == EroorRead)blinc(Led1, 3);
	}
	usleep(10000);
}

void LedGpio::blinc(uint8_t pin, uint8_t count)
{
	for (int i = 0; i < count; i++) {
		
		digitalWrite(pin, HIGH);
		delay(100);
		digitalWrite(pin, LOW);
		delay(100);
	}
}
