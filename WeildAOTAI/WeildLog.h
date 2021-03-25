#pragma once
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>  
class WeildLog
{
public:
	void Init(std::string path);
	~WeildLog();
	void WeildLogOpen();
	void WeildLogWrite(std::string WeildData);
	void WeildLogClose();

private:
	std::string PathFile;
	int FileWeld;
	bool FileIsOpen;
};

