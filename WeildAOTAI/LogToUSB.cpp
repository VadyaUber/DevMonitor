#include "LogToUSB.h"

void WeildLogUSB::Init(string devicemac)
{
	USBBlinkCount = 0;
	mac = devicemac;
	USBstate = false;
	USBblink = false;
	USBConfigRead = false;
}

string WeildLogUSB::PipCmd(string command)
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

bool WeildLogUSB::USBmount(string device)
{
	string devcommand = "mount ";
	devcommand.append(device);
	devcommand.append(" /usb1");
	string buf_out = "";
	buf_out = PipCmd(devcommand);
}

bool WeildLogUSB::USBumount()
{
	string devcommand = "umount /usb1";
	PipCmd(devcommand);
	USBstate = true;
	WeildLog log;
	log.FileIsOpen = false;

}

bool WeildLogUSB::USBconnect()
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
			if (PipCmd("ls /usb1/| grep config.xml") == "config.xml")	//check md5
			{
				if (PipCmd("rsync  -a --no-owner --no-group  /usb1/config.xml /weildpath/") == "")
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