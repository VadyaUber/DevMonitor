#pragma once
#include"WeildServer.h"
#include "SPI.h"
#include "WeildADC.h"
#include"ElectricMeter.h"
#include "thread"
#include "weilgand.h"
#include "MyTime.h"
#include"UBMLed.h"
#include "DigitalOutUbmLast.h"
#include "Rtc.h"
class WeildUBMLast
{
public:
	WeildUBMLast(WeildServer *server);
	void UbmLoop();
private:
	WeildServer * UbmServer;
	
	WeildADC *  I_Sensor ;
	WeildADC *  U_Sensor ;
	ElectricMeter * meter;
	MyTime * TimerCalculate;
	UBMLed * Led;
	DigitalOutUbmLast * Dout;
	Rtc * rtc;
	MyTime * RtcTime;
};

