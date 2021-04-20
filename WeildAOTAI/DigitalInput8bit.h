#pragma once
#include <iostream>
#include "wiringPi.h"
#include "SPI.h"
using namespace std;
class DigitalInput8bit
{
public:
	DigitalInput8bit(uint8_t CS_pin);
	void ReadData();
	uint8_t Value;
	bool IsEnable=false;
	bool Over_Temper = false;
	bool Falult_Voltage = false;
private:
	uint8_t CS;
	int  spi;
	spi_ioc_transfer  tr;
	uint8_t Buff[2];

};

