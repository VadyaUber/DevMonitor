#pragma once
#include <iostream>
#include "MyTime.h"
#include "WeildServer.h"
#include "weilgand.h"
#include "SPI_C.h"
#include "wiringPi.h"
using namespace std;

class DigitalOutUbmSPI
{

public:
	DigitalOutUbmSPI(uint8_t cs_dev, uint8_t LED3_Pin, uint8_t LED4_Pin, uint8_t Beeper_Pin, uint8_t LED2_Pin, uint8_t LED1_Pin, uint8_t WG_36_Pin);
	
	string* Inerfece;
	WeildStatus* Status;
	bool RFID;
	bool RFID_upd;

	bool LED3 = false;
	bool LED4 = false;
	bool Beeper = false;
	bool LED2 = false;
	bool LED1 = false;
	bool WG_36 = false;
	void Loop();
	
private:
	uint64_t LastTime;
	MyTime* Time;
	uint8_t CS;
	uint16_t LastVal = 0;
	//uint16_t value=0xFFFF;
	volatile uint16_t value = 0;
	uint8_t LED3_Pin = 0;
	uint8_t LED4_Pin = 0;
	uint8_t Beeper_Pin = 0;
	uint8_t LED2_Pin = 0;
	uint8_t LED1_Pin = 0;
	uint8_t WG_36_Pin = 0;
	void blinc(uint16_t pin, uint16_t count, uint16_t delay1, uint16_t delay2);
};

