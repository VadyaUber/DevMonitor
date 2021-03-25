#pragma once
#include "wiringPi.h"
#include <iostream>
#include "MyTime.h"
#include "WeildServer.h"
using namespace std;
class LedGpio
{
public:
	LedGpio(uint8_t LedPin1, uint8_t LedPin2, uint8_t LedPin3, uint8_t LedPin4);
	void LedLoop();
	string * Inerfece;
	WeildStatus * Status;
	uint8_t * LedStatus;
private:
	MyTime Time;
	uint8_t Led1;
	uint8_t Led2;
	uint8_t Led3;
	uint8_t Led4;
	void blinc(uint8_t pin, uint8_t count);
};

