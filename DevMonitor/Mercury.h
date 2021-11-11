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
//#include <c++/9/cstddef>

#include <wiringPi.h>

#define VOLTAGE_ASYMMETRY_MAX     4
#define NOMINAL_SUPPLY_VOLTAGE     220
#define VOLTAGE_DEVIATION_PERCENT_MAX     10
#define VOLTAGE_DEVIATION_MAX     NOMINAL_SUPPLY_VOLTAGE*VOLTAGE_DEVIATION_PERCENT_MAX/100
#define EM_SCALE_COEFFICIENT     100
#define NOMINAL_POWER_FREQUENCY     50
#define FREQUENCY_DEVIATION_MAX     0.4

using namespace std;

class Merc
{
public:
	Merc(string devUSB, uint32_t baud_rate);
	void get_data_merc();
	double power[9] = { 0 };
	uint32_t senegj = 0;;
	uint32_t enegj = 0;
	uint32_t sumpower = 0;
	uint8_t power_quality = 0;

private:
	uint8_t zapros[11] = { 0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x77,0x81 };
	uint8_t array_phase[9] = { 0b00000001 , 0b00000010, 0b00000011, 0b00001001 , 0b00001010, 0b00001011, 0b00000101 , 0b00000110, 0b00000111 };
	uint8_t data_format[6] = { 0x00,0x08,0x11,0x00,0,0 }; //чтение мощности
	uint8_t datel_format[6] = { 0x00,0x05,0x00,0x00,0,0 };

	int serial_port;
	uint16_t CRC16_2(unsigned char* buf, int len);
	int openserialport(string devUSB, uint32_t baud_rate);
	int writeserial(char* buff, size_t size);
	int readserial(uint8_t buf_read, size_t size);
	int mercInit();
	void getLivePower();
	void getWats();
	uint32_t getEnergy();
	uint32_t getVoltage(uint8_t phase);
	double getFreq();
	void power_quality_analyze();
};

