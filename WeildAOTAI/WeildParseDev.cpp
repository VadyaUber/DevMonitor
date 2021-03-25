#include "WeildParseDev.h"
#define WILDGANPIN0 1
#define WILDGANPIN1 1
#define WG35Pin  6 
#define LED1 2
#define LED2 3
#define LED3 4
#define LED4 5
WeildParseDev::WeildParseDev(WeildServer * server)
{
	ServerDev = server;
	if (ServerDev->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(WILDGANPIN0, WILDGANPIN1);
			});

	}
	led = new LedGpio(LED1, LED2, LED3, LED4);
	
	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
	port = new ParserPort("/dev/ttyS1");

	new thread([&]() {
		led->LedLoop();
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
	ServerDev->rfid = weilgand_id;
}
