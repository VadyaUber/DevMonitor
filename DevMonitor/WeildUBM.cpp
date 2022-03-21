/*----------
	Маленький убм4 встраиваемого типа
*/
#include "WeildUBM.h"


#define CS_SENSOR_I 11
#define CS_SENSOR_V 10
#define CS_METER    9		// в образце используем как индикатор блокировки
#define CICLE_METER 100
#define WILDGANPIN0 16
#define WILDGANPIN1 15

#define CS_DOUT_PIN 3

#define SW_POWER 8
#define RTC_CS 1

#define LED3_PIN 0x01
#define LED4_PIN 0x02
#define BEEPER_PIN 0x04
#define LED2_PIN 0x08
#define LED1_PIN 0x10
#define WG36Pin  0x20 
//bool wg_sendet = false;
WeildUBM::WeildUBM(DevServer * server)
{
	wiringPiSetup();
	UbmServer = server;
	//pullUpDnControl(13, PUD_DOWN);
	//pullUpDnControl(12, PUD_DOWN);
	//pullUpDnControl(14, PUD_UP);
	pinMode(SW_POWER, OUTPUT);
	digitalWrite(SW_POWER, HIGH);
	pinMode(CS_METER, OUTPUT);
	digitalWrite(CS_METER, LOW);
	wiegand = new RFID();
	wiegand->Serv_RFID = &UbmServer->RFID_status;
	error.err = 0;
	Dout = new DigitalOutUbmSPI(CS_DOUT_PIN, LED3_PIN, LED4_PIN, BEEPER_PIN, LED2_PIN, LED1_PIN, WG36Pin);
	Dout->Inerfece = &UbmServer->WeildConfig.interface;
	Dout->Status = &UbmServer->Status;
	Dout->Led_RFID = &wiegand->Led_RFID;
	Dout->UsbBlink = &logtousb.USBblink;

	//Dout->RFID = &UbmServer->RFID_status;
	


	if (UbmServer->WeildConfig.RTC_ON)
	{
		rtc = new Rtc(RTC_CS);
		rtc->GetRtc();
		RtcTime = new MyTime();
		RtcTime->IntevralSleep = 3600000;
	}
	if (UbmServer->WeildConfig.SENSOR_I_ON) {
		I_Sensor = new DevADC(CS_SENSOR_I, true, "/weildpath/modules.xml", "SENSOR_I"/* { 1.586679 ,651.22388 ,1 }*/);
	}
	else {
		pinMode(CS_SENSOR_I, OUTPUT);
		digitalWrite(CS_SENSOR_I, HIGH);
	}
	if (UbmServer->WeildConfig.SENSOR_U_ON) {
		U_Sensor = new DevADC(CS_SENSOR_V, true, "/weildpath/modules.xml", "SENSOR_U"/*{ 1.5828 ,60.882 ,10 }*/);
	}
	else {
		pinMode(CS_SENSOR_V, OUTPUT);
		digitalWrite(CS_SENSOR_V, HIGH);
	}
	/*if (UbmServer->WeildConfig.SENSOR_W_ON) {
		meter = new ElectricMeter(CS_METER, CICLE_METER, "/weildpath/modules.xml", "ELECTRIC_METER");
	}
	else {
		pinMode(CS_METER, OUTPUT);
		digitalWrite(CS_METER, HIGH);
	}*/
	

	if (UbmServer->WeildConfig.RFID_ON) {
		new thread([&]() {
			wiegand->wiegand_loop(WILDGANPIN0, WILDGANPIN1,!UbmServer->WeildConfig.WG35);
			});
	}

	if (UbmServer->WeildConfig.QR_ON)
		qr = new QrDev("/dev/ttyACM0");
	if (UbmServer->WeildConfig.USB_OUT)
		logtousb.Init(UbmServer->WeildConfig.mac);

	if (UbmServer->WeildConfig.MercuryMeter)
	{
		pinMode(4, OUTPUT);
		digitalWrite(4, LOW);
		Mercury = new Merc("/dev/ttyS2", 9600);

		new thread([&]() {Mercury->get_data_merc(); });
	}


	//if (I_Sensor != NULL || U_Sensor != NULL || meter != NULL) {
		new thread([&]() {

			while (true)
			{
				
				if (I_Sensor != NULL) I_Sensor->ReadValue();
				usleep(500);
				if (U_Sensor != NULL) U_Sensor->ReadValue();
				usleep(500);
				if (meter != NULL) meter->ReadValue();
				usleep(500);
				if (UbmServer->WeildConfig.RTC_ON)
				{
					if (RtcTime->CheckTimeEvent() || UbmServer->StatusServerRecv == NEW_DATA) {
						UbmServer->StatusServerRecv = IDEL_DATA;
						rtc->SetRtc();
					}
					if (!rtc->ReadOk) {
						if(rtc->CntError < MAX_ERROR_RTC)
							rtc->GetRtc();
						error.clock = !rtc->ReadOk;
					}
					else 
					{
						error.clock = !rtc->ReadOk;
					}
				}
				Dout->Loop();
				usleep(4000);

			}
			});
	//}
		
	TimerCalculate = new MyTime();
	TimerCalculate->IntevralSleep = 800;

	if (UbmServer->WeildConfig.BlockMode == "OFF" || UbmServer->WeildConfig.BlockMode == "RFID")			/////////////////////	ПРОВЕРИТЬ
		digitalWrite(SW_POWER, LOW);
	else 
		digitalWrite(SW_POWER,HIGH);
	
}

void WeildUBM::UbmLoop()
{
	UbmServer->WeildLoop();
	if (TimerCalculate->CheckTimeEvent()) {
		UbmServer->Perefir = "";
		UbmServer->Perefir.append("01");
		if (I_Sensor != NULL) {
			I_Sensor->CalculateAdc();
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&I_Sensor->Value16Bit,2));
			//printf(" I %d\n\r", I_Sensor->Value16Bit);
		}else UbmServer->Perefir.append("0000");
		if (U_Sensor != NULL) {
			U_Sensor->CalculateAdc();
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&U_Sensor->Value16Bit, 2));
			//printf(" V %d\n\r", U_Sensor->Value16Bit);
		}
		else UbmServer->Perefir.append("0000");
		
		if (meter != NULL) {
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->power_quality, 1));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->SumPowerVa, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t *)&meter->SumEnergyVa, 4));
		}
		else if(Mercury != NULL)
		{
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t*)&Mercury->power_quality, 1));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t*)&Mercury->sumpower, 4));
			UbmServer->Perefir.append(UbmServer->uint8_to_hex_string((uint8_t*)&Mercury->enegj, 4));
		}
		else 
		{
			UbmServer->Perefir.append("0000");
			UbmServer->Perefir.append("00000000");
			UbmServer->Perefir.append("00000000");
		}
		UbmServer->Perefir.append(UbmServer->uint8_to_hex_string(&error.err, 1));

		if (UbmServer->WeildConfig.QR_ON)
			qr->GetQrData(&UbmServer->QrCode);

		if (UbmServer->WeildConfig.RFID_ON)
		{
			wiegand->set_led_state();
			UbmServer->rfid = wiegand->RFID_id;
		}
		
		if (UbmServer->WeildConfig.USB_OUT)
		{
			logtousb.USBconnect();
		}
		if (UbmServer->WeildConfig.BlockMode == "REMOTE") { //clarify and change
			if (I_Sensor != NULL) {
				if (I_Sensor->Value16Bit < UbmServer->WeildConfig.Compare_I) {

					digitalWrite(SW_POWER, UbmServer->PowerOn);
					digitalWrite(CS_METER, !UbmServer->PowerOn);
				}
			}

		}
		if (UbmServer->WeildConfig.BlockMode == "RFID") { //clarify and change
			if (UbmServer->Status.StatusServer==1) //если есть связь с сервером
			{
				if (UbmServer->RFID_status)
				{
					if (I_Sensor != NULL) {
						if (I_Sensor->Value16Bit < UbmServer->WeildConfig.Compare_I) {

							digitalWrite(SW_POWER, UbmServer->PowerOn);
							digitalWrite(CS_METER, !UbmServer->PowerOn);
						}
					}
				}
				else
				{
					digitalWrite(SW_POWER, LOW);
					digitalWrite(CS_METER, HIGH);
				}
			}
			else
			{
				if (wiegand->RFID_id == "")
				{
					digitalWrite(SW_POWER, LOW);
					digitalWrite(CS_METER, HIGH);
				}
				else
				{
					digitalWrite(SW_POWER, HIGH);
					digitalWrite(CS_METER, LOW);
				}
			}
		}
	}
	
	usleep(10000);
}
