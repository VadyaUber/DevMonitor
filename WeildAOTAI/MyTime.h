#pragma once
#include <chrono>
#include <iostream>
#include <sys/time.h>
class MyTime
{
public:
	MyTime();

	uint32_t GetMilis();
	uint32_t IntevralSleep;
	bool CheckTimeEvent();
private:
	uint32_t TimeStart;
	uint32_t LastTime;
	uint64_t now;

};

