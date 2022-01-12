#include "WeildMachineSpotDev.h"
#define LED1 10

#define INPUTPIN 6

WeildMachineSpotDev::WeildMachineSpotDev(DevServer* serv_inp)
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
	new thread([&]() {
		while (true) {
			ServerDev->WeildLoop();
			usleep(50000);
		}

		});
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
	ServerDev->Perefir = "030000";
}

void WeildMachineSpotDev::WeildMachineSpotDevLoop()
{

	//if (TimerCalculate->CheckTimeEvent()) {
	
	
	if (DigitData != OldDigitData)
	{
		if (delaycount > 150)
		{
			delaycount = 0;
			ServerDev->Perefir = "";
			ServerDev->Perefir.append("03");
			ServerDev->Perefir.append(ServerDev->uint8_to_hex_string((uint8_t*)&DigitData, 2));
			OldDigitData = DigitData;
		}
		else
		{
			delaycount++;
		}
	}
	else
	{
		DigitData = digitalRead(INPUTPIN);
	}
	usleep(10000);
}