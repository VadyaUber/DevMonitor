#include "DevLog.h"


struct stat stFile;
int size;
void DevLog::Init(std::string path, uint16_t msizefile)
{
	PathFile = path;	
	FileIsOpen = false;
	maxsize = msizefile;
	olddata = ' ';
}
void DevLog::DevLogOpen() {
	if (access(PathFile.c_str(), 0)) //if a file exist
	{
		FileWeld = open(PathFile.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
		chmod(PathFile.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	else
		FileWeld = open(PathFile.c_str(), O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
	FileIsOpen = true;
}
void DevLog::DevLogWrite(std::string WeildData, std::string FrameData)
{
	if (olddata != FrameData)
	{
		olddata = FrameData;
		if (!FileIsOpen) {
			DevLogOpen();
		}

		stat(PathFile.c_str(), &stFile);
		size = stFile.st_size / 1048576;

		if (size < maxsize)
		{
			write(FileWeld, WeildData.c_str(), WeildData.size());
		}
	}
}

void DevLog::DevLogClose()
{
	FileIsOpen = false;
	close(FileWeld);
}

DevLog::~DevLog()
{


}
