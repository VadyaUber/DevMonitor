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
	static void init(int & fd, spi_ioc_transfer & tr,string device, uint32_t speed_spi, uint8_t bits, uint8_t modes);
	static int SpiWriteRead(int & fd, struct spi_ioc_transfer & tr,uint8_t * buffwrite, uint8_t * buffread, uint8_t len);
private:
	static uint8_t get_mode(uint8_t modes);

	static void pabort(const char *s);
};

