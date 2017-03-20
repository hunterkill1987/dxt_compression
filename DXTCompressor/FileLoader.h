#pragma once

#include <iostream>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include <cassert>
#include <sstream>
#include <iostream>

#include <string>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <algorithm>  
#include <iterator>

#include "DataTypas.h"


class BMPFile;
class DDSFile;

enum EFileType
{
	UNKNOW = 0,
	DDS = 1,
	BMP = 2
};

class FileLoader
{
private:
	FileLoader() {};
	~FileLoader();

	EFileType GetFileExt(std::string& str);

	BMPFile* ImgBMP;

	DDSFile* ImgDDS;

	static char* Filename;

public:

	std::ifstream Stream;

	void LoadFile(char* InFilename);

	static FileLoader* GetInstance();

	static FileLoader* Instance;

	static char* GetFileName();

	static void DeleteInstance();

	static void ReadData(std::istream_iterator<uint8_t>& it, int datasize, uint8_t* out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint32_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint16_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint8_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, int32_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, int16_t& out);

};

