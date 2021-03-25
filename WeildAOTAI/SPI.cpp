#include "SPI.h"

SPI::SPI(string device,uint32_t speed,uint8_t bits,uint8_t modes)
{
	int ret;
	uint8_t mode = get_mode(modes);
	
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.delay_usecs = 4;
	fd = open(device.c_str(), O_RDWR);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");


	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");
}

void SPI::SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len)
{
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = len;
	int ret;
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}
void SPI::pabort(const char *s)
{
	perror(s);
	abort();
}
uint8_t SPI::get_mode(uint8_t modes)
{
	switch (modes)
	{
	case 0:
		return SPI_MODE_0;
		break;
	case 1:
		return SPI_MODE_1;
		break;
	case 2:
		return SPI_MODE_2;
		break;
	case 3:
		return SPI_MODE_3;
		break;


	}
	return 0;
}



