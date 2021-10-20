#pragma once
#include "WeildLog.h"

class WeildLogUSB
{
public:
	void Init(string devicemac);
	bool USBconnect();
	bool USBblink;
private:
	bool USBmount(string device);
	bool USBumount();
	string PipCmd(string command);
	string mac;
	bool USBstate = false;
};