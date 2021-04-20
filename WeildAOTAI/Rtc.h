#pragma once
#include <iostream>
#include "SPI.h"
#include "wiringPi.h"
#include <time.h>
#include <sys/time.h>
using namespace std;

class Rtc
{
public:
	Rtc( uint8_t cs_dev);
	void SetRtc();
	void GetRtc();
	
private:
	SPI spi;
	uint8_t CS;
	uint8_t _encode(uint8_t value);
	uint8_t _decode(uint8_t value);
	void write_reg_rtc(uint8_t reg, uint8_t data);
};

