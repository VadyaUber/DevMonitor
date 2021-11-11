#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <wiringPi.h>
#include "MyTime.h"

enum Rfid_Led_Status
{
	Wait,
	SendServer,
	AccesTrue,
	EroorRead,
};

typedef struct {
	bool LedGreen = false;
	bool LedBlink = true;
	bool LedRed = false;
}Led_status_rfid;


using namespace std;
class RFID
{
	
	public:
		RFID();
		bool RFID_Err = false;
		bool* Serv_RFID;
		string RFID_id = "";
		volatile Rfid_Led_Status Led_RFID;
		void wiegand_loop(int d0pin, int d1pin, bool WG26);
		void set_led_state();
	private:
		string wiegand_id = "";
		MyTime* TimerRFID;
		bool wg_sendet = false;
		bool wg_upd = false;
		bool wg_readed = false;
		bool wg_reseted = false;
		static string BitVal;
		uint32_t Oldval = 0;
		static int t;
		static int timeout;
		static void getData0();
		static void getData1();
		int wiegandInit(int d0pin, int d1pin);
		uint32_t CovnerterToUint();
		bool getParity(uint32_t n);
};

