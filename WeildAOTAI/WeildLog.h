#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>  
#include <sys/stat.h>
using namespace std;
class WeildLog
{
public:
	void Init(std::string path, uint16_t msizefile);
	~WeildLog();
	void WeildLogOpen();
	void WeildLogWrite(std::string WeildData, std::string FrameData);
	void WeildLogClose();
	bool FileIsOpen;

private:
	std::string PathFile;
	int FileWeld;
	std::string olddata;
	uint16_t maxsize;
};

