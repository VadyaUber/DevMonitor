#include "DigitalInput8bit.h"
#define BIT_OTA  0x40
#define BIT_UVA  0x80
#define BIT_LOW  0x02

DigitalInput8bit::DigitalInput8bit( uint8_t CS_pin)
{
	//spi = SPI("/dev/spidev1.0", 1000000, 8, 2);
	/*init_SPI("/dev/spidev1.0", 1000000, 8, 0, DigitalInput);*/
	CS = CS_pin;
	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);
}


void DigitalInput8bit::ReadData() 
{

	//if (NameSPI != DigitalInput) {
	//	DeInitSPI();
	//	init_SPI("/dev/spidev1.0", 1000000, 8, 3, DigitalInput);
	//}
	uint8_t byf_tx[2] = { 0 }, byf_rx[2] = { 0 };
	digitalWrite(CS, LOW);
	SpiWriteRead(byf_tx, byf_rx, 2);
	digitalWrite(CS, HIGH);
	
	if ((byf_rx[1] & BIT_LOW)!=0) {
		IsEnable = false;
		Value = 0;
	}
	else {
		IsEnable = true;
		Over_Temper = (byf_rx[1] & BIT_OTA)==0;
		Falult_Voltage=(byf_rx[1] & BIT_UVA)==0 ;
		Value = byf_rx[0];
	}
	
}