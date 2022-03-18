#include "LogToUSB.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>

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
	return true;
}

bool DevLogUSB::USBumount()
{
	string devcommand = "umount /usb1";
	PipCmd(devcommand);
	USBstate = true;
	DevLog log;
	log.FileIsOpen = false;
	return true;
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
			if (PipCmd("ls /usb1/| grep copy_modules_to_usb.txt") == "copy_modules_to_usb.txt")	//copy modules.xml to flash
			{
				USBblink = true;
				PipCmd("rsync  -a --no-owner --no-group /weildpath/modules.xml /usb1/");
			}

			if (PipCmd("find /usb1/e61eb81163c8840a02f8be80d9e69fbb | wc -l") == "2") //check programm folder
			{
				string namefile = PipCmd("ls /usb1/e61eb81163c8840a02f8be80d9e69fbb | grep d");
				string sizefile = PipCmd("ls -l /usb1/e61eb81163c8840a02f8be80d9e69fbb | grep d | awk '{print  $5}'");
				string versionfile = namefile.substr(namefile.length()-3,3);
				string mydyf = md5(sizefile + "_" + versionfile);
				if (namefile == "d" + md5(sizefile + "_" + versionfile) + versionfile)
				{
					chdir("/");
					system("mount -o remount,rw /dev/mmcblk0p1");
					system("mv /media/root-ro/root/projects/DevMonitor/bin/ARM/Debug/DevMonitor.out /media/root-ro/root/projects/DevMonitor/bin/ARM/Debug/OldDevMonitor.out");
					string copypth = "rsync  -a --no-owner --no-group /usb1/e61eb81163c8840a02f8be80d9e69fbb/" + namefile + " /media/root-ro/root/projects/DevMonitor/bin/ARM/Debug/";
					system(copypth.c_str());
					string rnm = "mv /media/root-ro/root/projects/DevMonitor/bin/ARM/Debug/" + namefile + " /media/root-ro/root/projects/DevMonitor/bin/ARM/Debug/DevMonitor.out";
					system(rnm.c_str());
					system("mount -o remount,ro /dev/mmcblk0p1");
					system("exit");
					printf("DevMonitor updated");
					USBblink = true;
					USBConfigRead = true;
				}
			}

			if (PipCmd("ls /usb1/| grep copy_log_to_usb.txt") == "copy_log_to_usb.txt")	//check md5
			{
				USBblink = true;
				PipCmd("mv /weildpath/history.txt /weildpath/history_" + mac + ".txt"); //rename
				PipCmd("rsync  -a --no-owner --no-group --remove-source-files /weildpath/history_" + mac + ".txt /usb1/");
			}
			if (PipCmd("ls /usb1/| grep copy_config_to_usb.txt") == "copy_config_to_usb.txt")
			{
				USBblink = true;
				PipCmd("rsync  -a --no-owner --no-group /weildpath/config_ubm4.xml /usb1/");
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
				PipCmd("systemctl restart Monitor.service");
		}
	}
}