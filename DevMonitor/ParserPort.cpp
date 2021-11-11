#include "ParserPort.h"

ParserPort::ParserPort(string port_name)
{
	port = serialOpen(port_name.c_str(), 9600);
	new thread ([&]() {
		while (true)
		{
			PortLoop();
		}
		
	});
}

void ParserPort::GetParseData(string * data)
{
	if (NewDataPort) {
		NewDataPort = false;
		*data = DataInput;
		DataInput = "";
	}
}

void ParserPort::WriteData(string data)
{
	const char * char_ref = data.c_str();
	for (int i = 0; i < data.length(); i++)serialPutchar(port, char_ref[i]);
	//serialPuts(port, data.c_str());
}

void ParserPort::PortLoop()
{
	if (NewDataPort == false) {
		if (serialDataAvail(port) != -1) {
			ch = (char)serialGetchar(port);
			if (ch != '\n' && ch != '\r' && ch!=';') {
				DataInput += ch;
			}
			else if (ch == '\n') {
				NewDataPort = true;
			}
		}
		
	}
	delayMicroseconds(1000);
}
