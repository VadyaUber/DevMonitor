#include"DevServer.h"
#include"WeildParseDev.h"
#include"WeildUBM.h"
#include"WeildMachineDev.h"
#include"WeildMachineSpotDev.h"
#include "Rtc.h"
#include "WeildUBMLast.h"
using namespace std;
int main()
{
	DevServer weild("/weildpath/config_ubm4.xml", "/weildpath/history.txt"); 
	WeildParseDev * ParserDev= NULL;
	WeildUBM * UbmDev = NULL;
	WeildMachineDev * Machine = NULL;
	WeildMachineSpotDev* MachineSpot = NULL;
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
	else if (weild.WeildConfig.Type_Dev == "SPOT_WELD_MACHIN") {
		MachineSpot = new WeildMachineSpotDev(&weild);
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
		else if (MachineSpot != NULL) {
			MachineSpot->WeildMachineSpotDevLoop();
		}
		else if (UbmLast!=NULL) {
			UbmLast->UbmLoop();
		}
	}
}
