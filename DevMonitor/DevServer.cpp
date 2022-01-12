#include "DevServer.h"
#include <errno.h>
DevServer::DevServer(string path_config, string path_log) {
	ReadFileConfig(path_config);
	if (WeildConfig.LOG_ON) {
		Log.Init(path_log,WeildConfig.LOG_SIZE);
	}
	TimeEvent[0].Timer.IntevralSleep = TIMOUT_SEND;
	TimeEvent[0].func = &DevServer::SendServer;
	TimeEvent[1].Timer.IntevralSleep = TIMOUT_READ;
	TimeEvent[1].func = &DevServer::RecvServer;
	TimeEvent[2].Timer.IntevralSleep = TIMOUT_CONN;
	TimeEvent[2].func = &DevServer::ConectServer;
	TimeEvent[3].Timer.IntevralSleep = TIMOUT_DATA;
	TimeEvent[3].func = &DevServer::FormatString;
	sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
	ServTime = new MyTime();
	ServTime->IntevralSleep = 60000;
}

DevServer::~DevServer()
{
}

void DevServer::WeildLoop() {
	//if (Status.StatusIterfece == NOT_CONNECTED) {
	//	ConnectInterfece();
	//}
	//else {
		//CheckComnd(":120101BDE9E4;01CD34567812AF;03;YYYYMMDDHHMMSS;11\r", 50);
		for (int i = 0; i < MAX_EVET; i++) {
			if (TimeEvent[i].Timer.CheckTimeEvent()) {
				(*this.*TimeEvent[i].func)();
			}
		}
	//}
}

void DevServer::ConnectInterfeceWIFI(string ssid, string pass)
{
	try {
		if (wifi.create_conect(ssid, pass)) {
			Status.StatusIterfece = CONNECTED;
			printf("Wifi interface connect successful \n");
			lastconected = true;
			WeildConfig.ip_out = wifi.GetMyIpInterfece(WeildConfig.interface);
			WeildConfig.router_ip = wifi.GetIpInterf(WeildConfig.interface);
		}
	}
	catch (exception& e)
	{
		printf("error connect Wifi");
		cerr << "Caught " << e.what() << endl;
		cerr << "Type " << typeid(e).name() << endl;
	}
}

void DevServer::ConnectInterfeceLAN()
{
	try {
		//if (wifi.get_connect(sockfd, WeildConfig.interface)) {
		WeildConfig.ip_out = wifi.GetMyIpInterfece("eth0");
		WeildConfig.router_ip = wifi.GetIpInterf("eth0");

		if (WeildConfig.ip_out.length() != 0 && WeildConfig.router_ip.length() != 0)
		{
			Status.StatusIterfece = CONNECTED;
			printf("Lan interface connect successful \n");
			lastconected = true;
			WeildConfig.interface = "eth0";
		}
	}
	catch (exception& e)
	{
		printf("error connect Wifi");
		cerr << "Caught " << e.what() << endl;
		cerr << "Type " << typeid(e).name() << endl;
	}
	//}
}

void DevServer::ConnectInterfece(string ssid, string pass) {
		
		if (WeildConfig.interface == "wlan0") {
			ConnectInterfeceWIFI(ssid, pass);
		}
		else if (WeildConfig.interface == "eth0") 
		{
			ConnectInterfeceLAN();
		}
}
void DevServer::ReadFileConfig(string path)
{
	fstream FileMac;
	string line;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
	{
		printf("ReadFileConfig error \n");
		return;
	}
	WeildConfig.port = doc.child("config").child("server_port").attribute("value").as_uint();
	WeildConfig.server_dns = doc.child("config").child("server_host_dns").attribute("value").as_string();
	WeildConfig.server_ip = doc.child("config").child("server_host_ip").attribute("value").as_string();
	string conn_type = doc.child("config").child("connection_type").attribute("value").as_string();
	if (conn_type == "wifi")
	{
		WeildConfig.interface = "wlan0";
		WeildConfig.wifi_sid = doc.child("config").child("wifi_ssid").attribute("value").as_string();
		WeildConfig.wifi_pass = doc.child("config").child("wifi_pass").attribute("value").as_string();
		WeildConfig.reserve_wifi_sid = doc.child("config").child("reserve_wifi_ssid").attribute("value").as_string();
		WeildConfig.reserve_wifi_pass = doc.child("config").child("reserve_wifi_pass").attribute("value").as_string();
	}
	else {
		WeildConfig.interface = "eth0";
	}
	WeildConfig.LOG_ON = doc.child("config").child("LOG_ON").attribute("value").as_bool();
	WeildConfig.LOG_SIZE = doc.child("config").child("LOG_SIZE").attribute("value").as_uint();
	WeildConfig.RFID_ON = doc.child("config").child("RFID_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_I_ON = doc.child("config").child("SENSOR_I_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_U_ON = doc.child("config").child("SENSOR_U_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_W_ON = doc.child("config").child("SENSOR_W_ON").attribute("value").as_bool();
	WeildConfig.RTC_ON = doc.child("config").child("RTC_ON").attribute("value").as_bool();
	WeildConfig.BlockMode = doc.child("config").child("BLOCK_MODE").attribute("value").as_string();
	WeildConfig.Compare_I= doc.child("config").child("COMPARE_I").attribute("value").as_int();
	WeildConfig.WG35 = doc.child("config").child("WG35").attribute("value").as_bool(); 
	WeildConfig.QR_ON= doc.child("config").child("QR_ON").attribute("value").as_bool();
	WeildConfig.USB_OUT = doc.child("config").child("USB_OUT").attribute("value").as_bool();
	WeildConfig.ver = doc.child("config").child("version").attribute("value").as_string();
	WeildConfig.Type_Dev=doc.child("config").child("type_dev").attribute("value").as_string();
	WeildConfig.MercuryMeter = doc.child("config").child("MERCURY").attribute("value").as_bool();
	FileMac.open("/sys/class/net/" + WeildConfig.interface + "/address", ios::in);
	getline(FileMac, line);
	transform(line.begin(), line.end(), line.begin(), ::toupper);
	for (int i = 0; i < 5; i++)
	{
		line.erase(line.find(':'), 1);
	}
	WeildConfig.mac = line;
}

void DevServer::CheckConnectInterface()
{
	try
	{
		if (Status.StatusSocet == NOT_CONNECTED)
		{
			FILE* output;
			string out_commad = "nmcli connection | grep -c " + WeildConfig.interface;
			//string out_commad = "iwgetid -r";
			if (!(output = popen(out_commad.c_str(), "r")))
			{
				printf("error check interface connection \n");
			}
			//char buffer[20];
			//string buf_out = "";
			//while (fgets(buffer, 10, output) != nullptr)
			//{
			//	buf_out += buffer;
			//}
			//if(buf_out.substr(0, buf_out.length() - 1) == WeildConfig.wifi_pass)
			unsigned int i;
			fscanf(output, "%u", &i);
			if (i <= 0)
			{
				Status.StatusIterfece = NOT_CONNECTED;
			}
			pclose(output);
		}
	}
	catch (const std::exception&ex)
	{
		printf("Error:_CheckConnectInterface__%s\n", ex);
		Status.StatusIterfece = NOT_CONNECTED;
	}

		/*if (!wifi.get_connect(sockfd, WeildConfig.interface)) {
			Status.StatusIterfece = NOT_CONNECTED;
			if (Status.StatusSocet == CONNECTED) {
				Status.StatusSocet = NOT_CONNECTED;
				shutdown(sockfd, SHUT_RDWR);
				sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
			}
		}*/
}
int DevServer::init_soket(string ip, int port) {
	//struct cli_addr;
	//int clilen, rfd;
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) { perror("ERROR: socket()");}

	int val = 1000;//Время на прием данных
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&val, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&val, sizeof(int));
	val = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

	//int flags = 10;
	//if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPIDLE");};

	//flags = 5;
	//if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void*)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPCNT");};

	//flags = 5;
	//if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void*)&flags, sizeof(flags))) { perror("ERROR: setsocketopt(), SO_KEEPINTVL");};

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	servaddr.sin_port = htons(port);
	//if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) { perror("ERROR: bind()");}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	//listen(sockfd, 5);

	//clilen = sizeof(cli_addr);
	//rfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
	//if (rfd < 0) { perror("ERROR: accept()");}

	return sockfd;

}


void DevServer::ConectServer()
{
	if (Status.StatusIterfece == NOT_CONNECTED) {
		
		if (!lastconected && !LANcheckconect)
		{
			ConnectInterfeceLAN();
			if (count_try_connection > 6)
			{
				LANcheckconect = true;
				count_try_connection = 0;
			}
		}
		else if (count_try_connection > 10 && !lastconected)
		{
			ConnectInterfeceWIFI(WeildConfig.reserve_wifi_sid, WeildConfig.reserve_wifi_pass);
		}
		else 
		{
			ConnectInterfece(WeildConfig.wifi_sid, WeildConfig.wifi_pass);
		}
		count_try_connection++;
		if (count_try_connection > 20)
		{
			count_try_connection = 0;
		}
	}
	else if (Status.StatusSocet == NOT_CONNECTED) 
	{
		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) >= 0) {	
			Log.DevLogClose();
			Status.StatusSocet = CONNECTED;
			StatusServerRecv = NOT_DATA;
			printf("socket connect successful \n");
			
		}
		else {
			CheckConnectInterface();
			printf("socket connect failed \n");
			printf("%d error \n\r", errno);
			Status.StatusSocet = NOT_CONNECTED;
			usleep(150000);
			if (WeildConfig.server_dns != "")
			{
				struct addrinfo* result;
				int error = getaddrinfo(WeildConfig.server_dns.c_str(), NULL, NULL, &result);
				if (error >= 0)
				{
					string ips = to_string(result->ai_addr->sa_data[2]) + '.' + to_string(result->ai_addr->sa_data[3]) + '.' + to_string(result->ai_addr->sa_data[4]) + '.' + to_string(result->ai_addr->sa_data[5]);
					if (WeildConfig.server_ip != ips)
					{
						WeildConfig.server_ip = ips;
						shutdown(sockfd, SHUT_RDWR);
						close(sockfd);
						sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
						//string path = "/weildpath/config_ubm4.xml";
						//pugi::xml_document doc;
						//pugi::xml_parse_result result = doc.load_file(path.c_str());
						////WeildConfig.server_ip = doc.child("config").child("server_host_ip").attribute("value").as_string();
						//doc.child("config").child("server_host_ip").attribute("value").set_value(ips.c_str());
						//doc.save_file(path.c_str());
					}
				}
				else
				{
					if (error == EAI_SYSTEM)
					{
						perror("getaddrinfo");
					}
					else
					{
						fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
					}
				}
			}
		}
	}
	else if((Status.StatusIterfece == CONNECTED)&&(Status.StatusSocet == CONNECTED)&&(Status.StatusServer == NOT_CONNECTED))
	{

	}
}
void DevServer::RecvServer()
{
	if (Status.StatusSocet == CONNECTED)
	{
		int rads = 0;
		rads = recv(sockfd, dataInput, 99, MSG_NOSIGNAL);
		if (rads < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				Status.StatusSocet = NOT_CONNECTED;
				shutdown(sockfd, SHUT_RDWR);
				close(sockfd);
				sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
			}
			else if (wdserverrecv>20)
			{
				wdserverrecv = 0;
				Status.StatusSocet = NOT_CONNECTED;
				Status.StatusServer = NOT_CONNECTED;
				shutdown(sockfd, SHUT_RDWR);
				close(sockfd);
				sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
			}
			wdserverrecv++;
		}

		else if (rads == 0)
		{
			Status.StatusSocet = NOT_CONNECTED;
			shutdown(sockfd, SHUT_RDWR);
			close(sockfd);
			sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
		}

		else //пришли данные.. парсим,проверяем.
		{
			if (CheckComnd(dataInput, rads)) 
			{
				NewDataInput = true;
				wdserverrecv = 0;
			}
			if (buff_str.length() > 0)
			{
				CheckComnd(0, 0);
			}
		}
	}
	//else {
	//	Log.DevLogWrite(SendSoket);
	//}
}

void DevServer::SendServer()
{
	if (Status.StatusSocet == CONNECTED && Status.StatusServer == CONNECTED)
	{
		if (send(sockfd, SendSoket.c_str(), SendSoket.size(), MSG_NOSIGNAL) < 0) 
		{
			Status.StatusSocet = NOT_CONNECTED;
			if (WeildConfig.LOG_ON)Log.DevLogWrite(SendSoket, mutable_data);
			shutdown(sockfd, SHUT_RDWR);
			sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
		}
	}
	else if (Status.StatusSocet == CONNECTED && Status.StatusServer == NOT_CONNECTED)
	{
		if (WeildConfig.LOG_ON)Log.DevLogWrite(SendSoket, mutable_data);
		if (send(sockfd, SendSoket.c_str(), SendSoket.size(), MSG_NOSIGNAL) < 0)
		{
			Status.StatusSocet = NOT_CONNECTED;
			shutdown(sockfd, SHUT_RDWR);
			sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
		}
	}
	else 
	{
		if (WeildConfig.LOG_ON)Log.DevLogWrite(SendSoket, mutable_data);
	}
}

string DevServer::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);
	string bufstring(buf);
	return bufstring;
}
void DevServer::FormatString()
{
	SendSoket = ":";
	SendSoket.append(WeildConfig.mac);
	SendSoket.append(";");
	SendSoket.append(UartPackage);
	SendSoket.append(";");
	SendSoket.append(Perefir);
	SendSoket.append(";");
	SendSoket.append(rfid);
	SendSoket.append(";");
	SendSoket.append(QrCode);
	SendSoket.append(";");
	if (WeildConfig.interface == "wlan0")
		SendSoket.append(wifi.get_bd(WeildConfig.interface));
	else
		SendSoket.append("98");
	SendSoket.append(";");
	SendSoket.append(WeildConfig.router_ip);
	SendSoket.append(";");
	SendSoket.append(WeildConfig.ip_out);
	SendSoket.append(";");
	SendSoket.append(ORANGE_PROGRAM);
	SendSoket.append(";");
	SendSoket.append(currentDateTime());
	SendSoket.append(";");
	uint8_t crc = Crc8(SendSoket.substr(1, SendSoket.size() - 1).c_str(), SendSoket.size() - 1);
	SendSoket.append(uint8_to_hex_string(&crc, 1));
	SendSoket.append("\r\n");
	/*printf("\r\n");
	printf(SendSoket.c_str());
	printf("\r\n");*/
	//string tmp = rfid + "\n\r";
    //printf(tmp.c_str());
	mutable_data = UartPackage + Perefir + rfid + QrCode;
}
unsigned char DevServer::Crc8(const char *pcBlock, unsigned char len)
{
	unsigned char crc = 0x00;
	int b = 0x00;
	unsigned char CRC_Table[256] = {
	0, 94, 188, 226, 97, 63, 221, 131,
	194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30,
	95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160,
	225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61,
	124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197,
	132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103,  57, 186, 228, 6, 88,
	25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230,
	167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123,
	58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15,
	78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146,
	211, 141, 111,  49, 178, 236,  14,  80,
	175, 241,  19,  77, 206, 144, 114,  44,
	109,  51, 209, 143,  12,  82, 176, 238,
	50, 108, 142, 208,  83,  13, 239, 177,
	240, 174,  76,  18, 145, 207,  45, 115,
	202, 148, 118,  40, 171, 245,  23,  73,
	8,  86, 180, 234, 105,  55, 213, 139,
	87,   9, 235, 181,  54, 104, 138, 212,
	149, 203,  41, 119, 244, 170,  72,  22,
	233, 183,  85,  11, 136, 214,  52, 106,
	43, 117, 151, 201,  74,  20, 246, 168,
	116,  42, 200, 150,  21,  75, 169, 247,
	182, 232,  10,  84, 215, 137, 107,  53 };

	for (int i = 0; i < len; i++)
	{
		b = pcBlock[i] ^ crc;
		crc = CRC_Table[b];
	}
	return crc;
}
string  DevServer::uint8_to_hex_string(uint8_t *v, const size_t s) {
	stringstream ss;

	ss << hex << setfill('0');

	for (int i = s - 1; i >= 0; i--) {
		ss << hex << setw(2) << static_cast<int>(v[i]);
	}
	string data = ss.str();
	transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}
bool  DevServer::CheckComnd(char * buff, int len ) {
	try {
		struct tm TimeServer;
		uint8_t tmp=0;
		time_t unix_time;
		uint32_t lenMsg;
		struct pak
		{

		string mac = "";
		string pack = "";
		string comd = "";
		string dtim = "";
		string cr = "";

		};
		if(len!=0)
			buff_str += convertToString(buff, len);
		uint16_t last_delimiter_index = buff_str.find("\r\n", buff_str.find("\r\n") + 1) - 2; //конец пакета 
		if (last_delimiter_index >= 0) //еcли что-то пришло
		{
			string s = buff_str.substr(buff_str.find(":") + 1, buff_str.find("\r\n", buff_str.find("\r\n") + 1) - 3);
			if ((count(s.begin(), s.end(), ';')) < 4) //если меньше 4х разделителей
				return false;
			ArrayVector = split(s, ';');
			
			

			lenMsg = last_delimiter_index + 4;
			if (buff_str.length() > lenMsg) //Еcли разделитель не поcледний cимвол
			{
				buff_str = buff_str.substr(lenMsg, buff_str.length());
			}
			else
			{
				buff_str = "";
			}
		//}
		//string s = convertToString(buff, len);
		////return false;
		//if (s.find("\r") != -1 && s.find(":") != -1 && s.find_first_of(":") == 2) {
		//	s.replace(s.find("\r"), 1, "");
		//	s.replace(s.find("\n"), 1, "");
		//	s = s.substr(s.find(":")+1, s.find("\r")-1);
		//	ArrayVector = split(s, ';');
		//	

				if (WeildConfig.mac == ArrayVector[0]) {

					if (ArrayVector[2].size() > 0)
					{
						tmp = (uint8_t)stoi(ArrayVector[2], nullptr, 16);
					}

					//string crc_tmp = ArrayVector[0] + ";" + ArrayVector[1] + ";" + ArrayVector[2]+";" + ArrayVector[3]+";";
					string crc_tmp = s.substr(0, s.size() - 2);

					uint8_t crc_package = (uint8_t)stoi(ArrayVector[4], nullptr, 16);

					
					uint8_t crc_real = Crc8(crc_tmp.c_str(), crc_tmp.size());
					if (crc_real == crc_package) {
						//	LedByte = tmp;
						PowerOn = ((tmp >> 0) & 1) == 0;
						RFID_status = ((tmp >> 1) & 1) == 1;
						QR_status = ((tmp >> 2) & 1) == 1;
						//printf("PowerON %d \n\r", PowerOn);
						DataOut = ";" + ArrayVector[1] + "\r\n";
						if (ServTime->CheckTimeEvent()|| StatusServerRecv == NOT_DATA)
						{
							strptime(ArrayVector[3].c_str(), "%Y%m%d%H%M%S", &TimeServer);
							unsigned char buff[32] = { 0 };
							sprintf((char*)buff, (const char*)"date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", TimeServer.tm_mon + 1, TimeServer.tm_mday, TimeServer.tm_year + 1900, TimeServer.tm_hour, TimeServer.tm_min, TimeServer.tm_sec);
							//strcat(buff, " > /dev/null");
							system((const char*)buff);
						}
						/*struct timeval  stime;
						stime.tv_sec = mktime(&TimeServer);
						settimeofday(&stime, NULL);*/
						if (StatusServerRecv == NOT_DATA)
						{
							StatusServerRecv = NEW_DATA;
						}
						Status.StatusServer = CONNECTED; //данные с сервера пришли 
						return true;
					}
					else
					{
						printf("Error check crc\n");
					}
				}
			}
		else
		{
			printf("Can't find |r or : in buff\n");
		}
		
	}
	catch (int a) {
		printf("Error:___%d\n", a);
	}
	catch (const std::out_of_range& oror) {
		std::cerr << "Out of Range error: " << oror.what() << '\n';
	}
	
	printf("fault check server comand:___%s\n",buff);
	return false;
}
vector<string> DevServer::split(string strToSplit, char delimeter)
{
	vector<string> splittedStrings;
	string w = "";
	for (auto i : strToSplit) {
		if (i == ';') {
			splittedStrings.push_back(w);
			w = "";
		}
		else {
			w += i;
		}
	}
	splittedStrings.push_back(w);
	return splittedStrings;
}
string DevServer::convertToString(char* a, int size)
{
	int i;
	string s = "";
	for (i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}
