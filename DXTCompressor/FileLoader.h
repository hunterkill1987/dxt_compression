#pragma once

#include <iostream>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include <cassert>
#include <sstream>
#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <memory>
#include <algorithm>  
#include <iterator>

#include "DataTypas.h"
#include "BMPFile.h"
#include "DDSFile.h"

class FileLoader
{
private:
	FileLoader() {};
	~FileLoader() {};

public:

	std::ifstream Stream;

	static FileLoader* GetInstance();

	static FileLoader* Instance;

	static void DeleteInstance();

	void LoadFile(const char* Filename);

	static void ReadData(std::istream_iterator<uint8_t>& it, int datasize, uint8_t* out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint32_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint16_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, uint8_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, int32_t& out);

	static void ReadData(std::istream_iterator<uint8_t>& it, int16_t& out);

};

