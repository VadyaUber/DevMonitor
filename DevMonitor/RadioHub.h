#pragma once
#include <iostream>
#include "SerialDev.h"
#include "DevServer.h"
#include "LedGpio.h"
#include "thread"
#include "LogToUSB.h"

#define RF_M0 3
#define RF_M1 4
#define LED1 7 //UBM1
#define LED2 1 //connect server
#define LED3 5
#define LED4 12 //ubm2
#define LED5 13 //ubm3



using namespace std;
class HUB
{
public:
	HUB(DevServer* serv);
	void init(string* MAC_address, string* Packet);
	size_t Recive();
	void SendPacket(string data);
	void HUBloop();
	string* Inerfece;
	uint8_t* LedStatus;
	volatile bool datainput = false;
	string data = "";
	//vector<string> TXdata = {"","",""}; //���������� ������ � ����� ��� �������� �� ������
	//vector<string> RXdata = { "","","" }; //���������� ������ � ������� ��� �������� �����
	vector<string> DevicesMac = { "","","" }; //������ ��� �������� �����
private:
	LedGpio* led;
	DevLogUSB USB;
	void Checkpacket();
	string* MAC;
	string* send;
	SerialPort* SR;
	DevServer* Server;
	char rxbuff[115] = { 0 };
	vector<string> StandartReqest = { "","","" }; //����������� ����� ��� ������� �� ����� ����
	vector<uint8_t> StatusConnection = { 200,200,200 }; //��������� ����� � ������
	uint8_t wdconnection = 4;
};
