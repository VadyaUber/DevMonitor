#pragma once
#include "DevLog.h"

class DevLogUSB
{
public:
	void Init(string devicemac);
	bool USBconnect();
	bool USBblink;
	bool USBConfigRead;
private:
	bool USBmount(string device);
	bool USBumount();
	string PipCmd(string command);
	string mac;
	bool USBstate = false;
	uint8_t USBBlinkCount = 0;
};