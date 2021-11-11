#pragma once
#include <iostream>
#include "ShiftRegister.h"
using namespace std;

class DigitalOutUbmLast:private ShiftRegister
{

public:
	DigitalOutUbmLast(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin, uint8_t PowerOnPin, uint8_t BeepPin, uint8_t RST_Pin, uint8_t WG_36Pin);
	bool PowerOn = false;
	bool Beep = false;
	bool RST_CLK = false;
	bool WG_36 = false;
	void Loop();
private:
	uint16_t LastVal = 0;
	uint16_t value=0xff00;
	uint8_t PowerOnPin = 0;
	uint8_t BeepPin = 0;
	uint8_t RST_Pin = 0;
	uint8_t WG_36Pin = 0;

};

