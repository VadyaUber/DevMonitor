#include "MyTime.h"

MyTime::MyTime()
{
	TimeStart = GetMilis();
	LastTime = GetMilis();
}

uint32_t MyTime::GetMilis()
{
	struct  timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	now = (uint64_t)ts.tv_sec * (uint64_t)1000 +(uint64_t)(ts.tv_nsec / 1000000L);
	return now - TimeStart;
}
bool MyTime :: CheckTimeEvent()
{
	if (GetMilis() < LastTime) {
		if (((int)(GetMilis() - LastTime)) + UINT32_MAX > IntevralSleep) {
			LastTime = GetMilis();
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if (GetMilis() - LastTime > IntevralSleep) {
			LastTime = GetMilis();
			return true; }
		else { return false; }
	}
	return false;
}