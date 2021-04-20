#pragma once
#include <iostream>
#include "SPI.h"
#include<cmath>
#include "pugixml.hpp"
#define FILTER_SIZE 10000
using namespace std;
typedef struct {
	double Array[FILTER_SIZE];
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
	SPI  spi_dev;
	uint8_t CS_PIN;
	double REF= 3.3 / 0XFFFF;
	bool filter_on = true;
	Filter FilterADC;
	bool MeasureEnable = true;
	bool PriznReadAdc = false;
	double SummArray = 0;
	void GetConfig(string FileConfig, string NameConfig);
	double FiltringADC(double * aray, int size);
	MatParam mat;
};

