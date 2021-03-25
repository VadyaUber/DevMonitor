#include "WeildLog.h"



void WeildLog::Init(std::string path)
{
	PathFile = path;	
	FileIsOpen = false;
}
void WeildLog::WeildLogOpen() {
	FileWeld= open(PathFile.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	FileIsOpen = true;
}
void WeildLog::WeildLogWrite(std::string  WeildData)
{
	if (!FileIsOpen) {
		WeildLogOpen();
	}
	write(FileWeld, WeildData.c_str(), WeildData.size());
}

void WeildLog::WeildLogClose()
{
	FileIsOpen = false;
	close(FileWeld);
}

WeildLog::~WeildLog()
{


}
