#pragma once
#pragma once
#include"DevServer.h"
#include"wiringPi.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
class WeildMachineSpotDev
{
public:
	WeildMachineSpotDev(DevServer* serv_inp);
	void WeildMachineSpotDevLoop();
private:
	LedGpio* led;
	DevServer* ServerDev;
	MyTime* TimerCalculate;
	uint16_t DigitData = 0;
	uint16_t OldDigitData = 2;
	uint16_t delaycount = 0;
};
