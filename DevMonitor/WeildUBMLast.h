#pragma once
#include"DevServer.h"

#include "DevADC.h"
#include"ElectricMeter.h"
#include "thread"
#include "wiegand.h"
#include "MyTime.h"
#include"UBMLed.h"
#include "DigitalOutUbmLast.h"
#include "Rtc.h"
class WeildUBMLast
{
public:
	WeildUBMLast(DevServer *server);
	void UbmLoop();
private:
	DevServer * UbmServer;
	
	DevADC * I_Sensor =NULL;
	DevADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	MyTime * TimerCalculate;
	UBMLed * Led;
	DigitalOutUbmLast * Dout;
	Rtc * rtc;
	MyTime * RtcTime;
	RFID* wiegand;
};

