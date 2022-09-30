#include "DS1307.h"

DS1307::DS1307(uint8_t cs_dev)
{
    ReadOk = false;
    CntError = 0;
    const char* devName = "/dev/i2c-0";
    // Open up the I2C bus
    fd = open(devName, O_RDWR);
    if (fd == -1)
    {
        perror(devName);
    }
    // Specify the address of the slave device.
    if (ioctl(fd, I2C_SLAVE, DEV_ADDRESS) < 0)
    {
        perror("Failed to acquire bus access and/or talk to slave");
    }
}



void DS1307::GetRtc()
{
    //---read
    char date[10];
    // Write a byte to the slave.
    date[0] = 0;
    if (write(fd, date, 1) != 1)
    {
        perror("Failed to write to the i2c bus");
        //exit(1);
    }

    // Read a byte from the slave.
    if (read(fd, date, DEV_REGISTERS_NUM+1) != DEV_REGISTERS_NUM+1)
    {
        perror("Failed to read from the i2c bus");
    }
    struct tm  tim_RTC = { 0 };

    tim_RTC.tm_sec = _decode(date[0]);
    tim_RTC.tm_min = _decode(date[1]);
    tim_RTC.tm_hour = _decode(date[2]);
    tim_RTC.tm_mday = _decode(date[4]);
    tim_RTC.tm_mon = _decode(date[5]) - 1;
    tim_RTC.tm_year = _decode(date[6]) + 100;

    time_t t1 = mktime(&tim_RTC);
    time_t t2 = time(NULL);
    if ((t2 - 5 * 60 * 60) < t1) {
        ReadOk = true;
        CntError = 0;
        unsigned char buff[32] = { 0 };
        sprintf((char*)buff, (const char*)"date -s \"%02d/%02d/%04d %02d:%02d:%02d\"", tim_RTC.tm_mon + 1, tim_RTC.tm_mday, tim_RTC.tm_year + 1900, tim_RTC.tm_hour, tim_RTC.tm_min, tim_RTC.tm_sec);
        system((const char*)buff);

        unsigned char buffs[32] = { 0 };
        sprintf((char*)buffs, (const char*)"\"%02d/%02d/%04d \"", tim_RTC.tm_year + 1900, tim_RTC.tm_mon + 1, tim_RTC.tm_mday);
        CurrDate = (tim_RTC.tm_year + 1900) * 10000 + (tim_RTC.tm_mon + 1) * 100 + tim_RTC.tm_mday;
        //string dtame = rtc->tim_RTC.tm_mon + 1 + rtc->tim_RTC.tm_mday + rtc->tim_RTC.tm_year + 1900;
        printf("datescurr: %d\n", CurrDate);
        printf("datescurr: %s\n", buffs);
    }
    else {
        ReadOk = false;
        printf("error read RTC \n\r");
        CntError++;
    }
}

void DS1307::SetRtc()
{
    CntError = 0;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char buf[10];
    // Write a byte to the slave.
    //buf[0] = 0;
    //buf[1] = _encode(tm.tm_min);
    //buf[2] = _encode(tm.tm_hour & 0x3f);
    //buf[3] = 0;
    //buf[4] = _encode(tm.tm_mday);
    //buf[5] = _encode(tm.tm_mon + 1);
    //buf[6] = _encode((tm.tm_year - 100));


    buf[0] = 0;
    buf[1] = _encode(tm.tm_sec);
    buf[2] = _encode(tm.tm_min);
    buf[3] = _encode((tm.tm_hour & 0x3f));
    buf[4] = _encode((tm.tm_wday+1));
    buf[5] = _encode(tm.tm_mday);
    buf[6] = _encode((tm.tm_mon + 1));
    buf[7] = _encode((tm.tm_year - 100));
    if (write(fd, buf, DEV_REGISTERS_NUM+2) != DEV_REGISTERS_NUM+2)
    {
        perror("Failed to write to the i2c bus");
        //exit(1);
    }
}
// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS1307::_encode(uint8_t value)
{
    return (value % 10 + ((value / 10) << 4));
}
// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS1307::_decode(uint8_t value)
{
    return (((value & 0xf0) >> 4) * 10) + (value & 0x0f);
}
