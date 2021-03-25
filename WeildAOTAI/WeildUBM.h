#pragma once
#include"WeildServer.h"
#include "SPI.h"
#include "WeildADC.h"
#include"ElectricMeter.h"
#include "thread"
#include "weilgand.h"
#include "MyTime.h"
class WeildUBM
{
public:
	WeildUBM(WeildServer *server);
	void UbmLoop();
private:
	WeildServer * UbmServer;
	SPI * SpiDev;
	WeildADC * I_Sensor =NULL;
	WeildADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	MyTime * TimerCalculate;
};
