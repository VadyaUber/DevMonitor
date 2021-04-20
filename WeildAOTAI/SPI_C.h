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
#include <map>
using namespace std;
extern string NameSPI;
int SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len);
void init_SPI(string device, uint32_t speed_spi, uint8_t bits, uint8_t modes, string name);
void DeInitSPI();