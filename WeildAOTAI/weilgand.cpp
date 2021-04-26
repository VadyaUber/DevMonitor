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

using  namespace std;




string  uint8_to_hex_string(uint8_t *v, const size_t s);
//void * loop(void *);
string weilgand_id="0000000000";
string BitVal = "";
int t = 15;
int timeout = t;
void getData0(void) {


	BitVal += "1";
	timeout = t;
}

void getData1(void) {

	BitVal += "0";
	timeout = t;
}

int wiegandInit(int d0pin, int d1pin) {
	// Setup wiringPi
	wiringPiSetup();
	//pinMode(d0pin, INPUT);
	//pinMode(d1pin, INPUT);
	//pullUpDnControl(d0pin, PUD_UP);
	//pullUpDnControl(d1pin, PUD_UP);
	wiringPiISR(d0pin, INT_EDGE_FALLING, getData0);
	wiringPiISR(d1pin, INT_EDGE_FALLING, getData1);

}







uint32_t CovnerterToUint() {
	uint32_t CardId = 0;
	BitVal = BitVal.substr(1, BitVal.length() - 2);
	const char * charts_bit_val = BitVal.c_str();
	for (char c : BitVal)
		CardId = (CardId << 1) | (c - '0');
	return CardId;
}


void wiegand_loop(int d0pin, int d1pin, bool WG26)
{
	int i;
	string tmp = "";
	wiegandInit(d0pin, d1pin);
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
				weilgand_id = uint8_to_hex_string((uint8_t *)&tmp, 4);
				BitVal = "";
				
			}
		}
		else {
			usleep(1000);
		}
	}

}


string  uint8_to_hex_string(uint8_t *v, const size_t s) {
	stringstream ss;

	ss << hex << setfill('0');

	for (int i = s - 1; i >= 0; i--) {
		ss << hex << setw(2) << static_cast<int>(v[i]);
	}
	string data = ss.str();
	transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}