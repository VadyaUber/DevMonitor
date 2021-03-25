#pragma once
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <wiringPi.h> 
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
class SPI
{
public:
	SPI(string device, uint32_t speed_spi, uint8_t bits, uint8_t modes);
	void SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len);
private:
	struct spi_ioc_transfer tr;
	uint8_t get_mode(uint8_t modes);
	int fd;
	void pabort(const char *s);
};

