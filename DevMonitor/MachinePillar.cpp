#include "MachinePillar.h"

#define WILDGANPIN0 16
#define WILDGANPIN1 15
#define WG35Pin  4 
#define LED1o 5
#define LED2o 7
#define LED3o 10
#define LED4o 11
#define CS_PIN 4

#define RTC_CS 1
#define RL1 9
#define RL2 8
#define RL3 6

#define OE 6
#define LEDNET 10

bool blinkusbs = false;

#define CSL 3 //CS max 7219

void MachinePillar::spisend(uint8_t d1, uint8_t d2) //управление сегментниками
{
	if (NameSPI != dev) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", 100000, 8, 2, dev);

	}
	uint8_t rx[2] = { 0 };
	//digitalWrite(CSL, HIGH);
	//usleep(500);
	SpiWriteRead((uint8_t*)&d1, rx, 1);
	SpiWriteRead((uint8_t*)&d2, rx, 1);
	
	digitalWrite(CSL, LOW);
	usleep(50);
	digitalWrite(CSL, HIGH);
}

MachinePillar::MachinePillar(DevServer* serv_inp)
{
	wiringPiSetup();
	pinMode(RL1, OUTPUT);
	digitalWrite(RL1, LOW); //реле выключены
	pinMode(RL2, OUTPUT);
	//digitalWrite(RL2, LOW); //реле выключены
	pinMode(RL3, OUTPUT);
	digitalWrite(RL3, LOW); //реле выключены

	pullUpDnControl(14, PUD_UP);
	pullUpDnControl(12, PUD_UP);


	//-------------------INIT MAX7219
	pinMode(CSL, OUTPUT);
	digitalWrite(CSL, LOW);
	

	spisend(0x0F, 0x00); //no test
	spisend(0x0B, 0x00);
	spisend(0x09, 0x00);
	spisend(0x0A, 0x0D); //yarkosTb
	spisend(0x0C, 0x01);  //unsleep

	spisend(0x01, 0x0);  //off led
	
	StatusJob = 0; //вкл желтый свет на колонне
	digitalWrite(RL1, LOW); 
	digitalWrite(RL2, HIGH);
	digitalWrite(RL3, LOW); 

	//printf("Led 1 ON \n\r ");
	//digitalWrite(RL1, HIGH); //реле выключены
	//digitalWrite(RL2, LOW); //реле выключены
	//digitalWrite(RL3, LOW); //реле выключены
	//sleep(1);

	//printf("Led 2 ON \n\r ");
	//digitalWrite(RL1, LOW); //реле выключены
	//digitalWrite(RL2, HIGH); //реле выключены
	//digitalWrite(RL3, LOW); //реле выключены
	//sleep(1);
	//printf("Led 3 ON \n\r ");
	//digitalWrite(RL1, LOW); //реле выключены
	//digitalWrite(RL2, LOW); //реле выключены
	//digitalWrite(RL3, HIGH); //реле выключены
	//sleep(1);
	//digitalWrite(RL1, LOW); //реле выключены
	//digitalWrite(RL2, LOW); //реле выключены
	//digitalWrite(RL3, LOW); //реле выключены

	Digital = new DigitalInput8bit(CS_PIN);
	ServerDev = serv_inp;
	if (ServerDev->WeildConfig.RTC_ON)
	{
		RtcTime = new MyTime();
		RtcTime->IntevralSleep = 3600000;
		rtc = new Rtc(RTC_CS);
		rtc->GetRtc();
		if (!rtc->ReadOk)
		{
			usleep(100);
			rtc->GetRtc();
		}
	}


	init_SPI("/dev/spidev1.0", 100000, 8, 2, dev);

	//spisend(0x01, 0x08);//USB
	//sleep(1);
	//spisend(0x01, 0x10); //Авария
	//sleep(1);
	//spisend(0x01, 0x20); //Наладка
	//sleep(1);
	//spisend(0x01, 0x40); //Простой
	//sleep(1);


	USB.Init(ServerDev->WeildConfig.mac, to_string(ServerDev->WeildConfig.postid));

	Leds_RFID = EroorRead;

	if (ServerDev->WeildConfig.RFID_ON) {

	}

	//led = new LedGpio(LED1o, LED2o, LED3o, LED4o);
	led = new LedGpio(LED1o, LED2o, LEDNET, LEDNET); //RFIDBLOCK Check timings
	led->Inerfece = &ServerDev->WeildConfig.interface;
	led->Status = &ServerDev->Status;
	led->UsbBlink = &USB.USBblink;
	led->Led_RFID = &Leds_RFID;
	new thread([&]() {
		while (true) {
			led->LedLoop();
			usleep(50000);
		}
		});

	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 500;

	//Everyhourcheck = new MyTime();
	//Everyhourcheck->IntevralSleep =10000;

	if (ServerDev->WeildConfig.QR_ON)
		qr = new QrDev("/dev/ttyACM0");

	//Mercury = new Merc("/dev/ttyUSB0", 9600); //sudo screen /dev/ttyUSB0 9600
	//new thread([&]() {Mercury->get_data_merc(); });
	Mercury = new Merc("/dev/ttyS2", 9600);///------------------------------------------- Read Mercury current or power 
	new thread([&]() {Mercury->get_data_merc(250); });

ServerDev->rfid = "0000";
Serialrf = new SerialPort("/dev/ttyS1", 9600);
new thread([&]() {
	uint8_t wd = 0;
	while (true)  // ----------- RFID UART
	{
		tcflush(Serialrf->serial_port, TCIOFLUSH);
		size_t received = 0;
		while (received < 13)
		{
			ssize_t r = read(Serialrf->serial_port, rxbuff + received, 13 - received);
			if (r < 0)
			{
				perror("failed to read from port");
				return -1;
			}
			if (r == 0)
			{
				// Timeout
				break;
			}
			received += r;
		}
		if (received)
		{
			wd = 0;
			char summa[3];
			summa[0] = rxbuff[11];
			summa[1] = rxbuff[12];
			long decimal_sum = strtol(summa, NULL, 16);
			uint8_t CRC = 0;
			for (uint8_t i = 1; i <= 10; i++) //calculate crc with XOR
			{
				summa[0] = rxbuff[i++];
				summa[1] = rxbuff[i];
				long crc_sum = strtol(summa, NULL, 16);
				CRC ^= crc_sum;
			}

			/*printf("1 = %d\n", decimal_sum);
			printf("2 = %d\n", CRC);*/

			if (decimal_sum == CRC) //CRC ok?
			{
				string hex_str = "";
				for (uint8_t i = 3; i <= 10; i++)
				{
					hex_str += (char)rxbuff[i];
				}
				unsigned int decimal;
				stringstream my_ss;
				my_ss << hex << hex_str;
				my_ss >> decimal;
				ServerDev->rfid = to_string(decimal);
				rflag = true;
			}printf("OK");

			//printf("recv = %s \n", rxbuff);
		}
		else //
		{
			wd++;
			if (wd > 4)
			{
				wd = 0;
				ServerDev->rfid = "0000";
				rflag = false;
			}
		}
		usleep(200);
	}

	});




}



void MachinePillar::CheckDataChanged()
{
	if (Mercury->sumcurrent > ServerDev->WeildConfig.Power_max_run)
	{
		StatusJob = WORK;
		printf("Work \n\r ");
		digitalWrite(RL1, HIGH); //реле выключены
		digitalWrite(RL2, LOW); //реле выключены
		digitalWrite(RL3, LOW); //реле выключены
		spisend(0x01, 0x00);
	}
	else if ((Mercury->sumcurrent < ServerDev->WeildConfig.Power_min_wait) && StatusJob==WORK)
	{
		StatusJob = 0;
		printf("Work ended \n\r ");
		digitalWrite(RL1, LOW); //реле выключены
		digitalWrite(RL2, HIGH); //реле выключены
		digitalWrite(RL3, LOW); //реле выключены
	}
	if (StatusJob != WORK)
	{
		/*
			нажата 1я кнопка
			обнулили 3 светодиода и зажгли первый светодиод
			другие не смотрим

			нажата 2я кнопка
			обнулили 3 светодиода и зажгли 2й светодиод

		*/
		bitset<8> bsetODD(OutDigitData);
		if (DigitData != OldDigitData)
		{
			if (!((DigitData >> 2) & 1))
			{
				StatusJob = FAULT;
				printf("Led 3 ON \n\r ");
				digitalWrite(RL1, LOW); //реле выключены
				digitalWrite(RL2, LOW); //реле выключены
				digitalWrite(RL3, HIGH); //реле выключены
				spisend(0x01, 0x10); //Авария
			}
			else if ((DigitData >> 1) & 1)
			{
				StatusJob = SETUP;
				printf("Led 2 ON \n\r ");
				digitalWrite(RL1, LOW); //реле выключены
				digitalWrite(RL2, HIGH); //реле выключены
				digitalWrite(RL3, LOW); //реле выключены
				spisend(0x01, 0x20); //Наладка
			}
			else if (((DigitData >> 0) & 1))
			{
				StatusJob = IDLE;
				printf("Led 1 ON \n\r ");
				digitalWrite(RL1, LOW); //реле выключены
				digitalWrite(RL2, HIGH); //реле выключены
				digitalWrite(RL3, LOW); //реле выключены
				spisend(0x01, 0x40); //Простой
			}
			//spisend(0x01, StatusJob);
			//spisend(0x01, 0x10); //Авария
			//sleep(1);
			//spisend(0x01, 0x20); //Наладка
			//sleep(1);
			//spisend(0x01, 0x40); //Простой
			//sleep(1);
			OldDigitData = DigitData;
		}
	}

}




void MachinePillar::MachinePillarLoop()
{
	//////rflag = *&Mercury->errorread;
	//////if (rflag) ServerDev->rfid=*&Mercury->RFID;
	//////else ServerDev->rfid = "0000";
	//if (ServerDev->RFID_status) led->RFID = true;
	//else led->RFID = false;

	if (TimerCalculate->CheckTimeEvent()) {

	if (ServerDev->Status.StatusSocet == CONNECTED)
	{
			if (rflag)
			{
				if (waited > 8)
					Leds_RFID = EroorRead;
				else
					Leds_RFID = SendServer;
				if (ServerDev->RFID_status)
				{
					//blocked(true);
					Leds_RFID = AccesTrue;
					waited = 0;
				}
				else
				{
					if (waitserver != 0) //если авторизация уже была по списку из файла ждём от сервера 10/2 сек ответа на авторизацию
					{
						waitserver--;
					}
					else
					{
						//blocked(false);
						waited++;
					}
				}
			}
			else
			{
				//blocked(false);
				Leds_RFID = Wait;
				waited = 0;
			}
	}
	else if (rflag) //нет связи с сервером и карта вставлена зажигаем светодиод и пишем номер карты в лог
	{
		//set green RFID led 
	}
	else
	{
		//blocked(false);
		Leds_RFID = Wait;
		waitserver = 0;
	}

	

		Digital->ReadData();
	if (Digital->IsEnable) {
		DigitData = Digital->Value;
		//printf("Dt = %02x \n\r", DigitData);
		CheckDataChanged();
		ServerDev->Perefir = "";
		ServerDev->Perefir.append("03");
		ServerDev->Perefir.append(ServerDev->uint8_to_hex_string((uint8_t*)&StatusJob, 2));  /// edit for send status workersd

	}
	else {
		ServerDev->Perefir = "";
		ServerDev->Perefir.append("03");
		ServerDev->Perefir.append("0000");
	}

	USB.USBconnect();
	if (USB.USBblink)
	{
		if (blinkusbs)spisend(0x01, 0x08);
		else spisend(0x01, 0x00);
		
		blinkusbs = !blinkusbs;
		
	}

	}




	if (ServerDev->WeildConfig.QR_ON)
		qr->GetQrData(&ServerDev->QrCode);

	if (ServerDev->WeildConfig.RTC_ON)
	{
		if (RtcTime->CheckTimeEvent() || ServerDev->StatusServerRecv == NEW_DATA) {
			ServerDev->StatusServerRecv = IDEL_DATA;

			rtc->SetRtc();
		}
	}
	////////if (ServerDev->Status.StatusIdle)
	////////{
	////////	//update access_cards.txt
	////////	usleep(10);
	////////}
	ServerDev->WeildLoop();
	usleep(100000);
}




