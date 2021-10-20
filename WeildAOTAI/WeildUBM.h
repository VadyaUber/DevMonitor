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
#include "LogToUSB.h"

typedef union {
	struct {
		uint8_t block : 1, clock : 1, adcU : 1, adcI : 1, elmet : 1, log : 1;
	};
	uint8_t err;
}errS;

class WeildUBM
{
public:
	WeildUBM(WeildServer *server);
	void UbmLoop();
	errS error;

private:
	WeildServer * UbmServer;
	WeildADC * I_Sensor =NULL;
	WeildADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	MyTime * TimerCalculate;
	MyTime* TimerRFID;
	DigitalOutUbmSPI* Dout;
	Rtc * rtc;
	MyTime * RtcTime;
	QrDev* qr;
	RFID* wiegand;
	WeildLogUSB logtousb;
	uint8_t USBBlinkCount = 0;
};

