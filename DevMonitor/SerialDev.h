#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>

#include <errno.h> 
#include <unistd.h>
#include <termios.h> 
#include <fcntl.h>
#include <cstddef>
#include <sys/ioctl.h>

#include <wiringPi.h>

using namespace std;

class SerialPort
{
public:
	int serial_port;
	SerialPort(string devUSB, uint32_t baud_rate);
	int writeserial(char* buff, size_t size);
	int readserial(uint8_t buf_read, size_t size);
	uint16_t CRC16(uint8_t* data, uint16_t size);

private:
	uint16_t CRC16_2(unsigned char* buf, int len);
	int openserialport(string devUSB, uint32_t baud_rate);
};
