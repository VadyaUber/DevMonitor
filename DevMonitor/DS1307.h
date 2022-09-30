#pragma once
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <ctime>
#include <cstring>

#define DEV_ADDRESS 0x68
#define DEV_REGISTERS_NUM 6
#define MAX_ERROR_RTC 50
using namespace std;

class DS1307
{
public:
	DS1307(uint8_t cs_dev);
	void SetRtc();
	void GetRtc();
	bool ReadOk;
	int CntError;
	uint64_t CurrDate;
private:

	uint8_t CS;
	uint8_t _encode(uint8_t value);
	uint8_t _decode(uint8_t value);
	int fd;
};
