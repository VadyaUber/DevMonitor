#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <wiringPi.h>
using namespace std;
extern string weilgand_id;
extern bool wg_upd;
void wiegand_loop(int d0pin, int d1pin,bool WG26);
void getData0(void);
void getData1(void);
string  uint8_to_hex_string(uint8_t* v, const size_t s);