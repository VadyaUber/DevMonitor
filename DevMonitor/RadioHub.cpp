#include "RadioHub.h"

HUB::HUB(DevServer* serv)
{
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
	}
	pinMode(RF_M0, OUTPUT);
	pinMode(RF_M1, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(LED5, OUTPUT);
	digitalWrite(RF_M0, LOW);
	digitalWrite(RF_M1, LOW);
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);
	digitalWrite(LED4, LOW);
	digitalWrite(LED5, LOW);

	SR = new SerialPort("/dev/ttyS1", 115200);
	Server = serv;
	DevicesMac = { "1281DD05BFAD","12814ADC83A0","1281F162A6F5" };
	//DevicesMac[0] = "1281DD05BFAD";
	//DevicesMac[1] = "12814ADC83A0";
	//DevicesMac[2] = "1281F162A6F5";
	//this.TXdata[0] = "";
	//TXdata[1] = " ";
	//TXdata[2] = " ";
	//TXdata[0] = " ";
	StandartReqest[0] = ":" + DevicesMac[0] + ";;00;00000000000000;00\r\n";
	StandartReqest[1] = ":" + DevicesMac[1] + ";;00;00000000000000;00\r\n";
	StandartReqest[2] = ":" + DevicesMac[2] + ";;00;00000000000000;00\r\n";

	USB.Init(Server->WeildConfig.mac);

	//init led
	led = new LedGpio(5, 5, LED2, LED2); //RFIDBLOCK Check timings
	led->Inerfece = &Server->WeildConfig.interface;
	led->Status = &Server->Status;
	led->UsbBlink = &USB.USBblink;
	led->Led_RFID = NULL;
	new thread([&]() {
		while (true) {
			led->LedLoop();
			usleep(50000);
		}
		});
	wdconnection = 3;
}
#pragma push
//#pragma O3

void HUB::HUBloop()
{
	/*string writ2 = ":1281DD05BFAD;;00;00000000000000;00\r\n";
	string writ3 = ":12814ADC83A0;;00;00000000000000;00\r\n"; 
	string writ4 = ":1281F162A6F5;;00;00000000000000;00\r\n";*/

	int retrn;
	size_t received = 0;
	//string packet = *send;
	if (Server->ExternalDataSocket.find(DevicesMac[0]) != string::npos)
	{
		uint8_t stpk = Server->ExternalDataSocket.find(DevicesMac[0]) - 1;
		string packet = Server->ExternalDataSocket.substr(stpk, 35);
		Server->ExternalDataSocket.erase(stpk, packet.length());
		//string packet = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(DevicesMac[0]) - 1, (Server->ExternalDataSocket.find(DevicesMac[0]) + 1, Server->ExternalDataSocket.find("\r\n", Server->ExternalDataSocket.find("\r\n") + 1) - 2));
		//Server->ExternalDataSocket.erase(Server->ExternalDataSocket.find(DevicesMac[0]) - 1, packet.length());
		//string mc = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(":")+1, Server->ExternalDataSocket.find(":")+10);
		//string packet = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(":"), (Server->ExternalDataSocket.find(":") + 1, Server->ExternalDataSocket.find("\r\n", Server->ExternalDataSocket.find("\r\n") + 1)-2));
		SendPacket(packet);
		Server->NewDataInput = false;
	}
	else
	{
		printf("send str %s \n", StandartReqest[0].c_str());
		SendPacket(StandartReqest[0]);
	}
	tcflush(SR->serial_port, TCIOFLUSH);
	usleep(500000);
	Server->WeildLoop();
	
	received = Recive();
	if (received)
	{
		StatusConnection[0] = 0;
		printf("recv 12 ok: %s", rxbuff);
		Server->ExternalSendSoket = rxbuff;
		Server->WeildLoop(3);
		//datainput = true;

	}
	else
	{
		if (StatusConnection[0] <= wdconnection)StatusConnection[0]++;
		printf("recv bad 12: %s", rxbuff);
		Server->ExternalSendSoket = "";
	}
	
	
	if (Server->ExternalDataSocket.find(DevicesMac[1]) != string::npos)
	{
		uint8_t stpk = Server->ExternalDataSocket.find(DevicesMac[1]) - 1;
		string packet = Server->ExternalDataSocket.substr(stpk, 35);
		Server->ExternalDataSocket.erase(stpk, packet.length());
		//string packet = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(DevicesMac[1])-1, (Server->ExternalDataSocket.find(DevicesMac[1]) + 1, Server->ExternalDataSocket.find("\r\n", Server->ExternalDataSocket.find("\r\n") + 1) - 2));
		//Server->ExternalDataSocket.erase(Server->ExternalDataSocket.find(DevicesMac[1]) - 1, packet.length());
		SendPacket(packet);
		Server->NewDataInput = false;
	}
	else
	{
		printf("send str %s \n", StandartReqest[1].c_str());
		SendPacket(StandartReqest[1]);
	}
	tcflush(SR->serial_port, TCIOFLUSH);
	usleep(500000);
	Server->WeildLoop();

	received = Recive();
	if (received)
	{
		StatusConnection[1] = 0;
		printf("recv 12 ok: %s", rxbuff);
		Server->ExternalSendSoket = rxbuff;
		Server->WeildLoop(3);
		//datainput = true;

	}
	else
	{
		if (StatusConnection[1] <= wdconnection)StatusConnection[1]++;
		printf("recv bad 12: %s", rxbuff);
		Server->ExternalSendSoket = "";
	}
	//printf("\n");
	
	if (Server->ExternalDataSocket.find(DevicesMac[2]) != string::npos)
	{
		//string packet = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(DevicesMac[2]) - 1, (Server->ExternalDataSocket.find(DevicesMac[2]) + 1, Server->ExternalDataSocket.find("\r\n", Server->ExternalDataSocket.find("\r\n") + 1) - 2));
		uint8_t stpk = Server->ExternalDataSocket.find(DevicesMac[2]) - 1;
		//uint8_t enpk = Server->ExternalDataSocket.find(stpk, Server->ExternalDataSocket.find("\r\n"  - 2));
		string packet = Server->ExternalDataSocket.substr(stpk, 35);
		Server->ExternalDataSocket.erase(stpk, packet.length());
		//string mc = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(":") + 1, Server->ExternalDataSocket.find(":") + 10);
		//string packet = Server->ExternalDataSocket.substr(Server->ExternalDataSocket.find(":"), (Server->ExternalDataSocket.find(":") + 1, Server->ExternalDataSocket.find("\r\n", Server->ExternalDataSocket.find("\r\n") + 1) - 2));
		SendPacket(packet);
		Server->NewDataInput = false;
	}
	else
	{
		printf("send str %s \n", StandartReqest[2].c_str());
		SendPacket(StandartReqest[2]);
	}
	tcflush(SR->serial_port, TCIOFLUSH);
	usleep(500000);
	Server->WeildLoop();

	received = Recive();
	if (received)
	{
		StatusConnection[2] = 0;
		printf("recv 2 ok: %s", rxbuff);
		Server->ExternalSendSoket = rxbuff;
		Server->WeildLoop(3);
		//datainput = true;

	}
	else
	{
		if (StatusConnection[2] <= wdconnection)StatusConnection[2]++;
		printf("recv bad 2: %s", rxbuff);
		Server->ExternalSendSoket = "";
	}
	Server->WeildLoop(2);
	USB.USBconnect();

	if (StatusConnection[0] >= wdconnection) digitalWrite(LED1, 0);
	else digitalWrite(LED1, 1);
	if (StatusConnection[1] >= wdconnection) digitalWrite(LED4, 0);
	else digitalWrite(LED4, 1);
	if (StatusConnection[2] >= wdconnection) digitalWrite(LED5, 0);
	else digitalWrite(LED5, 1);

	Server->ExternalDataSocket.erase(0, Server->ExternalDataSocket.find(":")-2);
}

void HUB::SendPacket(string data) //проверить чтение на убме
{
	int retrn;
	retrn = write(SR->serial_port, data.c_str(), data.length());
	usleep(10000);
	//tcflush(SR->serial_port, TCIOFLUSH);
	printf("retrn send = %d\n", retrn);
}

size_t HUB::Recive()
{
	size_t recv = 0;
	while (recv < 94)
	{
		ssize_t r = read(SR->serial_port, rxbuff + recv, 100 - recv);
		if (r < 0)
		{
			perror("failed to read from port");
			//return -1;
		}
		if (r == 0)
		{
			// Timeout
			break;
		}
		recv += r;
	}
	return recv;
}
#pragma pop