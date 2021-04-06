#include"WeildServer.h"
#include"WeildParseDev.h"
#include"WeildUBM.h"
#include"WeildMachineDev.h"
#include "SPI.h"
#include "Rtc.h"
using namespace std;
int main()
{
	WeildServer weild("/root/config.xml", "/root/dataweld.txt"); 
	WeildParseDev * ParserDev= NULL;
	WeildUBM * UbmDev = NULL;
	WeildMachineDev * Machine = NULL;
	MyTime  RtsEvent;
	RtsEvent.IntevralSleep = 3600000;
	SPI RtsSpi("/dev/spidev1.0",1000000,8,0);
	Rtc rtc(&RtsSpi, 5);
	rtc.GetRtc();


	if (weild.WeildConfig.Type_Dev == "PARSER") {
		ParserDev = new WeildParseDev(&weild);
	}
	else if (weild.WeildConfig.Type_Dev == "UBM4") {
		UbmDev = new WeildUBM(&weild);
	}
	else if (weild.WeildConfig.Type_Dev == "MACHIN") {
		Machine = new WeildMachineDev(&weild);
	}


	while (true)
	{
		if (ParserDev!=NULL) {
			ParserDev->WeildParseDevLoop();
		}
		else if (UbmDev!=NULL) {
			UbmDev->UbmLoop();
		}
		else if (Machine!=NULL) {
			Machine->WeildMachineDevLoop();
		}
		if (RtsEvent.CheckTimeEvent()) {
			rtc.SetRtc();
		}
	}
}