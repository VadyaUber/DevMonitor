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
	ReadOk = false;
	CntError = 0;
	//wiringPiSetup();
	CS = cs_dev;
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
	//spi = SPI("/dev/spidev1.0", 1000000, 8, 1);
	init_SPI("/dev/spidev1.0", 2000000, 8, 3, RTC);
	
}

void Rtc::SetRtc()
{
	if (NameSPI != RTC) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", 2000000, 8, 3, RTC);

	}
	CntError = 0;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	write_reg_rtc(REG_SEC, _encode(tm.tm_sec));
	write_reg_rtc(REG_MIN, _encode(tm.tm_min));
	write_reg_rtc(REG_HOUR, _encode(tm.tm_hour));
	write_reg_rtc(REG_DATE, _encode(tm.tm_mday));
	write_reg_rtc(REG_MON, _encode(tm.tm_mon + 1));
	write_reg_rtc(REG_YEAR, _encode((tm.tm_year-100)));
}

void Rtc::GetRtc()
{
	if (NameSPI != RTC) {
		DeInitSPI();
		init_SPI("/dev/spidev1.0", 2000000, 8, 3, RTC);

	}
	/*or (int i = 0; i < 5; i++) {
		if (ValidDataRtc()) {*/
			//break;
		//}
	ValidDataRtc();
	//}
	
	

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

bool Rtc::ValidDataRtc()
{
	
	uint8_t rx[8] = { 0 };
	uint8_t tx[8] = { 0 };
	digitalWrite(CS, LOW);
	SpiWriteRead(tx, rx, 5);
    digitalWrite(CS, HIGH);

	digitalWrite(CS, LOW);
	SpiWriteRead(tx, rx, 8);
	digitalWrite(CS, HIGH);
	struct tm  tim_RTC = { 0 };

	tim_RTC.tm_sec = _decode(rx[1]);
	tim_RTC.tm_min = _decode(rx[2]);
	tim_RTC.tm_hour = _decode(rx[3] & 0x3f);
	tim_RTC.tm_mday = _decode(rx[5]);
	tim_RTC.tm_mon = _decode(rx[6] & 0x1f)-1;
	tim_RTC.tm_year = _decode(rx[7]) + 100;

	time_t t1 = mktime(&tim_RTC);
	time_t t2 = time(NULL);
	if ((t2-5*60*60) < t1) {
		ReadOk = true;
		CntError = 0;
		unsigned char buff[32] = { 0 };
		sprintf((char*)buff, (const char *)"date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", tim_RTC.tm_mon+1, tim_RTC.tm_mday, tim_RTC.tm_year + 1900, tim_RTC.tm_hour, tim_RTC.tm_min, tim_RTC.tm_sec);
		system((const char *)buff);
		return true;
	}
	else {
		ReadOk = false;
		printf("error read RTC \n\r");
		CntError++;
	}

	return false;
}
