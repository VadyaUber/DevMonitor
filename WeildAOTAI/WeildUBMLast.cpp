#include "WeildUBMLast.h"

#define CS_SENSOR_I 11
#define CS_SENSOR_V 10
#define CS_METER    9
#define CICLE_METER 100
#define WILDGANPIN0 5
#define WILDGANPIN1 4
#define RTC_CS 1

#define DS_PIN 0
#define SH_PIN 2
#define ST_PIN 3


#define RST_CLK_PIN 0x01
#define POWER_PIN   0x02
#define BEEEPER_PIN 0x08
#define LED1_PIN    0x20
#define LED2_PIN    0x40
#define WG35Pin		0x80 
WeildUBMLast::WeildUBMLast(WeildServer * server)
{
	UbmServer = server;
	rtc = new Rtc(RTC_CS);
	rtc->GetRtc();
	RtcTime = new MyTime();
	RtcTime->IntevralSleep = 3600000;
	Dout = new DigitalOutUbmLast(DS_PIN, SH_PIN, ST_PIN, POWER_PIN, BEEEPER_PIN, RST_CLK_PIN, WG35Pin);
	if (UbmServer->WeildConfig.SENSOR_I_ON) {
		I_Sensor = new WeildADC(CS_SENSOR_I, true, "/weildpath/modules.xml","SENSOR_I");
	}
	if (UbmServer->WeildConfig.SENSOR_U_ON) {
		U_Sensor = new WeildADC(CS_SENSOR_V, true,"/weildpath/modules.xml" , "SENSOR_U");
	}
	if (UbmServer->WeildConfig.SENSOR_W_ON) {
		meter = new ElectricMeter(CS_METER, CICLE_METER, "/weildpath/modules.xml", "ELECTRIC_METER");
	}
	

	if (UbmServer->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(WILDGANPIN0, WILDGANPIN1,!UbmServer->WeildConfig.WG35);
			});
	}
	new thread([&]() {
		while (true)
		{
			Dout->Loop();
			usleep(10000);
		}
		});
	if (I_Sensor != NULL || U_Sensor != NULL || meter != NULL) {
		new thread([&]() {

			while (true)
			{
				if (I_Sensor != NULL) I_Sensor->ReadValue();
				if (U_Sensor != NULL) U_Sensor->ReadValue();
				if (meter != NULL) meter->ReadValue();
				if (RtcTime->CheckTimeEvent() || UbmServer->StatusServerRecv==NEW_DATA) {
					UbmServer->StatusServerRecv = IDEL_DATA;

					rtc->SetRtc();
				}

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
	//Dout->Loop();
	//printf("PowerOn %d\n\r", Dout->PowerOn);
	if (UbmServer->WeildConfig.BlockMode == "ON")Dout->PowerOn = true;
	if (UbmServer->WeildConfig.BlockMode == "OFF")Dout->PowerOn = true;
	if (UbmServer->WeildConfig.BlockMode == "REMOTE") {
		if (I_Sensor->Value16Bit < UbmServer->WeildConfig.Compare_I) {
			Dout->PowerOn = UbmServer->PowerOn;
		}
	}
	
	usleep(100);


}
