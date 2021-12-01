#include "LogToUSB.h"

void DevLogUSB::Init(string devicemac)
{
	USBBlinkCount = 0;
	mac = devicemac;
	USBstate = false;
	USBblink = false;
	USBConfigRead = false;
}

string DevLogUSB::PipCmd(string command)
{
	string buf_out = "";
	char buffer[10];
	FILE* f = popen(command.c_str(), "r");
	while (fgets(buffer, 10, f) != nullptr)
	{
		buf_out += buffer;
	}
	pclose(f);
	return buf_out.substr(0, buf_out.length() - 1);
}

bool DevLogUSB::USBmount(string device)
{
	string devcommand = "mount ";
	devcommand.append(device);
	devcommand.append(" /usb1");
	string buf_out = "";
	buf_out = PipCmd(devcommand);
}

bool DevLogUSB::USBumount()
{
	string devcommand = "umount /usb1";
	PipCmd(devcommand);
	USBstate = true;
	DevLog log;
	log.FileIsOpen = false;

}

bool DevLogUSB::USBconnect()
{
	string dev = "";
	dev = PipCmd("sudo fdisk -l | grep FAT32 | awk '{print $1}'");
	if ((dev.length() > 0))
	{
		if (!USBstate)
		{
			USBmount(dev);

			if (PipCmd("ls /usb1/| grep copy_log_to_usb.txt") == "copy_log_to_usb.txt")	//check md5
			{
				USBblink = true;
				PipCmd("mv /weildpath/history.txt /weildpath/history_" + mac + ".txt"); //rename
				PipCmd("rsync  -a --no-owner --no-group --remove-source-files /weildpath/history_" + mac + ".txt /usb1/");
			}
			if (PipCmd("ls /usb1/| grep config_ubm4.xml") == "config_ubm4.xml")	//check md5
			{
				if (PipCmd("rsync  -a --no-owner --no-group  /usb1/config_ubm4.xml /weildpath/") == "")
				{
					USBblink = true;
					USBConfigRead = true;
				}
				
			}
			USBumount();
		}
	}
	else
	{
		USBstate = false;
	}
	if (USBblink)
	{
		USBBlinkCount++;
		if (USBBlinkCount > 40)
		{
			USBblink = false;
			USBBlinkCount = 0;
			if (USBConfigRead)
				PipCmd("systemctl restart Weld.service");
		}
	}
}