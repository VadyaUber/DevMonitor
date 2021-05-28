#include "QrDev.h"

QrDev::QrDev(string port_name)
{
	port = serialOpen(port_name.c_str(), 115200);
	new thread([&]() {
		while (true)
		{
			Loop();
		}

		});
}

void QrDev::GetQrData(string * data)
{
	if (NewData) {
		*data = data_qr;
		data_qr = "";
		NewData = false;
	}
}

void QrDev::Loop()
{
	if (NewData == false) {
		if (serialDataAvail(port) != -1) {
			ch = (char)serialGetchar(port);
			if (ch != '\n' && ch != '\r' && ch != ';') {
				data_qr += ch;
			}
			else if (ch == '\n') {
				NewData = true;
			}
		}

	}
	delayMicroseconds(1000);
}
