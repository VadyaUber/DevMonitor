#pragma once
#include <wiringPi.h>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <iomanip> 
#include <sstream> 
#include <functional>
#include <memory.h>
#include <thread>
using namespace std;
#define PIN_0 4 // GPIO Pin 17 | Green cable | Data0
#define PIN_1 5// GPIO Pin 18 | White cable | Data1
#define PIN_SOUND 25 // GPIO Pin 26 | Yellow cable | Sound

#define MAXWIEGANDBITS 34
#define READERTIMEOUT 30000000
#define LEN 256
class Rfid
{
public:
	string Rfid_value= "0000000000";
	bool NewData = false;
	Rfid(int d0pin, int d1pin);
	void getData0(void);
	void getData1(void);
private :
	
	void wiegandReset();
	int wiegandGetPendingBitCount();
	int wiegandReadData(void* data, int dataMaxLen);
	void  loop();
	string uint8_to_hex_string(uint8_t *v, const size_t s);
	unsigned char __wiegandData[MAXWIEGANDBITS];
	unsigned long __wiegandBitCount;
	struct timespec __wiegandBitTime;
};

