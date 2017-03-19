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
	Stream.open(Filename, std::ios::binary);

	if (Stream.is_open())
	{
		if (Stream.good())
		{
			//BMPFile* bmp = new BMPFile();
			DDSFile* dds = new DDSFile();
			//delete bmp;
			Stream.close();
		}
	}
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
