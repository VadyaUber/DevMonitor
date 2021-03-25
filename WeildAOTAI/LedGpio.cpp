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
}

void LedGpio::LedLoop()
{
	if (Time.CheckTimeEvent()) {
		if (*Inerfece == "eth0") {
			if (!Status->StatusIterfece)blinc(Led4, 1);
			else if (!Status->StatusSocet)blinc(Led4, 2);
			else digitalWrite(Led4, 1);

		}
		else {
			if (!Status->StatusIterfece)blinc(Led3, 1);
			else if (!Status->StatusSocet)blinc(Led3, 2);
			else digitalWrite(Led3, 1);
		}


	}
}

void LedGpio::blinc(uint8_t pin, uint8_t count)
{
	for (int i = 0; i < count; i++) {
		
		digitalWrite(pin, HIGH);
		delay(100);
		digitalWrite(pin, LOW);
	}
}
