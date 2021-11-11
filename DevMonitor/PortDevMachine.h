#pragma once
#include "iostream"
#include <thread>
#include "wiringPi.h"
#include "wiringSerial.h"
using namespace std;
class PortDevMachine
{
public:
	PortDevMachine(string  port_name);
	bool NewDataPort = false;
	void GetParseData(string * data);
	void WriteData(string data);
private:
	string DataInput;
	char ch;
	int port;
	void PortLoop();
	uint32_t cnt;
};

