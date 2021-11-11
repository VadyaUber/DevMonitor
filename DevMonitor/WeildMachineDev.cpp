#include "WeildMachineDev.h"
#include <bitset>
#define WILDGANPIN0 16
#define WILDGANPIN1 15
#define WG35Pin  4 
#define LED1 5
#define LED2 7
#define LED3 10
#define LED4 11
#define CS_PIN 4

#define RTC_CS 1
#define RL0 8
#define RL1 9

WeildMachineDev::WeildMachineDev(WeildServer * serv_inp)
{
	wiringPiSetup();
	digitalWrite(RL0, LOW); //реле выключены
	digitalWrite(RL1, LOW);
	if (ServerDev->WeildConfig.RTC_ON)
	{
		RtcTime = new MyTime();
		RtcTime->IntevralSleep = 3600000;
		rtc = new Rtc(RTC_CS);
		rtc->GetRtc();
	}


	Digital = new DigitalInput8bit( CS_PIN);
	ServerDev = serv_inp;




	if (ServerDev->WeildConfig.RFID_ON) {

		wiegand = new RFID();
		wiegand->Serv_RFID = &ServerDev->RFID_status;

		new thread([&]() {
			wiegand->wiegand_loop(WILDGANPIN0, WILDGANPIN1, !ServerDev->WeildConfig.WG35);
			});
	}
	led = new LedGpio(LED1, LED2, LED3, LED4);

	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
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
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;
	if(ServerDev->WeildConfig.QR_ON)
		qr = new QrDev("/dev/ttyACM0");
}

void WeildMachineDev::CheckDataChanged()
{	
	bitset<8> bsetODD(OutDigitData);
	if (DigitData != OldDigitData)
	{
		for (int i = 0; i < 8; i++)
		{
			if (((DigitData >> i) & 1) != ((OldDigitData >> i) & 1))
			{
				if (!Datachanged[i])
				{
					if (((DigitData >> i) & 1) == 1)
					{
						Datachanged[i] = true;
						bsetODD.set(i);
						OutDigitData = (uint16_t)bsetODD.to_ulong();
					}
					else
					{
						Datachanged[i] = true;
						bsetODD.reset(i);
						OutDigitData = (uint16_t)bsetODD.to_ulong();
					}
				}
			}

		}
		OldDigitData = DigitData;
	}
	for (int i = 0; i < 8; i++)
	{
		if (Datachanged[i])
		{
			if (delaycount[i] > 1500)
			{
				delaycount[i] = 0;
				Datachanged[i] = false;
				//OutDigitData |= ((DigitData >> i) & 1) << i;
				if (((DigitData >> i) & 1) == 1)
					bsetODD.set(i);
				else
					bsetODD.reset(i);
				OutDigitData = (uint16_t)bsetODD.to_ulong();
			}
			else
			{
				delaycount[i]++;
				//OutDigitData |= 1 << i;
			}
		}
	}
	
}


/*
* изменилось несколько значений ( 1бит и 3й бит)
* Datachanged[1,3]=true;
* if datachanged delaycount++;
* OutData = 0,1,0,3,0,0,0,0
* senddelaycount;
* Изменились значения;
* Datachanget add
* if datachanged;
	chechdelaycount;
	if(delaycount>1030)
		datachanged = false;
		outdata = this bit;
 
*/
void WeildMachineDev::WeildMachineDevLoop()
{
	
	//if (TimerCalculate->CheckTimeEvent()) {
		
		Digital->ReadData();
		if (Digital->IsEnable) {
			DigitData = Digital->Value;
			CheckDataChanged();
			ServerDev->Perefir = "";
			ServerDev->Perefir.append("03");
			ServerDev->Perefir.append(ServerDev->uint8_to_hex_string((uint8_t *)&OutDigitData,2));

		}
		else {
			ServerDev->Perefir = "";
			ServerDev->Perefir.append("03");
			ServerDev->Perefir.append("0000");
		}
	//}
	


	
	if (ServerDev->WeildConfig.QR_ON)
		qr->GetQrData(&ServerDev->QrCode);
	
	if (ServerDev->WeildConfig.RFID_ON)
	{
		wiegand->set_led_state();
		ServerDev->rfid = wiegand->RFID_id;
	}
	if (ServerDev->WeildConfig.RTC_ON)
	{
		if (RtcTime->CheckTimeEvent() || ServerDev->StatusServerRecv == NEW_DATA) {
			ServerDev->StatusServerRecv = IDEL_DATA;

			rtc->SetRtc();
		}
	}
	ServerDev->WeildLoop();
	usleep(1000);
}




