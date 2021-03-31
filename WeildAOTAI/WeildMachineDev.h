#pragma once
#include"WeildServer.h"
#include"wiringPi.h"
#include"wiringPi.h"
#include"weilgand.h"
#include <thread>
#include "LedGpio.h"
#include "MyTime.h"
#include"DigitalInput8bit.h"
#include"SPI.h"
class WeildMachineDev
{
public:
	WeildMachineDev(WeildServer * serv_inp);
	void WeildMachineDevLoop();
private:
	LedGpio * led;
	WeildServer * ServerDev;
	MyTime * TimerCalculate;
	SPI * spi;
	DigitalInput8bit * Digital;
	uint16_t DigitaData = 0;
	
};

