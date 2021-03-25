#pragma once
#include "iostream"
#include "ShiftRegister.h"
#include "WeildServer.h"
#define LED1 4
#define LED2 3
#define LED3 0
#define LED4 1
#define BEEPER 2
#define WG24_36 5
using namespace std;
class UBMLed:private ShiftRegister
{

public:

	UBMLed(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin, bool WG36);
	bool WG36 = false;
	string * Inerfece;
	WeildStatus * Status;
	void Loop();
private:
	uint8_t ValueReg;
	MyTime Time;
	void blink(uint8_t pin,uint8_t count);
	void SetLed(uint8_t pin);
};

