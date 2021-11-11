#pragma once
#include"DevServer.h"
#include"wiringPi.h"
#include"wiegand.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
#include"DigitalInput8bit.h"
#include"SPI_C.h"
#include"QrDev.h"
#include"Rtc.h"
class WeildMachineDev
{
public:
	WeildMachineDev(DevServer * serv_inp);
	void WeildMachineDevLoop();
private:
	LedGpio * led;
	DevServer * ServerDev;
	MyTime * TimerCalculate;
	Rtc* rtc;
	MyTime * RtcTime;
	DigitalInput8bit * Digital;
	uint16_t DigitData = 0;
	uint16_t OldDigitData = 0;
	uint16_t OutDigitData = 0;
	QrDev * qr;
	RFID* wiegand;
	void CheckDataChanged();
	bool Datachanged[8] = { 0 };
	uint16_t delaycount[8] = { 0 };
};

