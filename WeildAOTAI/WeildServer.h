#pragma once
#include <iostream>
#include "WeildLog.h"
#include "Wifi.h"
#include "pugixml.hpp"
#include <fstream>
#include <algorithm>
#include <iomanip> 
#include <sstream> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MyTime.h"
#include <functional>
using namespace std;

#define SA struct sockaddr 
#define CONNECTED 1 
#define NOT_CONNECTED 2
#define TIMOUT_SEND 1000
#define TIMOUT_READ 100
#define TIMOUT_CONN 500
#define TIMOUT_DATA 900
#define MAX_EVET 4
#define SEND 0
#define READ 1
#define CONN 2
#define DATA 3
typedef struct{
    string ip_out;
    int port;
    string server_ip;
    string wifi_sid;
    string wifi_pass;
    string interface;
    string mac;
    string ver;
	bool RFID_ON = false;
	bool LOG_ON= false;
	bool SENSOR_I_ON=false;
	bool SENSOR_U_ON = false;
	bool SENSOR_W_ON = false;
	string Type_Dev = "";
	string router_ip;
}config; 
typedef struct {
	int StatusIterfece=NOT_CONNECTED;
	int StatusSocet = NOT_CONNECTED;
}WeildStatus;

class WeildServer
{		
	typedef void (WeildServer::*MetodClass)();
	typedef struct
	{
		MyTime  Timer;
		MetodClass func;
	}EventTime;
	public:
		WeildServer(string path_config, string path_log);
		~WeildServer();
		WeildStatus Status;
		//string DataToServer="0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
		string DataOut;
		config WeildConfig;
		void WeildLoop();
		string rfid = "0000000000";
		string QrCode = "0000000000";
		string UartPackage = "000000";
		string Perefir = "000000";
		bool NewDataInput=false;
	private:
		string ORANGE_PROGRAM = "01";
		string SendSoket;
		
		char  dataInput[100];
		struct sockaddr_in servaddr;
		int sockfd;

		WeildLog  Log;
		void ConnectInterfece();
		void ReadFileConfig(string path);
		void CheckConnectInterface();
		Wifi  wifi;
		
		string currentDateTime();
		string uint8_to_hex_string(uint8_t *v, const size_t s);
		unsigned char Crc8(const char *pcBlock, unsigned char len);
		int init_soket(string ip, int port);
		void  ConectServer();
		void  SendServer();
		void  RecvServer();
		void  FormatString();
		
		EventTime TimeEvent[MAX_EVET];
		string convertToString(char* a, int size);
		bool CheckComnd(char * buff, int len);
};

