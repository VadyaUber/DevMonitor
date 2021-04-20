#include"WeildServer.h"
#include"WeildParseDev.h"
#include"WeildUBM.h"
#include"WeildMachineDev.h"
#include "SPI.h"
#include "Rtc.h"
#include "WeildUBMLast.h"
using namespace std;
int main()
{
	WeildServer weild("/weildpath/config.xml", "/weildpath/histoty.txt"); 
	WeildParseDev * ParserDev= NULL;
	WeildUBM * UbmDev = NULL;
	WeildMachineDev * Machine = NULL;
	WeildUBMLast * UbmLast = NULL;
	MyTime  RtsEvent;
    RtsEvent.IntevralSleep = 3600000;

	if (weild.WeildConfig.Type_Dev == "PARSER") {
		ParserDev = new WeildParseDev(&weild);
	}
	else if (weild.WeildConfig.Type_Dev == "UBM4") {
		UbmDev = new WeildUBM(&weild);
	}
	else if (weild.WeildConfig.Type_Dev == "MACHIN") {
		Machine = new WeildMachineDev(&weild);
	}
	else if (weild.WeildConfig.Type_Dev == "UBM4_LAST") {
		UbmLast = new WeildUBMLast(&weild);
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
		else if (UbmLast!=NULL) {
			UbmLast->UbmLoop();

		}

	}
}