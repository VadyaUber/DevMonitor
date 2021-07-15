#pragma once
#include"WeildServer.h"
#include "SPI_C.h"
#include"Rtc.h"
#include "MyTime.h"
#include "PortDevMachine.h"
#include "MyTime.h"
#include <string>
#include"weilgand.h"
#include <thread>
#include "LedGpio.h"
#include"QrDev.h"
#include <stdio.h>

typedef struct 
{
	string	Q100 = "";
	string	Q101 = "";
	string	Q102 = "";
	string  Q104 = "";
	string  Q200 = "";
	string  Q201 = "";
	string  Q300 = "";
	string	Q301 = "";
	string	Q303 = "";
	string  Q304 = "";
	string  Q500 = "";

}DataComandHaas;
class HaasMach
{
public:
	HaasMach(WeildServer *server);
	void HaasLoop();
private:
	string FormatString="";
	DataComandHaas HaasData;
	WeildServer * Server;
	PortDevMachine * HaasPort;
	int NumberComand1 = 0, NumberComand2 = 0;
	bool FastWrite = false;
	void HaasWriteComand();
	MyTime * Tim;
	MyTime * RtcTime;
	Rtc * rtc;
	string ComandArray[10] = { "Q100" ,"Q101" ,"Q102" ,"Q200" ,"Q300" ,"Q301" ,"Q303" ,"Q304" ,"Q201" ,"Q104" };
	bool ParsePascage();
	void FormatServer();
	string TmpData = "";
	string  removeAll(string str, const string& from);
	string replaceAll(string str, const string& from, const string& to);
	LedGpio * led;
};

