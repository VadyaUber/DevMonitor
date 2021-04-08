#include "WeildUBMLast.h"

#define CS_SENSOR_I 11
#define CS_SENSOR_V 10
#define CS_METER    9
#define CICLE_METER 100
#define WILDGANPIN0 4
#define WILDGANPIN1 5
#define DS_PIN 1
#define SH_PIN 2
#define ST_PIN 3



#define LED1_PIN 1
#define LED2_PIN 2
#define LED3_PIN 3
#define LED4_PIN 4
#define BEEPER_PIN 5
#define WG35Pin  6 
WeildUBMLast::WeildUBMLast(WeildServer * server)
{
	UbmServer = server;
	
	
	if (UbmServer->WeildConfig.SENSOR_I_ON) {
		I_Sensor = new WeildADC(CS_SENSOR_I, true, { 1.586679 ,651.22388 ,1 });
	}
	if (UbmServer->WeildConfig.SENSOR_U_ON) {
		U_Sensor = new WeildADC(CS_SENSOR_V, true, { 1.5828 ,60.882 ,10 });
	}
	if (UbmServer->WeildConfig.SENSOR_W_ON) {
		meter = new ElectricMeter(CS_METER, CICLE_METER);

	}
	

	if (UbmServer->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(WILDGANPIN0, WILDGANPIN1,!UbmServer->WeildConfig.WG35);
			});
	}
	if (I_Sensor != NULL || U_Sensor != NULL || meter != NULL) {
		new thread([&]() {

			while (true)
			{
				if (I_Sensor != NULL) I_Sensor->ReadValue();
				if (U_Sensor != NULL) U_Sensor->ReadValue();
				if (meter != NULL) meter->ReadValue();
				usleep(100);

			}
			});
	}

	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
}

void WeildUBMLast::UbmLoop()
{
	UbmServer->WeildLoop();
	if (TimerCalculate->CheckTimeEvent()) {
		UbmServer->Perefir = "";
		UbmServer->Perefir.append("01");
		if (I_Sensor != NULL) {
			I_Sensor->CalculateAdc();
		
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&I_Sensor->Value16Bit, 2));
		}
		else UbmServer->Perefir.append("0000");
		if (U_Sensor != NULL) {
			U_Sensor->CalculateAdc();
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&U_Sensor->Value16Bit, 2));
		}
		else UbmServer->Perefir.append("0000");
		UbmServer->Perefir.append("0000");//���������� ������

		if (meter != NULL) {
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->status, 3));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->SumPowerVa, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->SumEnergyWat, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->SumEnergyVar, 4));

		}
		else {
			UbmServer->Perefir.append("000000");
			UbmServer->Perefir.append("00000000");
			UbmServer->Perefir.append("00000000");
			UbmServer->Perefir.append("00000000");
		}
	}

	usleep(100);


}
