#pragma once

#include <iostream> 
#include <thread>
#include "wiringPi.h"
#include "wiringSerial.h"


using namespace std;

class ParserPort
{
public:

	ParserPort(string port);
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

