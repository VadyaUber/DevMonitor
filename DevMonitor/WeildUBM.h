#pragma once
#include "DevServer.h"
#include "SPI_C.h"
#include "DevADC.h"
#include "ElectricMeter.h"
#include "thread"
#include "wiegand.h"
#include "MyTime.h"
#include "DigitalOutUbmSPI.h"
#include "Rtc.h"
#include "QrDev.h"
#include "LogToUSB.h"
#include "Mercury.h"

typedef union {
	struct {
		uint8_t block : 1, clock : 1, adcU : 1, adcI : 1, elmet : 1, log : 1;
	};
	uint8_t err;
}errS;

class WeildUBM
{
public:
	WeildUBM(DevServer *server);
	void UbmLoop();
	errS error;

private:
	DevServer * UbmServer;
	DevADC * I_Sensor =NULL;
	DevADC * U_Sensor = NULL;
	ElectricMeter * meter = NULL;
	Merc* Mercury = NULL;
	MyTime * TimerCalculate;
	MyTime* TimerRFID;
	DigitalOutUbmSPI* Dout;
	Rtc * rtc;
	MyTime * RtcTime;
	QrDev* qr;
	RFID* wiegand;
	DevLogUSB logtousb;
	uint8_t USBBlinkCount = 0;
};

