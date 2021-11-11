#include "Rfid.h"

Rfid::Rfid(int d0pin, int d1pin)
{
	wiringPiSetup();
	pinMode(d0pin, INPUT);
	pinMode(d1pin, INPUT);
	pullUpDnControl(d0pin, PUD_DOWN);
	pullUpDnControl(d1pin, PUD_DOWN);
	/*wiringPiISR(d0pin, INT_EDGE_FALLING, this->getData0);
	wiringPiISR(d1pin, INT_EDGE_FALLING, this->getData1);*/
	new thread([&]() { loop(); });
}

void Rfid::getData0(void)
{
	if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
		__wiegandData[__wiegandBitCount / 8] <<= 1;
		__wiegandBitCount++;
	}
	clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void Rfid::getData1(void)
{
	if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
		__wiegandData[__wiegandBitCount / 8] <<= 1;
		__wiegandData[__wiegandBitCount / 8] |= 1;
		__wiegandBitCount++;
	}
	clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void Rfid::wiegandReset()
{
	memset((void *)__wiegandData, 0, MAXWIEGANDBITS);
	__wiegandBitCount = 0;
}

int Rfid::wiegandGetPendingBitCount()
{
	struct timespec now, delta;
	clock_gettime(CLOCK_MONOTONIC, &now);
	delta.tv_sec = now.tv_sec - __wiegandBitTime.tv_sec;
	delta.tv_nsec = now.tv_nsec - __wiegandBitTime.tv_nsec;

	if ((delta.tv_sec > 1) || (delta.tv_nsec > READERTIMEOUT))
		return __wiegandBitCount;

	return 0;
}

int Rfid::wiegandReadData(void * data, int dataMaxLen)
{
	if (wiegandGetPendingBitCount() > 0) {
		int bitCount = __wiegandBitCount;
		int byteCount = (__wiegandBitCount / 8) + 1;
		memcpy(data, (void *)__wiegandData, ((byteCount > dataMaxLen) ? dataMaxLen : byteCount));

		wiegandReset();
		return bitCount;
	}
	return 0;
}


string Rfid::uint8_to_hex_string(uint8_t * v, const size_t s)
{
	stringstream ss;

	ss << hex << setfill('0');

	for (int i = s - 1; i >= 0; i--) {
		ss << hex << setw(2) << static_cast<int>(v[i]);
	}
	string data = ss.str();
	transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}

void Rfid::loop()
{
	int i;

	while (1) {
		int bitLen = wiegandGetPendingBitCount();
		if (bitLen == 0) {
			usleep(500);
		}
		else {
			char data[100];
			char string1[100];
			bitLen = wiegandReadData((void *)data, 100);
			int bytes = bitLen / 8 + 1;
			Rfid_value = "";
			for (i = 0; i < bytes; i++)
				Rfid_value.append(uint8_to_hex_string((uint8_t *)&data[i], 1));
			NewData = true;
		}
	}
}
