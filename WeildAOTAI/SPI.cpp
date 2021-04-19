#include "SPI.h"
SPI::SPI()
{
}
void SPI::init(int &fd, spi_ioc_transfer & tr,string device, uint32_t speed_spi, uint8_t bits, uint8_t modes)
{
	int ret;

	uint8_t mode = get_mode(modes);

	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	

	tr.speed_hz = speed_spi;
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

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &bits);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &bits);
	if (ret == -1)
		pabort("can't get max speed hz");

	/*uint8_t buffwrite[3] = { 0 }, buffread[3] = { 0 };
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = 3;
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	printf("ret %d \n", ret);*/
}
int SPI::SpiWriteRead(int & fd, spi_ioc_transfer & tr, uint8_t * buffwrite, uint8_t * buffread, uint8_t len)
{
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = len;
	int ret;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	printf("ret %d \n", ret);
	//fprintf(stderr, "%s\n", explain_ioctl(fd, SPI_IOC_MESSAGE(1), &tr));
	return ret;
}
//#include "libexplain/ioctl.h"
/*SPI::SPI(string device,uint32_t speed,uint8_t bits,uint8_t modes)
{
	int ret;
	this->speed = speed;
	uint8_t mode = get_mode(modes);
	
	if (wiringPiSetup() == -1) {
		printf("Unable to start wiringPi: \n");
		return;
	}
	this->bits = bits;

	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;
	tr.delay_usecs = 4;
	fd = open(device.c_str(), O_RDWR);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");


	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &this->bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &this->bits);
	
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	/*uint8_t buffwrite[3] = { 0 }, buffread[3] = { 0 };
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = 3;
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	printf("ret %d \n", ret);*/

//}

/*int SPI::SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len)
{
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = len;
	int ret;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	printf("ret %d \n", ret);
	//fprintf(stderr, "%s\n", explain_ioctl(fd, SPI_IOC_MESSAGE(1), &tr));
	return ret;
}*/
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



