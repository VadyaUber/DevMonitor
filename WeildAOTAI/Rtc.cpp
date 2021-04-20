#include "Rtc.h"
#define REG_SEC			0x00
#define REG_MIN			0x01
#define REG_HOUR		0x02
#define REG_DOW			0x03
#define REG_DATE		0x04
#define REG_MON			0x05
#define REG_YEAR		0x06
#define REG_CONTROL		0x0E
#define REG_STATUS		0x0F
#define REG_TEMP_MSB	0x11
#define REG_TEMP_LSB	0x12
#define REG_RAM_ADDR	0x18
#define REG_RAM_DATA	0x19
Rtc::Rtc(uint8_t cs_dev)
{
	wiringPiSetup();
	CS = cs_dev;
	//spi = SPI("/dev/spidev1.0", 1000000, 8, 1);
	init_SPI("/dev/spidev1.0", 1000000, 8, 1,"RTC");
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
}

void Rtc::SetRtc()
{
	if (NameSPI != "RTC") {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", 1000000, 8, 1, "RTC");

	}
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	write_reg_rtc(REG_SEC, _encode(tm.tm_sec));
	write_reg_rtc(REG_MIN, _encode(tm.tm_min));
	write_reg_rtc(REG_HOUR, _encode(tm.tm_hour));
	write_reg_rtc(REG_DATE, _encode(tm.tm_mday));
	write_reg_rtc(REG_MON, _encode(tm.tm_mon + 1));
	write_reg_rtc(REG_YEAR, _encode((tm.tm_year + 1900) - 2000));
}

void Rtc::GetRtc()
{
	if (NameSPI != "RTC") {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", 1000000, 8, 1, "RTC");

	}
	uint8_t tx[8] = { 0 };
	uint8_t rx[8] = { 0 };
	digitalWrite(CS, LOW);
	SpiWriteRead(tx, rx, 8);
	digitalWrite(CS, HIGH);
	struct tm tm;
	tm.tm_sec = _decode(rx[1]);
	tm.tm_min = _decode(rx[2]);
	tm.tm_hour = _decode(rx[3] & 0x3f);
	tm.tm_mday = _decode(rx[5]);
	tm.tm_mon = _decode(rx[6] & 0x1f) - 1;
	tm.tm_year = ((_decode(rx[7]) + 2000) - 1900);
	unsigned char buff[32] = { 0 };
	sprintf((char*)buff, (const char *)"date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", tm.tm_mon, tm.tm_mday, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec);
	system((const char *)buff);
	/*struct timeval  stime;
	stime.tv_sec = mktime(&tm);
	stime.tv_usec = 0;
	if (stime.tv_sec < 0)stime.tv_sec = 0;
	settimeofday(&stime, NULL);*/
}

uint8_t Rtc::_encode(uint8_t value)
{
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}

uint8_t Rtc::_decode(uint8_t value)
{
	return (value & 0xf) + (((value & 0xf0) >> 4) * 10);
}

void Rtc::write_reg_rtc(uint8_t reg, uint8_t data)
{
	uint8_t tx[2] = { 0 };
	uint8_t rx[2] = { 0 };

	tx[0] = reg + 0x80;

	tx[1] = data;
	digitalWrite(CS, LOW);
	SpiWriteRead(tx, rx, 2);
	digitalWrite(CS, HIGH);
}
