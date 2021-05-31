#pragma once
#include <iostream>
#include "SPI_C.h"
#include "wiringPi.h"
#include <time.h>
#include <sys/time.h>
#include <ctime>
#include <cstring>
#define MAX_ERROR_RTC 50
using namespace std;

class Rtc
{
public:
	Rtc( uint8_t cs_dev);
	void SetRtc();
	void GetRtc();
	bool ReadOk = false;
	int CntError = 0;
private:

	uint8_t CS;
	uint8_t _encode(uint8_t value);
	uint8_t _decode(uint8_t value);
	void write_reg_rtc(uint8_t reg, uint8_t data);
	bool ValidDataRtc();
};

