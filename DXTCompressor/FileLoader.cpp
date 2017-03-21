#include "stdafx.h"
#include "FileLoader.h"
#include "BMPFile.h"
#include "DDSFile.h"

FileLoader* FileLoader::Instance = 0;

char* FileLoader::Filename = 0;

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

FileLoader::~FileLoader()
{
	if (ImgBMP != nullptr)
		delete ImgBMP;

	if (ImgDDS != nullptr)
		delete ImgDDS;
}

EFileType FileLoader::GetFileExt(std::string& str)
{
	size_t i = str.rfind('.',str.length());
	if (i != std::string::npos)
	{
		std::string Ext = str.substr(i + 1, str.length() - i);
		str.erase(i , str.length() - i);
		if (Ext.length() > 0)
		{
			if (Ext.compare("bmp") == 0)
			{
				str.append("D.dds");
				Filename = const_cast<char * >(str.c_str());
				return BMP;
			}

			if (Ext.compare("dds") == 0)
			{
				str.append("B.bmp");
				Filename = const_cast<char * >(str.c_str());
				return DDS;
			}
		}
	}
	return UNKNOW;
}

void FileLoader::LoadFile(char* InFilename)
{
	if (InFilename != nullptr)
	{
		Stream.open(InFilename, std::ios::binary);

		std::string str(InFilename);

		if (Stream.is_open())
		{
			if (Stream.good())
			{
				switch (GetFileExt(str))
				{
				case BMP:
					ImgBMP = new BMPFile();
					break;
				case DDS:
					ImgDDS = new DDSFile();
					break;
				case UNKNOW:
					std::cout << "Unknow file type" << std::endl;
					break;
				}

				Stream.close();
			}
		}
	}
	return;
}

char* FileLoader::GetFileName()
{
	return Filename;
}

void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, uint8_t& out)
{
	uint8_t data[1];

	ReadData(it, sizeof(data), data);

	out = *(uint8_t*)data;
}
void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, uint16_t& out)
{
	uint8_t data[2];

	ReadData(it, sizeof(data), data);

	out = *(uint16_t*)data;
}

void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, uint32_t& out)
{
	uint8_t data[4];

	ReadData(it, sizeof(data), data);

	out = *(uint32_t*)data;
}

void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, int16_t& out)
{
	uint16_t a;
	ReadData(it, a);

	out = (int16_t)a;
}

void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, int32_t& out)
{
	uint32_t a;
	ReadData(it, a);

	out = (int32_t)a;
}


void FileLoader::ReadData(std::istream_iterator<uint8_t>& it, int datasize, uint8_t* out)
{
	std::istream_iterator<uint8_t> eos;
	for (int i = 0; i < datasize; i++)
	{
		if (it != eos)
		{
			out[i] = *it;
			++it;
		}
	}
}
