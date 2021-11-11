#include "WeildParseDev.h"
#define WILDGANPIN0 15
#define WILDGANPIN1 16
#define WG35Pin  4 
#define LED1 5
#define LED2 6
#define LED3 10
#define LED4 11
#define RTC_CS 1
WeildParseDev::WeildParseDev(WeildServer * server)
{
	wiringPiSetup();
	rtc = new Rtc(RTC_CS);
	rtc->GetRtc();

	RtcTime = new MyTime();
	RtcTime->IntevralSleep = 3600000;
	//RtcTime->IntevralSleep = 60000;
	ServerDev = server;
	if (ServerDev->WeildConfig.RFID_ON) 
	{
		wiegand = new RFID();
		wiegand->Serv_RFID = &ServerDev->RFID_status;
		new thread([&]() {
			wiegand->wiegand_loop(WILDGANPIN0, WILDGANPIN1,!ServerDev->WeildConfig.WG35);
			});

	}
	led = new LedGpio(LED1, LED2, LED3, LED4);
	
	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
	port = new ParserPort("/dev/ttyS2");

	new thread([&]() {
		while (true) {
			led->LedLoop();
		}
		
		});
	if (ServerDev->WeildConfig.WG35) {
		digitalWrite(WG35Pin, HIGH);
	}
	else {
		digitalWrite(WG35Pin, LOW);
	}
}

void WeildParseDev::WeildParseDevLoop()
{	
	ServerDev->WeildLoop();
	port->GetParseData(&(ServerDev->UartPackage));
	if (ServerDev->NewDataInput) {
		port->WriteData(ServerDev->DataOut);
		ServerDev->NewDataInput = false;
	}
	if (ServerDev->WeildConfig.RFID_ON)
	{
		wiegand->set_led_state();
		ServerDev->rfid = wiegand->RFID_id;
	}

	if (RtcTime->CheckTimeEvent() || ServerDev->StatusServerRecv == NEW_DATA) {
		ServerDev->StatusServerRecv = IDEL_DATA;
		rtc->SetRtc();
	}
	usleep(1000);
}
