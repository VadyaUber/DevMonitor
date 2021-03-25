#include"WeildServer.h"
#include"WeildADC.h"
#include"ElectricMeter.h"
#include"UBMLed.h"
#include"ParserPort.h"
#include <iostream> 
#include <thread>
#include "weilgand.h"
using namespace std;
int main()
{
	WeildServer weild("/root/config.xml", "/root/dataweld.txt"); 
	
	
}