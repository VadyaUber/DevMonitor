#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>  
#include <sys/stat.h>
class WeildLog
{
public:
	void Init(std::string path, uint16_t msizefile);
	~WeildLog();
	void WeildLogOpen();
	void WeildLogWrite(std::string WeildData, std::string FrameData);
	void WeildLogClose();

private:
	std::string PathFile;
	int FileWeld;
	bool FileIsOpen;
	std::string olddata;
	uint16_t maxsize;
};

