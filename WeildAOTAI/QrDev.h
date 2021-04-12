#pragma once
#include "iostream"
#include "wiringPi.h"
#include "wiringSerial.h"

using namespace std;

class QrDev
{
public:
	QrDev(string port_name);
	void GetQrData(string * data);
private:
	int port;
	bool NewData;
	string data_qr;
	void Loop();
	char ch;
};

