#pragma once
#include <fstream>
#include <bitset>
#include"DevServer.h"
#include"wiringPi.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
#include"DigitalInput8bit.h"
#include"SPI_C.h"
#include"QrDev.h"
#include"Rtc.h"
#include "Mercury.h"
#include "DS1307.h"
#include "LogToUSB.h"
#include "DigitalOutUbmSPI.h"

//work state 

#define WORK 0x01 // Работа
#define IDLE 0x02 // Простой
#define SETUP 0x04 // Наладка
#define FAULT 0x08 // Авария

class MachinePillar
{
public:
	MachinePillar(DevServer* serv_inp);
	void MachinePillarLoop();
private:
	LedGpio* led;
	DevServer* ServerDev;
	MyTime* TimerCalculate;
	//MyTime* Everyhourcheck;
	Rtc* rtc;
	//DS1307* rtc;
	MyTime* RtcTime;
	DigitalInput8bit* Digital;
	uint16_t DigitData = 0;
	uint16_t OldDigitData = 0;
	uint16_t OutDigitData = 0;
	QrDev* qr;
	RFID* wiegand;
	void CheckDataChanged();
	void spisend(uint8_t d1, uint8_t d2);
	bool Datachanged[8] = { 0 };
	uint16_t delaycount[8] = { 0 };
	Merc* Mercury = NULL;
	DevLogUSB USB;
	DigitalOutUbmSPI* Dout;

	SerialPort* Serialrf;
	int num_bytes = 0;
	uint8_t rxbuff[56] = { 0 };

	uint8_t rflag = 0;

	uint64_t dararr[25] = { 0 };
	bool blockstatus = false;
	volatile Rfid_Led_Status Leds_RFID;
	uint32_t waited = 0;
	uint8_t waitserver = 0;

	uint16_t StatusJob = IDLE;
};

