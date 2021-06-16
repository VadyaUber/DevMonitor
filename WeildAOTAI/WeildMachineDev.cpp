#include "WeildMachineDev.h"
#define WILDGANPIN0 16
#define WILDGANPIN1 15
#define WG35Pin  4 
#define LED1 5
#define LED2 7
#define LED3 10
#define LED4 11
#define CS_PIN 4

#define NONE_PIN 255
#define PIN1 1
#define PIN2 2
#define PIN3 3
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7
#define PIN8 8
#define RTC_CS 1
WeildMachineDev::WeildMachineDev(WeildServer * serv_inp)
{
	RtcTime = new MyTime();
	RtcTime->IntevralSleep = 3600000;
	rtc = new Rtc(RTC_CS);
	rtc->GetRtc();
	Digital = new DigitalInput8bit( CS_PIN);
	ServerDev = serv_inp;
	if (ServerDev->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(WILDGANPIN0, WILDGANPIN1, !ServerDev->WeildConfig.WG35);
			});
	}
	led = new LedGpio(LED1, LED2, LED3, LED4);

	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
	new thread([&]() {
		while (true) {
			led->LedLoop();
			//if (RtcTime->CheckTimeEvent() || ServerDev->StatusServerRecv == NEW_DATA) {
			//	ServerDev->StatusServerRecv = IDEL_DATA;

			//	rtc->SetRtc();
			//}
		}

		});


	if (ServerDev->WeildConfig.WG35) {
		digitalWrite(WG35Pin, HIGH);
	}
	else {
		digitalWrite(WG35Pin, LOW);
	}
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
	if(ServerDev->WeildConfig.QR_ON)
		qr = new QrDev("/dev/ttyACM0");
}

void WeildMachineDev::WeildMachineDevLoop()
{
	
	if (TimerCalculate->CheckTimeEvent()) {
		ServerDev->Perefir = "";
		Digital->ReadData();
		if (Digital->IsEnable) {
			ServerDev->Perefir.append("03");
			DigitaData = Digital->Value;
			ServerDev->Perefir.append(ServerDev->uint8_to_hex_string((uint8_t *)&DigitaData,2));

		}
		else {
			ServerDev->Perefir.append("03");
			ServerDev->Perefir.append("0000");
		}
	}
	
	if (RtcTime->CheckTimeEvent() || ServerDev->StatusServerRecv == NEW_DATA) {
		ServerDev->StatusServerRecv = IDEL_DATA;

		rtc->SetRtc();
	}

	ServerDev->WeildLoop();
	if (ServerDev->WeildConfig.QR_ON)
		qr->GetQrData(&ServerDev->QrCode);
	
	if (ServerDev->WeildConfig.RFID_ON)
	{
		ServerDev->rfid = weilgand_id;
	}
	usleep(1000);
}




