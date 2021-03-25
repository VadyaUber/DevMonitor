#include "WeildUBM.h"

#define CS_SENSOR_I 1
#define CS_SENSOR_V 2
#define CS_METER    4
#define CICLE_METER 100
#define WILDGANPIN0 1
#define WILDGANPIN1 1
#define WG35Pin  6 
WeildUBM::WeildUBM(WeildServer * server)
{
	UbmServer = server;
	SpiDev = new SPI("/dev/spidev1.0", 5000000, 8, 2);
	if (UbmServer->WeildConfig.SENSOR_I_ON) {
		I_Sensor = new WeildADC(CS_SENSOR_I, SpiDev, true, { 1.5828 ,606 ,1 });
	}
	if (UbmServer->WeildConfig.SENSOR_U_ON) {
		U_Sensor = new WeildADC(CS_SENSOR_V, SpiDev, true, { 1.5828 ,60.882 ,10 });
	}
	if (UbmServer->WeildConfig.SENSOR_W_ON) {
		meter = new ElectricMeter(CS_METER, SpiDev, CICLE_METER);

	}
	if (UbmServer->WeildConfig.WG35) {
		digitalWrite(WG35Pin, HIGH);
	}
	else {
		digitalWrite(WG35Pin, LOW);
	}
	if (UbmServer->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(WILDGANPIN0, WILDGANPIN1);
			});
	}
	new thread([&]() {
			while (true)
			{
				if (I_Sensor!=NULL) I_Sensor->ReadValue();
				if (U_Sensor!= NULL) U_Sensor->ReadValue();
				if (meter!=NULL) meter->ReadValue();
			}
		});
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
}

void WeildUBM::UbmLoop()
{
	UbmServer->WeildLoop();
	if (TimerCalculate->CheckTimeEvent()) {
		if (I_Sensor != NULL) I_Sensor->CalculateAdc();
		if (U_Sensor != NULL) U_Sensor->CalculateAdc();
	}
	UbmServer->rfid = weilgand_id;

}
