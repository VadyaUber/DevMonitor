#pragma once
#include <iostream>
#include "SerialDev.h"

using namespace std;
class Radio
{
public:
	Radio();
	void init(string* MAC_address, string* Packet);
	void WaitRecive();
	void SendPacket();
	string* Inerfece;
	uint8_t* LedStatus;
	volatile bool datainput = false;
	string data = "";
private:
	void Checkpacket();
	string* MAC;
	string* send;

};