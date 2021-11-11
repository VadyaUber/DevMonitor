#pragma once
#include"WeildServer.h"

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
	
	WeildADC * I_Sensor =NULL;
	WeildADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	MyTime * TimerCalculate;
	UBMLed * Led;
	DigitalOutUbmLast * Dout;
	Rtc * rtc;
	MyTime * RtcTime;
	RFID* wiegand;
};

