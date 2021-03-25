#include"WeildServer.h"
#include"WeildADC.h"
#include"ElectricMeter.h"
#include"UBMLed.h"
#include"ParserPort.h"
#include <iostream> 
#include <thread>
#include "weilgand.h"
using namespace std;
int main()
{
	WeildServer weild("/root/config.xml", "/root/dataweld.txt"); 
	SPI * spi_dev;
	ParserPort * port;
	WeildADC * Sensor_I = NULL;
	WeildADC * Sensor_V = NULL;
	ElectricMeter * meter = NULL;
	MyTime TimerCalculate;
	//Rfid * card;
	//UBMLed Led(1, 2, 4,true);
	//Led.Status = &weild.Status;
	TimerCalculate.IntevralSleep = 500;
	if (weild.WeildConfig.Type_Dev == "ubm") {
		spi_dev = new SPI("/dev/spidev1.0", 5000000, 8, 2);
		if (weild.WeildConfig.SENSOR_W_ON) {
			meter = new ElectricMeter(3, spi_dev, 100);
		}
		if (weild.WeildConfig.SENSOR_I_ON) {
			Sensor_I = new WeildADC(1, spi_dev, true, { 1.5828 ,606 ,1 });
		}
		if (weild.WeildConfig.SENSOR_U_ON) {
			Sensor_V = new WeildADC(2, spi_dev, true, { 1.5828 ,60.882 ,10 });
		}
	}else{
		port = new ParserPort("/dev/ttyS1");
	}
	if (weild.WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand_loop(5, 6);
		});
	}
	if (weild.WeildConfig.Type_Dev == "ubm") {
		new thread([&]() {
			while (true)
			{
				if (Sensor_I != NULL)Sensor_I->ReadValue();
				if (Sensor_V != NULL)Sensor_V->ReadValue();
				if (meter != NULL) meter->ReadValue();

			}
		});
	}
	//thread th_led([&](){Led.Loop();});
	while (true)
	{
		weild.rfid = weilgand_id;
		weild.WeildLoop();
		if (weild.WeildConfig.Type_Dev == "ubm") {
			if (TimerCalculate.CheckTimeEvent()) {
				if (Sensor_I != NULL) {
					Sensor_I->CalculateAdc();
				}
				if (Sensor_V != NULL) {
					Sensor_V->CalculateAdc();
				}
			}

		}
		else {
			port->GetParseData(&weild.DataToServer);
			if (weild.NewDataInput) {
				weild.DataOut.append("\r\n");
				port->WriteData(weild.DataOut);
			}
		}
	}
}