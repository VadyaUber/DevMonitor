#include "PortDevMachine.h"

PortDevMachine::PortDevMachine(string  port_name)
{
	this->port = serialOpen(port_name.c_str(), 9600);
	new thread([&]() {
		while (true)
		{
			PortLoop();
		}

		});
}

void PortDevMachine::GetParseData(string * data)
{
	if (NewDataPort) {
		NewDataPort = false;
		*data = DataInput;
		DataInput = "";
	}
}

void PortDevMachine::WriteData(string data)
{
	const char * char_ref = data.c_str();
	for (int i = 0; i < data.length(); i++)serialPutchar(port, char_ref[i]);
}

void PortDevMachine::PortLoop()
{
	if (NewDataPort == false) {
		if (serialDataAvail(port) >0) {
			ch = (char)serialGetchar(port);
			
				DataInput += ch;
			
		
		}
		else if (DataInput != "") {

			NewDataPort = true;

		}

	}
	delayMicroseconds(10000);
}
