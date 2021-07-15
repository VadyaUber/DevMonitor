#pragma once
#include "WeildServer.h"
#include "SPI_C.h"
#include "WeildADC.h"
#include "ElectricMeter.h"
#include "thread"
#include "weilgand.h"
#include "MyTime.h"
#include "DigitalOutUbmSPI.h"
#include "Rtc.h"
#include "QrDev.h"
class WeildUBM
{
public:
	WeildUBM(WeildServer *server);
	void UbmLoop();
private:
	WeildServer * UbmServer;
	WeildADC * I_Sensor =NULL;
	WeildADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	MyTime * TimerCalculate;
	DigitalOutUbmSPI* Dout;
	Rtc * rtc;
	MyTime * RtcTime;
	QrDev* qr;
};

