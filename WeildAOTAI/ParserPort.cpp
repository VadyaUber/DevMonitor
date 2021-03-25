#include "ParserPort.h"

ParserPort::ParserPort(string port_name)
{
	port = serialOpen("/dev/ttyS1", 9600);
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
	serialPuts(port, data.c_str());
}

void ParserPort::PortLoop()
{
	if (NewDataPort == false) {
		if (serialDataAvail(port) != -1) {
			ch = (char)serialGetchar(port);
			if (ch != '\n' && ch != '\r') {
				DataInput += ch;
			}
			else if (ch == '\n') {
				NewDataPort = true;
			}
		}
		
	}
	delayMicroseconds(100);
}
