#include "WeildLog.h"


struct stat stFile;
int size;
void WeildLog::Init(std::string path, uint16_t msizefile)
{
	PathFile = path;	
	FileIsOpen = false;
	maxsize = msizefile;
	olddata = ' ';
}
void WeildLog::WeildLogOpen() {
	FileWeld= open(PathFile.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
	FileIsOpen = true;
}
void WeildLog::WeildLogWrite(std::string WeildData, std::string FrameData)
{
	if (olddata != FrameData)
	{
		olddata = FrameData;
		if (!FileIsOpen) {
			WeildLogOpen();
		}

		stat(PathFile.c_str(), &stFile);
		size = stFile.st_size / 1048576;

		if (size < maxsize)
		{
			write(FileWeld, WeildData.c_str(), WeildData.size());
		}
	}
}

void WeildLog::WeildLogClose()
{
	FileIsOpen = false;
	close(FileWeld);
}

WeildLog::~WeildLog()
{


}
