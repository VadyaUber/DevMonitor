#pragma once

#include <stdio.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <wiringPi.h> 
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
using namespace std;
class SPI
{
public:
	SPI();
	SPI(string device, uint32_t speed_spi, uint8_t bits, uint8_t modes);
	int SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len);
private:
	struct spi_ioc_transfer tr;
	uint8_t get_mode(uint8_t modes);
	int fd=0;
	void pabort(const char *s);
	uint32_t speed=0;
	uint8_t bits=0;
};

