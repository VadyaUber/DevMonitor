#pragma once
#include"WeildServer.h"
#include"wiringPi.h"
#include"weilgand.h"
#include"ParserPort.h"
#include <thread>
#include "LedGpio.h"
class WeildParseDev
{
public:
	WeildParseDev(WeildServer * server);
	void WeildParseDevLoop();
private:
	WeildServer * ServerDev;
	ParserPort * port;
	LedGpio * led;
};

