#pragma once
#pragma once
#include"WeildServer.h"
#include"wiringPi.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
class WeildMachineSpotDev
{
public:
	WeildMachineSpotDev(WeildServer* serv_inp);
	void WeildMachineSpotDevLoop();
private:
	LedGpio* led;
	WeildServer* ServerDev;
	MyTime* TimerCalculate;
	uint16_t DigitaData = 0;
};
