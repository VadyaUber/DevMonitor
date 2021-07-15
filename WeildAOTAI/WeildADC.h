#pragma once
#include <iostream>
#include "SPI_C.h"
#include<cmath>
#include "pugixml.hpp"
#define FILTER_SIZE 10000
#define FREQ_CLK 2000000
using namespace std;
typedef struct {
	uint32_t Array[FILTER_SIZE];
	uint32_t Cnt = 0;
}Filter;
typedef struct {
	double offset;
	double divisor;
	double multiplier;
}MatParam;
class WeildADC
{
public:
	WeildADC(uint8_t cs_pin,bool filter, string FileConfig,string NameConfig);
	double Value;
	uint16_t Value16Bit;
	void ReadValue();
	void CalculateAdc();

private:
	
	uint8_t CS_PIN;
	double REF= 3.3 / 0XFFFF;
	bool filter_on = true;
	Filter FilterADC;
	bool MeasureEnable = true;
	bool PriznReadAdc = false;
	double SummArray = 0;
	void GetConfig(string FileConfig, string NameConfig);
	uint32_t FiltringADC(uint32_t* aray, int size);
	MatParam mat;
};

