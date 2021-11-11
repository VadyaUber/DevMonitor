#pragma once
#include"DevServer.h"
#include"ParserPort.h"
#include"wiringPi.h"
#include"wiegand.h"
#include "Rtc.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
class WeildParseDev
{
public:
	WeildParseDev(DevServer * server);
	void WeildParseDevLoop();
private:
	DevServer * ServerDev;
	ParserPort * port;
	LedGpio * led;
	Rtc* rtc;
	MyTime* RtcTime;
	RFID* wiegand;
};

