#pragma once
#include "wiringPi.h"
#include "iostream"
#include <chrono>
#include <thread>
using namespace std;
class ShiftRegister
{
public:
	ShiftRegister(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin);
	void Write(uint8_t * value,uint8_t size);
 private:
	uint8_t Ds;
	uint8_t SHcp;
	uint8_t STcp;
	void Clock();
	void Strobe();
};

