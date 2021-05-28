#include "WeildUBM.h"

#define CS_SENSOR_I 10
#define CS_SENSOR_V 11
#define CS_METER    8
#define CICLE_METER 100
#define WILDGANPIN0 15
#define WILDGANPIN1 16
#define DS_PIN 0
#define SH_PIN 2
#define ST_PIN 3
#define SW_POWE 5
#define RTC_CS 1

#define LED1_PIN 1
#define LED2_PIN 2
#define LED3_PIN 3
#define LED4_PIN 4
#define BEEPER_PIN 5
#define WG35Pin  6 
WeildUBM::WeildUBM(WeildServer * server)
{
	UbmServer = server;
	rtc = new Rtc(RTC_CS);
	rtc->GetRtc();

	pinMode(SW_POWE, OUTPUT);
	RtcTime = new MyTime();
	RtcTime->IntevralSleep = 3600000;
	Led = new UBMLed(DS_PIN, SH_PIN, ST_PIN, UbmServer->WeildConfig.WG35, LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, WG35Pin, BEEPER_PIN);
	if (UbmServer->WeildConfig.SENSOR_I_ON) {
		I_Sensor = new WeildADC(CS_SENSOR_I, true, "/weildpath/modules.xml", "SENSOR_I"/* { 1.586679 ,651.22388 ,1 }*/);
	}
	if (UbmServer->WeildConfig.SENSOR_U_ON) {
		U_Sensor = new WeildADC(CS_SENSOR_V, true, "/weildpath/modules.xml", "SENSOR_U"/*{ 1.5828 ,60.882 ,10 }*/);
	}
	if (UbmServer->WeildConfig.SENSOR_W_ON) {
		meter = new ElectricMeter(CS_METER, CICLE_METER, "/weildpath/modules.xml", "ELECTRIC_METER");
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
				if (RtcTime->CheckTimeEvent() || UbmServer->StatusServerRecv == NEW_DATA) {
					UbmServer->StatusServerRecv = IDEL_DATA;

					rtc->SetRtc();
				}

				usleep(100);

			}
			});
	}
	Led->Inerfece = &UbmServer->WeildConfig.interface;
	Led->Status = &UbmServer->Status;
	new thread([&]() {while (true)Led->Loop(); });
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
	if (UbmServer->WeildConfig.BlockMode == "ON")digitalWrite(SW_POWE,HIGH);
	if (UbmServer->WeildConfig.BlockMode == "OFF")digitalWrite(SW_POWE, HIGH);
}

void WeildUBM::UbmLoop()
{
	UbmServer->WeildLoop();
	if (TimerCalculate->CheckTimeEvent()) {
		UbmServer->Perefir = "";
		UbmServer->Perefir.append("01");
		if (I_Sensor != NULL) {
			I_Sensor->CalculateAdc();
			printf(" I %d\n\r", I_Sensor->Value16Bit);
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&I_Sensor->Value16Bit,2));
		}else UbmServer->Perefir.append("0000");
		if (U_Sensor != NULL) {
			U_Sensor->CalculateAdc();
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&U_Sensor->Value16Bit, 2));
			printf(" V %d\n\r", I_Sensor->Value16Bit);
		}
		else UbmServer->Perefir.append("0000");
		UbmServer->Perefir.append("0000");//дискретные выходы
		
		if (meter != NULL) {
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)meter->status, 3));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)meter->SumPowerVa, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)meter->SumEnergyWat, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)meter->SumEnergyVar, 4));
		}
		else {
			UbmServer->Perefir.append("000000");
			UbmServer->Perefir.append("00000000");
			UbmServer->Perefir.append("00000000");
			UbmServer->Perefir.append("00000000");
		}
	}
	
	if (UbmServer->WeildConfig.BlockMode == "REMOTE") {
		if (I_Sensor->Value16Bit < UbmServer->WeildConfig.Compare_I) {
	
			digitalWrite(SW_POWE, UbmServer->PowerOn);
		}
	}
	
	UbmServer->rfid = weilgand_id;

}
