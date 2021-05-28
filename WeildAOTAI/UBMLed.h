#pragma once
#include "iostream"
#include "ShiftRegister.h"
#include "WeildServer.h"


using namespace std;
class UBMLed:private ShiftRegister
{

public:

	UBMLed(uint8_t Ds_pin, uint8_t SHcp_pin, uint8_t STcp_pin, bool WG36,uint8_t Led1Pin, uint8_t Led2Pin, uint8_t Led3Pin, uint8_t Led4Pin,uint8_t WG24_36Pin,uint8_t BeperPin);
	bool WG36 = false;
	string * Inerfece;
	WeildStatus * Status;
	void Loop();
private:
	uint8_t ValueReg;
	uint8_t Led1;
	uint8_t Led2;
	uint8_t Led3;
	uint8_t Led4;
	uint8_t WG24_36;
	uint8_t BeperPin;
	MyTime Time;
	void blink(uint8_t pin,uint8_t count);
	void SetPin(uint8_t pin);
	void ResetPin(uint8_t pin);
	
};

