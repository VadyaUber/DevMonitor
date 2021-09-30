#include "Wifi.h"

bool Wifi::create_conect(string sid, string pass)
{
	string result = "";
	set_comand_cmd("nmcli device wifi rescan");
	std::string::size_type n;
	if (!check_connect(sid)) {
		if (check_add(sid)) {
			result = set_comand_cmd("nmcli connection  delete " + sid);
		}
		result = set_comand_cmd("nmcli device wifi connect " + sid + " password " + pass);
		n = result.find("Device 'wlan0' successfully activated");
		if (n == std::string::npos)return false;
		return true;
	}
	return true;

}
bool Wifi::get_connect(int dummy_fd, string name)
{
	struct ifreq ifr;
	bool up_and_running;
	memset(&ifr, 0, sizeof(ifr));

	strcpy(ifr.ifr_name, name.c_str());

	if (ioctl(dummy_fd, SIOCGIFFLAGS, &ifr) != -1)
	{
		up_and_running = (ifr.ifr_flags & (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING);
		//printf("wlan0 %d\n",up_and_running);
		if (up_and_running == 1) return true;
	}
	else
	{
		// printf("wlan0 eth0\n");
	}



	return false;
}
string Wifi::get_bd(string interfece) {
	if (interfece == "eth0") {
		return "1";
	}
	struct station_info station;
	struct bss_info bss[10];
	struct wifi_scan *wifi = wifi_scan_init(interfece.c_str());
	wifi_scan_station(wifi, &station);
	wifi_scan_close(wifi);
	//printf("%d",station.signal_avg_dbm);
	return to_string(station.signal_avg_dbm);
}
string Wifi::GetMyIpInterfece(string interfece)
{
	string ipAddress = "";
	struct ifaddrs *interfaces = NULL;
	struct ifaddrs *temp_addr = NULL;
	string name = "";
	int success = 0;
	// retrieve the current interfaces - returns 0 on success
	success = getifaddrs(&interfaces);
	if (success == 0) {
		// Loop through linked list of interfaces
		
		temp_addr = interfaces;
		while (temp_addr != NULL) {
			if (temp_addr->ifa_addr->sa_family == AF_INET) {
				// Check if interface is en0 which is the wifi connection on the iPhone
				name = temp_addr->ifa_name;
				if (name == interfece) {
					ipAddress = inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
				}
			}
			temp_addr = temp_addr->ifa_next;
		}
			
		}
	
	// Free memory
	freeifaddrs(interfaces);
	return ipAddress;
}

string Wifi::GetIpInterf(string interf) {
	string out_commad = set_comand_cmd("route -n | grep 'UG[ \t]'| grep " + interf + "| awk '{print $2}'");
	out_commad = out_commad.substr(0, out_commad.length() - 1);

	return out_commad;
}
bool Wifi:: check_add(string sid) {
	string result = "";
	result = set_comand_cmd("nmcli connection show | grep wifi |grep " + sid);
	std::string::size_type n;
	n = result.find(sid);
	if (n == std::string::npos)return false;
	return true;

}
bool Wifi::check_connect(string sid) {
	std::string::size_type n;
	string result = "";
	result = set_comand_cmd("nmcli device wifi | grep " + sid);
	//result = user_rep(result);

	n = result.find("*");
	if (n == std::string::npos)return false;
	return true;
}
string Wifi::set_comand_cmd(string command) {
	string buf_out = "";
	char buffer[10];
	FILE *f = popen(command.c_str(), "r");
	while (fgets(buffer, 10, f) != nullptr)
	{
		buf_out += buffer;
	}

	pclose(f);
	return buf_out;
}