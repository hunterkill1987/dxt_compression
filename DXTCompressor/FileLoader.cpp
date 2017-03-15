#include "stdafx.h"
#include "FileLoader.h"

FileLoader* FileLoader::Instance = 0;

FileLoader* FileLoader::GetInstance()
{
	if (Instance == 0)
	{
		Instance = new FileLoader();
	}
	return Instance;
}

void FileLoader::DeleteInstance()
{
	delete Instance;
	Instance = nullptr;
}

void FileLoader::LoadFile(const char* Filename)
{
	Stream.open(Filename,std::ios::binary);

	if (Stream.good())
	{
		//BMPFile* bmp = new BMPFile();
		DDSFile* dds = new DDSFile();
		//delete bmp;
	}
}
