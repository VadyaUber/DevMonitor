/*
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip> 
#include <sstream>
*/

#include "weilgand.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iomanip> 
#include <algorithm>

using  namespace std;

string RFID::BitVal = "";
int RFID::t = 15;
int RFID::timeout = t;


//string  uint8_to_hex_string(uint8_t *v, const size_t s);
//void * loop(void *);
RFID::RFID()
{

}

void RFID::getData0() {


	BitVal += "1";
	timeout = t;
}

void RFID::getData1() {

	BitVal += "0";
	timeout = t;
}

int RFID::wiegandInit(int d0pin, int d1pin) {
	// Setup wiringPi
	//wiringPiSetup();
	//pinMode(d0pin, INPUT);
	//pinMode(d1pin, INPUT);
	//pullUpDnControl(d0pin, PUD_UP);
	//pullUpDnControl(d1pin, PUD_UP);
	wiringPiISR(d0pin, INT_EDGE_FALLING, &getData0);
	wiringPiISR(d1pin, INT_EDGE_FALLING, &getData1);

}

bool RFID::getParity(uint32_t n)
{
	bool parity = 0;
	while (n) {
		parity ^= (n & 1);
		n >>= 1;
	}
	return (parity);
}


uint32_t RFID::CovnerterToUint() {
	uint32_t CardId = 0;
	bool bit1 = BitVal[0] != '0';
	bool bit2 = BitVal[BitVal.length() - 1] != '0';
	BitVal = BitVal.substr(1, BitVal.length() - 2);
	const char * charts_bit_val = BitVal.c_str();
	for (char c : BitVal)
		CardId = (CardId << 1) | (c - '0');

	uint32_t vl1 = (CardId >> 12);
	uint32_t vl2 = CardId & 0xFFF;

	if ((getParity(vl1) == bit1) && (getParity(vl2) != bit2))
		return CardId;
	else
		return 0;
}


void RFID::wiegand_loop(int d0pin, int d1pin, bool WG26)
{
	Led_RFID = Wait;
	int i;
	wiegandInit(d0pin, d1pin);
	string tmp = "";
	while (1) {
		if (BitVal != "") {
			timeout = timeout - 1;
			usleep(1000);
			if (BitVal.length() > 1 && timeout == 0) {
				//tmp = BitVal + "\r\n";
				//printf(tmp.c_str());
				//printf("%d \r\n ", );
				uint32_t tmp = CovnerterToUint();
				printf("%u \r\n", tmp);
				if (tmp == Oldval && (Led_RFID == AccesTrue))
				{
					Oldval = 0;
					weilgand_id = "";
					wg_reseted = true;
				}
				else
				{
					Oldval = tmp;
					weilgand_id = to_string(tmp);
					wg_readed = true;
				}
				BitVal = "";
			}
		}
		else {
			usleep(10000);
		}
	}

}

void RFID::set_led_state()
{
	if (wg_readed)
	{
		RFID_id = "";
		wg_readed = false;
		wg_upd = true;
		Led_RFID = SendServer;
	}
	if (wg_upd && !(*Serv_RFID))
	{
		RFID_id = weilgand_id;
		wg_sendet = true;
		TimerRFID = new MyTime();
		TimerRFID->IntevralSleep = 4000;
		wg_upd = false;
	}
	if (wg_sendet)
	{
		if (TimerRFID->CheckTimeEvent())
		{
			if (*Serv_RFID)
			{
				Led_RFID = AccesTrue;
			}
			else
			{
				Led_RFID = Wait;
			}
			wg_sendet = false;
		}
		if (*Serv_RFID)
		{
			Led_RFID = AccesTrue;
			wg_sendet = false;
		}
	}
	if (wg_reseted)
	{
		RFID_id = "";
		wg_reseted = false;
		Led_RFID = Wait;
	}

}