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
	digitalWrite(CS, LOW);
	init_SPI("/dev/spidev1.0", 2000000, 8, 2, dev);

	uint8_t rx[2] = { 0 };
	this->value = 0;
	SpiWriteRead((uint8_t*)&this->value, rx, 2);
	digitalWrite(CS, LOW);
	digitalWrite(CS, HIGH);
	digitalWrite(CS, LOW);
}

void DigitalOutUbmSPI::Loop()
{		
	if (firststart)
	{
		firststart = false;
		this->value |= this->LED4_Pin << 8;
		this->value |= this->LED3_Pin << 8;

		uint8_t rx[2] = { 0 };

		if (NameSPI != dev) {
			DeInitSPI();
			init_SPI("/dev/spidev1.0", 2000000, 8, 2, dev);
		}


		SpiWriteRead((uint8_t*)&this->value, rx, 2);
		digitalWrite(CS, LOW);
		digitalWrite(CS, HIGH);
		digitalWrite(CS, LOW);

		sleep(3);
		this->value &= ~this->LED4_Pin << 8;
		this->value &= ~this->LED3_Pin << 8;
	}

	if (*Inerfece == "eth0") {
		if(UsbBlink)blinc(LED4_Pin, 1, 200, 200);
		else
		{
			if (Status->StatusIterfece == NOT_CONNECTED)blinc(LED4_Pin, 1, 200, 800);
			else if ((Status->StatusSocet == NOT_CONNECTED) || (Status->StatusServer == NOT_CONNECTED))blinc(LED4_Pin, 1, 500, 500);
			else this->value |= this->LED4_Pin << 8;
		}

	}
	else {
		if (*UsbBlink)blinc(LED3_Pin, 1, 100, 100);
		else
		{
			if (Status->StatusIterfece == NOT_CONNECTED) blinc(LED3_Pin, 1, 200, 800);
			else if ((Status->StatusSocet == NOT_CONNECTED) || (Status->StatusServer == NOT_CONNECTED)) blinc(LED3_Pin, 1, 500, 500);
			else this->value |= this->LED3_Pin << 8;
		}
	}
	if ((Status->StatusIterfece == CONNECTED) && (Status->StatusSocet == CONNECTED) && (Status->StatusServer == CONNECTED))
	{
		if (*Led_RFID == AccesTrue)
		{
			this->value |= this->LED1_Pin << 8;
		}
		else if (*Led_RFID == SendServer)
		{
			blinc(LED1_Pin, 1, 500, 500);
		}
		else if (*Led_RFID == Wait)
		{
			blinc(LED1_Pin, 1, 200, 800);
		}
	}
	else this->value &= ~this->LED1_Pin << 8;

	//if (this->Beeper)this->value |= this->Beeper_Pin << 8;
	//else this->value &= ~this->Beeper_Pin << 8;

	//if (this->LED2)this->value |= this->LED2_Pin << 8;
	//else this->value &= ~this->LED2_Pin << 8;

	//if (this->LED1)this->value |= this->LED1_Pin << 8;
	//else this->value &= ~this->LED1_Pin << 8;

	//if (this->WG_36)this->value |= this->WG_36_Pin << 8;
	//else this->value &= ~this->WG_36_Pin << 8;

	if (this->LastVal != this->value) {
		this->LastVal = this->value;
		uint8_t rx[2] = { 0 };

		if (NameSPI != dev) {
			DeInitSPI();
			init_SPI("/dev/spidev1.0", 2000000, 8, 2, dev);
		}

			
		SpiWriteRead((uint8_t*)&this->value, rx, 2);
		digitalWrite(CS, LOW);
		digitalWrite(CS, HIGH);
		digitalWrite(CS, LOW);
	}
	usleep(100);
	
}

void DigitalOutUbmSPI::RFIDLoop()
{
	this->value &= ~this->LED4_Pin << 8;
	if (*Led_RFID == AccesTrue)
	{
		this->value |= this->LED3_Pin << 8;
	}
	else if (*Led_RFID == SendServer)
	{
		blinc(LED3_Pin, 1, 500, 500);
	}
	else if (*Led_RFID == Wait)
	{
		blinc(LED3_Pin, 1, 200, 800);
	}
	else if (*Led_RFID == EroorRead)
	{
		this->value |= this->LED4_Pin << 8;
		this->value &= ~this->LED3_Pin << 8;
	}

	if (this->LED2)this->value |= this->LED2_Pin << 8;
	else this->value &= ~this->LED2_Pin << 8;

	if (this->LED1)this->value |= this->LED1_Pin << 8;
	else this->value &= ~this->LED1_Pin << 8;


	if (this->LastVal != this->value) {
		this->LastVal = this->value;
		uint8_t rx[2] = { 0 };

		if (NameSPI != dev) {
			DeInitSPI();
			init_SPI("/dev/spidev1.0", 2000000, 8, 2, dev);
		}


		SpiWriteRead((uint8_t*)&this->value, rx, 2);
		digitalWrite(CS, LOW);
		digitalWrite(CS, HIGH);
		digitalWrite(CS, LOW);
	}
	usleep(100);
}

void DigitalOutUbmSPI::blinc(uint16_t pin, uint16_t count, uint16_t delay1, uint16_t delay2)
{
	uint8_t rx[2] = { 0 };
	if (Time->CheckTimeEvent()) {
		if ((this->value &= ~pin << 8) == this->LastVal)
		{
				Time->IntevralSleep = delay1+10;
				this->value |= pin << 8;
		}
		else
		{
				Time->IntevralSleep = delay2+10;
				this->value &= ~pin << 8;
		}
	}
}