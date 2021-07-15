#include "DigitalOutUbmSPI.h"
DigitalOutUbmSPI::DigitalOutUbmSPI(uint8_t cs_dev, uint8_t LED3_Pin, uint8_t LED4_Pin, uint8_t Beeper_Pin, uint8_t LED2_Pin, uint8_t LED1_Pin, uint8_t WG_36_Pin)
{
	Time = new MyTime();
	Time->IntevralSleep = 500;

	this->LED3_Pin = LED3_Pin;
	this->LED4_Pin = LED4_Pin;
	this->Beeper_Pin = Beeper_Pin;
	this->LED2_Pin = LED2_Pin;
	this->LED1_Pin = LED1_Pin;
	this->WG_36_Pin = WG_36_Pin;

	//wiringPiSetup();
	CS = cs_dev;
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
	//spi = SPI("/dev/spidev1.0", 1000000, 8, 1);
	init_SPI("/dev/spidev1.0", 2000000, 8, 3, DigOutUbm);
}

void DigitalOutUbmSPI::Loop()
{
		if (*Inerfece == "eth0") {
			if (Status->StatusIterfece == NOT_CONNECTED)blinc(LED4_Pin, 1, 200, 800);
			else if (Status->StatusSocet == NOT_CONNECTED)blinc(LED4_Pin, 1, 500,500);
			else this->value |= this->LED4_Pin << 8;

		}
		else {
			if (Status->StatusIterfece == NOT_CONNECTED)blinc(LED3_Pin, 1, 200, 800);
			else if (Status->StatusSocet == NOT_CONNECTED)blinc(LED3_Pin, 1, 500, 500);
			else this->value |= this->LED3_Pin << 8;
		}
		//if (Status->StatusSocet != NOT_CONNECTED && wg_upd == true)
		if (wg_upd == true)
		{
			blinc(LED1_Pin, 1 ,400,400);
			//wg_upd = false;
		}
		else
			//wg_upd = false;

		//if (this->LED3)this->value |= this->LED3_Pin << 8;
		//else this->value &= ~this->LED3_Pin << 8;

		//if (this->LED4)this->value |= this->LED4_Pin << 8;
		//else this->value &= ~this->LED4_Pin << 8;

		if (this->Beeper)this->value |= this->Beeper_Pin << 8;
		else this->value &= ~this->Beeper_Pin << 8;

			//if (this->LED2)this->value |= this->LED2_Pin << 8;
			//else this->value &= ~this->LED2_Pin << 8;

		//if (this->LED1)this->value |= this->LED1_Pin << 8;
		//else this->value &= ~this->LED1_Pin << 8;

		if (this->WG_36)this->value |= this->WG_36_Pin << 8;
		else this->value &= ~this->WG_36_Pin << 8;

		if (NameSPI != DigOutUbm) {
			DeInitSPI();
			init_SPI("/dev/spidev1.0", 2000000, 8, 3, DigOutUbm);

		}

		if (this->LastVal != this->value) {
			this->LastVal = this->value;
			uint8_t rx[2] = { 0 };
			digitalWrite(CS, LOW);
			SpiWriteRead((uint8_t*)&this->value, rx, 2);
			digitalWrite(CS, HIGH);
		}

		//if (this->LastVal != this->value) {
		//	this->LastVal = this->value;
		//	Write((uint8_t*)&this->value, 2);
		//}
	
}

void DigitalOutUbmSPI::blinc(uint16_t pin, uint16_t count, uint16_t delay1, uint16_t delay2)
{
	//for (int i = 0; i < count; i++) {
	uint8_t rx[2] = { 0 };
	if (Time->CheckTimeEvent()) {
		if ((this->value &= ~pin << 8) == this->LastVal)
		{
			//if (Time->GetMilis() - LastTime > delay1 + 100000) {
			//if (Time->CheckTimeEvent()) {
				Time->IntevralSleep = delay1+10;
				//LastTime = Time->GetMilis();
				this->value |= pin << 8;
				digitalWrite(CS, LOW);
				SpiWriteRead((uint8_t*)&this->value, rx, 2);
				digitalWrite(CS, HIGH);

				//delay(delay1);
				this->LastVal = this->value;

			//}
		}
		else
		{
			//if (Time->GetMilis() - LastTime > delay2 + 100000) {
			//if (Time->CheckTimeEvent()) {
				Time->IntevralSleep = delay2+10;
				this->value &= ~pin << 8;
				//LastTime = Time->GetMilis();
				digitalWrite(CS, LOW);
				SpiWriteRead((uint8_t*)&this->value, rx, 2);
				digitalWrite(CS, HIGH);

				//delay(delay2);
				this->LastVal = this->value;
			//}
		}
	}
	//}
}