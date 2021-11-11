
#include "HaasMach.h"
#define RTC_CS 1
#define WILDGANPIN0 15
#define WILDGANPIN1 16
#define WG35Pin  4 
#define LED1 5
#define LED2 7
#define LED3 10
#define LED4 11
#define CS_PIN 4
HaasMach::HaasMach(WeildServer * server)
{

	////file.open("/weildpath/programm/write/5141.NC");
	//FILE* fs;
	//char buffer[30];

	//fs = fopen("/weildpath/programm/write/5141.NC", "r");

	//if (NULL == fs)
	//{
	//	perror("opening fs");
	//	usleep(1);
	//}

	////while (EOF != fscanf(fs, "%[^\n]\n", buffer))
	//while (EOF != fscanf(fs, "%[^\n]\n", buffer))
	//{
	//	printf("%s\n", buffer);
	//}
	//fclose(fs);

	HaasData.Q100 = "S/N, 0000000";
	HaasData.Q101 = "SOFTWARE, VER 000.00";
	HaasData.Q102 = "MODEL, 0000";
	HaasData.Q104 = "MODE, 000";
	HaasData.Q200 = "TOOL CHANGES, 00";
	HaasData.Q201 = "USING TOOL, 0";
	HaasData.Q300 = "P.O. TIME, 00000:00:00";
	HaasData.Q301 = "C.S. TIME, 00000:00:00";
	HaasData.Q303 = "LAST CYCLE, 000:00:00";
	HaasData.Q304 = "PREV CYCLE, 000:00:00";
	HaasData.Q500 = "PROGRAM,O00000,000000000,PARTS,00";



	this->Server = server;
	this->HaasPort = new PortDevMachine("/dev/ttyUSB0");
	Tim = new MyTime();
	Tim->IntevralSleep = 1000;
	RtcTime = new MyTime();
	RtcTime->IntevralSleep = 3600000;
	rtc = new Rtc(RTC_CS);
	rtc->GetRtc();
	pinMode(WG35Pin, OUTPUT);
	if (Server->WeildConfig.RFID_ON) {
		new thread([&]() {
			//wiegand_loop(WILDGANPIN0, WILDGANPIN1, !Server->WeildConfig.WG35);
			});
	}
	led = new LedGpio(LED1, LED2, LED3, LED4);

	led->Inerfece = &Server->WeildConfig.interface;
	led->Status = &Server->Status;
	new thread([&]() {
		while (true) {
			led->LedLoop();
			if (RtcTime->CheckTimeEvent() || Server->StatusServerRecv == NEW_DATA) {
				Server->StatusServerRecv = IDEL_DATA;

				rtc->SetRtc();
			}
		}

		});

	if (Server->WeildConfig.WG35) {
		digitalWrite(WG35Pin, HIGH);
	}
	else {
		digitalWrite(WG35Pin, LOW);
	}
	FormatServer();
	Server->Perefir = FormatString;
}

void HaasMach::HaasLoop()
{
	this->Server->WeildLoop();
	if (Tim->CheckTimeEvent()) {
		this->HaasWriteComand();
	}
	if (HaasPort->NewDataPort) {
		HaasPort->GetParseData(&TmpData);
		if (ParsePascage()) {
			FormatServer();
			Server->Perefir = FormatString;
		}
	}

}

void HaasMach::HaasWriteComand()
{
	if (!this->FastWrite) {
		this->FastWrite = true;
		HaasPort->WriteData(ComandArray[NumberComand1]+"\r\n");
		NumberComand1++;
		if (NumberComand1 > 9) {
			NumberComand1 = 8;
		}
		NumberComand2++;
		if (NumberComand2 > 30) {
			NumberComand2 = 0;
			NumberComand1 = 0;
		}
	}
	else {
		this->FastWrite = false;
		HaasPort->WriteData("Q500\r\n");
	}

}

bool HaasMach::ParsePascage()
{
	if (TmpData != "") {
		TmpData= removeAll(TmpData, "\n");
		TmpData = removeAll(TmpData, " ");
		TmpData = removeAll(TmpData, "\r");
		TmpData = removeAll(TmpData, ">");
		TmpData = replaceAll(TmpData, ":","-");
		size_t pos1= TmpData.find((char)0x02);
		size_t pos2 = TmpData.find((char)0x17);
		if (pos1 != string::npos && pos2 != string::npos) {
			TmpData = TmpData.substr(pos1 + 1, pos2 - 1);
			//pos1 = TmpData.find("STATUS,BUSY");
			if (TmpData.find("STATUS,BUSY") != string::npos) {
				HaasData.Q500 = " ,BUSY, ";

			}
			//pos1 = ;
			if (TmpData.find("S/N,") != string::npos) {
				HaasData.Q100 = removeAll(TmpData, "S/N,");

			}
			if (TmpData.find("SOFTWARE,") != string::npos) {
				HaasData.Q101 = removeAll(TmpData, "SOFTWARE,");

			}
			if (TmpData.find("MODEL,") != string::npos) {
				HaasData.Q102 = removeAll(TmpData, "MODEL,");

			}
			if (TmpData.find("MODE,") != string::npos) {
				HaasData.Q104 = removeAll(TmpData, "MODE,");

			}
			if (TmpData.find("TOOLCHANGES,") != string::npos) {
				HaasData.Q200 = removeAll(TmpData, "TOOLCHANGES,");

			}
			if (TmpData.find("USINGTOOL,") != string::npos) {
				HaasData.Q201 = removeAll(TmpData, "USINGTOOL,");

			}
			if (TmpData.find("P.O.TIME,") != string::npos) {
				HaasData.Q300 = removeAll(TmpData, "P.O.TIME,");

			}
			if (TmpData.find("C.S.TIME,") != string::npos) {
				HaasData.Q301 = removeAll(TmpData, "C.S.TIME,");

			}
			if (TmpData.find("LASTCYCLE,") != string::npos) {
				HaasData.Q303= removeAll(TmpData, "LASTCYCLE,");

			}
			if (TmpData.find("PREVCYCLE,") != string::npos) {
				HaasData.Q304 = removeAll(TmpData, "PREVCYCLE,");

			}
			if (TmpData.find("PROGRAM,") != string::npos) {
				HaasData.Q500 = removeAll(TmpData, "PROGRAM,");
				HaasData.Q500=removeAll(HaasData.Q500, "PARTS,");
				

			}
			return true;
		}
		
	}
	else {
		return false;
	}
	
}

void HaasMach::FormatServer()
{
	FormatString = HaasData.Q100 + "_" + HaasData.Q101 + "_" + HaasData.Q102 + "_" + HaasData.Q104 + "_"
		+ HaasData.Q200 + "_" + HaasData.Q201 + "_" + HaasData.Q300 + "_" + HaasData.Q301 + "_" + HaasData.Q303 + "_" + HaasData.Q304 + "_" + HaasData.Q500;
	FormatString = removeAll(FormatString, " ");
	FormatString = removeAll(FormatString, "S/N,");
	FormatString = removeAll(FormatString, "SOFTWARE,");
	FormatString = removeAll(FormatString, "MODEL,");
	FormatString = removeAll(FormatString, "MODE,");
	FormatString = removeAll(FormatString, "TOOLCHANGES,");
	FormatString = removeAll(FormatString, "USINGTOOL,");
	FormatString = removeAll(FormatString, "P.O.TIME,");
	FormatString = removeAll(FormatString, "C.S.TIME,");
	FormatString = removeAll(FormatString, "LASTCYCLE,");
	FormatString = removeAll(FormatString, "PREVCYCLE,");
	FormatString = removeAll(FormatString, "PROGRAM,");
	FormatString = replaceAll(FormatString, ":", "-");

}

string HaasMach::removeAll(string str, const string & from)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from)) != std::string::npos) {
		str.erase(start_pos, from.length());
	}
	return str;
}

string HaasMach::replaceAll(string str, const string & from, const string & to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}
