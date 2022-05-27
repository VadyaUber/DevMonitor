#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>  
#include <sys/stat.h>
using namespace std;
class DevLog
{
public:
	void Init(std::string path, uint16_t msizefile);
	~DevLog();
	void DevLogOpen();
	bool DevLogWrite(std::string WeildData, std::string FrameData);
	void DevLogClose();
	bool FileIsOpen;
	string olddata;
private:
	std::string PathFile;
	int FileWeld;
	//std::string olddata;
	uint16_t maxsize;
};

