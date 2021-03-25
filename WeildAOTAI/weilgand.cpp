#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip> 
#include <sstream>
using  namespace std;

#define PIN_0 4 // GPIO Pin 17 | Green cable | Data0
#define PIN_1 5// GPIO Pin 18 | White cable | Data1
#define PIN_SOUND 25 // GPIO Pin 26 | Yellow cable | Sound

#define MAXWIEGANDBITS 34
#define READERTIMEOUT 30000000
#define LEN 256

static unsigned char __wiegandData[MAXWIEGANDBITS];
static unsigned long __wiegandBitCount;
static struct timespec __wiegandBitTime;
string  uint8_to_hex_string(uint8_t *v, const size_t s);
//void * loop(void *);
string weilgand_id="0000000000";

void getData0(void) {
   // printf("get_data0\n");
    if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void getData1(void) {
  //  printf("get_data1\n");
    if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandData[__wiegandBitCount / 8] |= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void wiegandInit(int d0pin, int d1pin) {
    wiringPiSetup() ;
    pinMode(d0pin, INPUT);
    pinMode(d1pin, INPUT);
    pullUpDnControl(d0pin,PUD_DOWN);
    pullUpDnControl(d1pin,PUD_DOWN);
    wiringPiISR(d0pin, INT_EDGE_FALLING, getData0);
    wiringPiISR(d1pin, INT_EDGE_FALLING, getData1);

}

void wiegandReset() {
    memset((void *)__wiegandData, 0, MAXWIEGANDBITS);
    __wiegandBitCount = 0;
}

int wiegandGetPendingBitCount() {
    struct timespec now, delta;
    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = now.tv_sec - __wiegandBitTime.tv_sec;
    delta.tv_nsec = now.tv_nsec - __wiegandBitTime.tv_nsec;

    if ((delta.tv_sec > 1) || (delta.tv_nsec > READERTIMEOUT))
        return __wiegandBitCount;

    return 0;
}

int wiegandReadData(void* data, int dataMaxLen) {
    if (wiegandGetPendingBitCount() > 0) {
        int bitCount = __wiegandBitCount;
        int byteCount = (__wiegandBitCount / 8) + 1;
        memcpy(data, (void *)__wiegandData, ((byteCount > dataMaxLen) ? dataMaxLen : byteCount));

        wiegandReset();
        return bitCount;
    }
    return 0;
}

void printCharAsBinary(unsigned char ch) {
    int i;
    FILE * fp;
    fp = fopen("output","a");

    for (i = 0; i < 8; i++) {
        printf("%d", (ch & 0x80) ? 1 : 0);
        fprintf(fp, "%d", (ch & 0x80) ? 1 : 0);
        ch <<= 1;
    }

    fclose(fp);
}


void makeBeep(int millisecs, int times){
    int i;
    for (i = 0; i < times; i++) {
        digitalWrite (PIN_SOUND,  LOW);
        delay(millisecs);
        digitalWrite (PIN_SOUND, HIGH);
        delay(millisecs/2);
    }
}



void wiegand_loop(int d0pin, int d1pin) {
    int i;
    wiegandInit(d0pin, d1pin);
    //data_submit(BIT_WG36);

    while(1) {
        int bitLen = wiegandGetPendingBitCount();
        if (bitLen == 0) {
            usleep(5000);
        } else {
            char data[100];
            char string1[100];
            bitLen = wiegandReadData((void *)data, 100);
            int bytes = bitLen / 8 + 1;
			weilgand_id="";
            for (i = 0; i < bytes; i++)
                weilgand_id.append(uint8_to_hex_string((uint8_t *) &data[i],1));
        }
    }
}
string  uint8_to_hex_string(uint8_t *v, const size_t s) {
	stringstream ss;

	ss << hex << setfill('0');

	for (int i = s - 1; i >= 0; i--) {
		ss << hex << setw(2) << static_cast<int>(v[i]);
	}
	string data = ss.str();
	transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}