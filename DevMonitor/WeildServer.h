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
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "MyTime.h"
#include <functional>
#include <vector>
#include <string>
using namespace std;

#define SA struct sockaddr 
#define CONNECTED 1 
#define NOT_CONNECTED 2

#define TIMOUT_SEND 1000
#define TIMOUT_READ 500
#define TIMOUT_CONN 500
#define TIMOUT_DATA 900
#define MAX_EVET 4
#define SEND 0
#define READ 1
#define CONN 2
#define DATA 3

#define NOT_DATA   1 
#define NEW_DATA   2
#define IDEL_DATA  3
//enum Rfid_Status
//{
//	Wait,
//	SendServer,
//	EroorRead,
//	
//
//};
typedef struct{
    string ip_out;
    int port;
    string server_ip;
    string wifi_sid;
    string wifi_pass;
	string reserve_wifi_sid;
	string reserve_wifi_pass;
    string interface;
    string mac;
    string ver;
	bool RFID_ON = false;
	bool LOG_ON= false;
	uint16_t LOG_SIZE = 10;
	bool SENSOR_I_ON=false;
	bool SENSOR_U_ON = false;
	bool SENSOR_W_ON = false;
	bool RTC_ON = false;
	bool WG35 = false;
	bool QR_ON = false;
	bool USB_OUT = false;
	bool MercuryMeter = false;
	string Type_Dev = "";
	string router_ip;
	string BlockMode = "";
	int Compare_I = 0;

}config; 
typedef struct {
	int StatusIterfece = NOT_CONNECTED;
	int StatusSocet = NOT_CONNECTED;
	int StatusServer = NOT_CONNECTED;
	//int StatusRfid1 = Wait;
	bool StatusBloc = false;
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
		string rfid = "";
		string QrCode = "";
		string UartPackage = "";
		string Perefir = "";
		bool NewDataInput=false;

		uint8_t LedByte = 0;
		bool PowerOn = true; // 0 бит Ц команда сервера по управлению блокировкой
		bool RFID_status = false; //1 бит Ц Ђ1ї - сервер прин€л код от RFID. Ђ0ї сервер не прин€л код от RFID.
		bool QR_status = false; //2 бит Ц Ђ1ї - сервер прин€л код от сканера.Ђ0ї сервер не прин€л код от сканера.
		string uint8_to_hex_string(uint8_t *v, const size_t s);
		uint8_t StatusServerRecv = NOT_DATA;
	private:
		string buff_str = "";
		string ORANGE_PROGRAM = "01";
		string SendSoket;
		string mutable_data; //UartPackage+Perefir+rfid+QrCode

		char  dataInput[100];
		struct sockaddr_in servaddr;
		int sockfd;
		int wdserverrecv = 0;

		WeildLog  Log;
		void ConnectInterfeceWIFI( string ssid, string pass);
		void ConnectInterfeceLAN();
		void ConnectInterfece(string ssid, string pass);
		void ReadFileConfig(string path);
		void CheckConnectInterface();
		Wifi  wifi;
		MyTime* ServTime;
		string currentDateTime();
		
		unsigned char Crc8(const char *pcBlock, unsigned char len);
		int init_soket(string ip, int port);
		void  ConectServer();
		void  SendServer();
		void  RecvServer();
		void  FormatString();
		
		bool lastconected = false;
		bool LANcheckconect = false;
		int count_try_connection = 0;
		EventTime TimeEvent[MAX_EVET];
		string convertToString(char* a, int size);
		bool CheckComnd(char * buff, int len);
		vector<string> ArrayVector;
		vector<string>split(string strToSplit, char delimeter);
		
};

