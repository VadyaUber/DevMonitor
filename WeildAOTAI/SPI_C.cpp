#include "SPI_C.h"
using namespace std;

static int fd;
static struct spi_ioc_transfer tr;
bool IsInitSPI = false;
volatile uint8_t NameSPI = 0;
uint8_t get_mode(uint8_t modes);
void pabort(const char *s);



void init_SPI(string device, uint32_t speed_spi, uint8_t bits, uint8_t modes, uint8_t name ) {
		if (!IsInitSPI) 
		{
		/*if (wiringPiSetup() == -1) {
			printf("Unable to start wiringPi: \n");
			return;
		}*/
		NameSPI = name;
		int ret;
		uint8_t mode = get_mode(modes);
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


		IsInitSPI = true;
		}
}
void DeInitSPI() {
	close(fd);
	IsInitSPI = false;
}
int SpiWriteRead(uint8_t * buffwrite, uint8_t * buffread, uint8_t len)
{
	tr.tx_buf = (unsigned long)buffwrite;
	tr.rx_buf = (unsigned long)buffread;
	tr.len = len;
	int ret;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	//printf("ret %d \n", ret);
	//fprintf(stderr, "%s\n", explain_ioctl(fd, SPI_IOC_MESSAGE(1), &tr));
	return ret;
}
void pabort(const char *s)
{

	perror(s);
	abort();
}
uint8_t get_mode(uint8_t modes)
{
	switch (modes)
	{
	case 0:
		//pullUpDnControl(14, PUD_DOWN);
		return SPI_MODE_0;
		break;
	case 1:
		//pullUpDnControl(14, PUD_DOWN);
		return SPI_MODE_1;
		break;
	case 2:
		//pullUpDnControl(14, PUD_UP);
		return SPI_MODE_2;
		break;
	case 3:
		//pullUpDnControl(14, PUD_UP);
		return SPI_MODE_3;
		break;


	}
	return 0;
}
