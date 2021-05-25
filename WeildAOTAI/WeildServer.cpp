#include "WeildServer.h"
#include <errno.h>
WeildServer::WeildServer(string path_config, string path_log) {
	ReadFileConfig(path_config);
	if (WeildConfig.LOG_ON) {
		Log.Init(path_log);
	}
	TimeEvent[0].Timer.IntevralSleep = TIMOUT_SEND;
	TimeEvent[0].func = &WeildServer::SendServer;
	TimeEvent[1].Timer.IntevralSleep = TIMOUT_READ;
	TimeEvent[1].func = &WeildServer::RecvServer;
	TimeEvent[2].Timer.IntevralSleep = TIMOUT_CONN;
	TimeEvent[2].func = &WeildServer::ConectServer;
	TimeEvent[3].Timer.IntevralSleep = TIMOUT_DATA;
	TimeEvent[3].func = &WeildServer::FormatString;
	sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);

}

WeildServer::~WeildServer()
{
}

void WeildServer::WeildLoop() {
	//if (Status.StatusIterfece == NOT_CONNECTED) {
	//	ConnectInterfece();
	//}
	//else {
		//CheckComnd(":120101BDE9E4;01CD34567812AF;03;YYYYMMDDHHMMSS;11\r", 50);
		//CheckConnectInterface();
		for (int i = 0; i < MAX_EVET; i++) {
			if (TimeEvent[i].Timer.CheckTimeEvent()) {
				(*this.*TimeEvent[i].func)();
			}
		}
	//}
}

void WeildServer::ConnectInterfeceWIFI(string ssid, string pass)
{
	if (wifi.create_conect(ssid, pass)) {
		Status.StatusIterfece = CONNECTED;
		lastconected = true;
		WeildConfig.ip_out = wifi.GetMyIpInterfece(WeildConfig.interface);
		WeildConfig.router_ip = wifi.GetIpInterf(WeildConfig.interface);
	}
}

void WeildServer::ConnectInterfeceLAN()
{
	//if (wifi.get_connect(sockfd, WeildConfig.interface)) {
	WeildConfig.ip_out = wifi.GetMyIpInterfece("eth0");
	WeildConfig.router_ip = wifi.GetIpInterf("eth0");

	if (WeildConfig.ip_out.length() != 0 && WeildConfig.router_ip.length() != 0)
	{
		Status.StatusIterfece = CONNECTED;
		WeildConfig.interface = "eth0";
	}
	//}
}

void WeildServer::ConnectInterfece(string ssid, string pass) {
		
		if (WeildConfig.interface == "wlan0") {
			ConnectInterfeceWIFI(ssid, pass);
		}
		else if (WeildConfig.interface == "eth0") 
		{
			ConnectInterfeceLAN();
		}
}
void WeildServer::ReadFileConfig(string path)
{
	fstream FileMac;
	string line;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return;
	string tmp1, tmp2;
	string ip_tmp;
	int n1;
	int n2;
	WeildConfig.port = doc.child("config").child("server_port").attribute("value").as_uint();
	ip_tmp = doc.child("config").child("server_host").attribute("value").as_string();
	WeildConfig.server_ip = ip_tmp;
	string conn_type = doc.child("config").child("connection_type").attribute("value").as_string();
	if (conn_type == "wifi")
	{
		WeildConfig.interface = "wlan0";
		WeildConfig.wifi_sid = doc.child("config").child("wifi_ssid").attribute("value").as_string();
		WeildConfig.wifi_pass = doc.child("config").child("wifi_pass").attribute("value").as_string();
	}
	else {
		WeildConfig.interface = "eth0";
	}
	WeildConfig.LOG_ON = doc.child("config").child("LOG_ON").attribute("value").as_bool();
	WeildConfig.RFID_ON = doc.child("config").child("RFID_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_I_ON = doc.child("config").child("SENSOR_I_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_U_ON = doc.child("config").child("SENSOR_U_ON").attribute("value").as_bool();
	WeildConfig.SENSOR_W_ON = doc.child("config").child("SENSOR_W_ON").attribute("value").as_bool();
	WeildConfig.BlockMode = doc.child("config").child("BLOCK_MODE").attribute("value").as_string();
	WeildConfig.Compare_I= doc.child("config").child("COMPARE_I").attribute("value").as_int();
	WeildConfig.WG35 = doc.child("config").child("WG35").attribute("value").as_bool(); 
	WeildConfig.QR_ON= doc.child("config").child("QR_ON").attribute("value").as_bool();
	WeildConfig.ver = doc.child("config").child("version").attribute("value").as_string();
	WeildConfig.Type_Dev=doc.child("config").child("type_dev").attribute("value").as_string();
	FileMac.open("/sys/class/net/" + WeildConfig.interface + "/address", ios::in);
	getline(FileMac, line);
	transform(line.begin(), line.end(), line.begin(), ::toupper);
	for (int i = 0; i < 5; i++)
	{
		line.erase(line.find(':'), 1);
	}
	WeildConfig.mac = line;
}

void WeildServer::CheckConnectInterface()
{
		if (!wifi.get_connect(sockfd, WeildConfig.interface)) {
			Status.StatusIterfece = NOT_CONNECTED;
			if (Status.StatusSocet == CONNECTED) {
				Status.StatusSocet = NOT_CONNECTED;
				shutdown(sockfd, SHUT_RDWR);
				sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
			}
		}
}
int WeildServer::init_soket(string ip, int port) {
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1000;//Время на прием данных
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&val, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&val, sizeof(int));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	servaddr.sin_port = htons(port);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	return sockfd;
}


void WeildServer::ConectServer()
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
			ConnectInterfece(wifi_sid_reserved, wifi_pass_reserved);
		}
		else if (count_try_connection > 20)
		{
			count_try_connection = 0;
		}
		else 
		{
			ConnectInterfece(WeildConfig.wifi_sid, WeildConfig.wifi_pass);
		}
		count_try_connection++;
	}
	else if (Status.StatusSocet == NOT_CONNECTED) {
		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) >= 0) {	
			Log.WeildLogClose();
			Status.StatusSocet = CONNECTED;
		}
		/*else {
			printf("%d error \n\r", errno);
		}*/
	}
}
void WeildServer::RecvServer()
{
	if (Status.StatusSocet == CONNECTED) {
		int rads = 0;
		rads = recv(sockfd, dataInput, 99, MSG_NOSIGNAL);
		if (rads < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				Status.StatusSocet = NOT_CONNECTED;
				shutdown(sockfd, SHUT_RDWR);
				close(sockfd);
				sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
			}
		}
		else if (rads == 0) {
			Status.StatusSocet = NOT_CONNECTED;
			shutdown(sockfd, SHUT_RDWR);
			close(sockfd);
			sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
		}
		else {
			if (CheckComnd(dataInput, rads)) {
				NewDataInput = true;
			}
		}
	}
	else {
		Log.WeildLogWrite(SendSoket);
	}
}

void WeildServer::SendServer()
{
	if (Status.StatusSocet == CONNECTED) {
		if (send(sockfd, SendSoket.c_str(), SendSoket.size(), MSG_NOSIGNAL) < 0) {
			Status.StatusSocet = NOT_CONNECTED;
			Log.WeildLogWrite(SendSoket);
			shutdown(sockfd, SHUT_RDWR);
			sockfd = init_soket(WeildConfig.server_ip, WeildConfig.port);
		}
	}
	else {
		Log.WeildLogWrite(SendSoket);
	}
}

string WeildServer::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);
	return buf;
}
void WeildServer::FormatString()
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
	SendSoket.append(wifi.get_bd(WeildConfig.interface));
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
	//printf(SendSoket.c_str());
	string tmp = rfid + "\n\r";
    //printf(tmp.c_str());
	
}
unsigned char WeildServer::Crc8(const char *pcBlock, unsigned char len)
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
string  WeildServer::uint8_to_hex_string(uint8_t *v, const size_t s) {
	stringstream ss;

	ss << hex << setfill('0');

	for (int i = s - 1; i >= 0; i--) {
		ss << hex << setw(2) << static_cast<int>(v[i]);
	}
	string data = ss.str();
	transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}
bool  WeildServer::CheckComnd(char * buff, int len ) {
	try {
		struct tm TimeServer;
		uint8_t tmp;
		time_t unix_time;
		string s = convertToString(buff, len);
		//return false;
		if (s.find("\r") != -1 && s.find(":") != -1) {
			s.replace(s.find("\r"), 1, "");
			s.replace(s.find("\n"), 1, "");
			s.replace(s.find("\r"), 1, "");
			s.replace(s.find("\n"), 1, "");
			s.replace(s.find(":"), 1, "");
			ArrayVector = split(s, ';');

			


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
						PowerOn = ((tmp & 1) == 0);
						//printf("PowerON %d \n\r", PowerOn);
						DataOut = ";" + ArrayVector[1] + "\r\n";
						strptime(ArrayVector[3].c_str(), "%Y%m%d%H%M%S", &TimeServer);
						unsigned char buff[32] = { 0 };
						sprintf((char*)buff, (const char *)"date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", TimeServer.tm_mon+1, TimeServer.tm_mday, TimeServer.tm_year+1900, TimeServer.tm_hour, TimeServer.tm_min, TimeServer.tm_sec);
						//strcat(buff, " > /dev/null");
						system((const char *)buff);
					
						/*struct timeval  stime;
						stime.tv_sec = mktime(&TimeServer);
						settimeofday(&stime, NULL);*/
						if (StatusServerRecv == NOT_DATA)StatusServerRecv = NEW_DATA;
						return true;
					}
				}
			}
			
		
	}
	catch (int a) {

	}
	
	printf("fault comand\n");
	return false;
}
vector<string> WeildServer::split(string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> splittedStrings;
	while (std::getline(ss, item, delimeter))
	{
		splittedStrings.push_back(item);
	}
	return splittedStrings;
}
string WeildServer::convertToString(char* a, int size)
{
	int i;
	string s = "";
	for (i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}
