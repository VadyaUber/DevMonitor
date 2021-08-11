#include "WeildMachineSpotDev.h"
#define LED1 10

#define INPUTPIN 6

WeildMachineSpotDev::WeildMachineSpotDev(WeildServer* serv_inp)
{
	wiringPiSetup();
	pinMode(INPUTPIN, INPUT);
	ServerDev = serv_inp;
	led = new LedGpio(0, 0, LED1, LED1);
	
	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
	new thread([&]() {
		while (true) {
			led->LedLoop();
		}

		});

	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
}

void WeildMachineSpotDev::WeildMachineSpotDevLoop()
{

	if (TimerCalculate->CheckTimeEvent()) {
			ServerDev->Perefir = "";
			ServerDev->Perefir.append("03");
			DigitaData = digitalRead(INPUTPIN);
			ServerDev->Perefir.append(ServerDev->uint8_to_hex_string((uint8_t*)&DigitaData, 2));
	}


	ServerDev->WeildLoop();
	usleep(10000);
}