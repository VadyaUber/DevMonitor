#include "DigitalInput8bit.h"
#define BIT_OTA  0x40
#define BIT_UVA  0x80
#define BIT_LOW  0x02
DigitalInput8bit::DigitalInput8bit( uint8_t CS_pin)
{
	spi = SPI("/dev/spidev1.0", 1000000, 8, 2);
	CS = CS_pin;
	pinMode(CS, OUTPUT);
}

void DigitalInput8bit::ReadData()
{
	uint8_t byf_tx[2] = { 0 }, byf_rx[2] = { 0 };
	digitalWrite(CS, LOW);
	spi.SpiWriteRead(byf_tx, byf_rx, 2);
	digitalWrite(CS, HIGH);
	if (byf_rx[0] & BIT_LOW) {
		IsEnable = false;
		Value = 0;
	}
	else {
		IsEnable = true;
		Over_Temper = (byf_rx[0] & BIT_OTA);
		Falult_Voltage=(byf_rx[0] & BIT_UVA);
		Value = byf_rx[1];
	}
}
