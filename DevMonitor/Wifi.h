#pragma once
#include <iostream>
#include <sys/ioctl.h>
#include <net/if.h>
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "wifi_scan.h"
#include <string>
using namespace std;
class Wifi
{
public:
	bool create_conect(string sid, string pass, bool hidden = false);
	bool get_connect(int dummy_fd, string name);
	string GetIpInterf(string interf);
	string get_bd(string interfece);
	string GetMyIpInterfece(string interfece);
	string set_comand_cmd(string command);

private:
	bool check_connect(string sid);
	bool check_add(string sid);
	
};

