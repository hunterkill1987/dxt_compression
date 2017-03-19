#pragma once
#include "FileLoader.h"
#include "BCCompression.h"
#include "DDSFile.h"

#define BI_RGB	0

class DDSfile;

class BMPFile
{

friend class FileLoader;

private:

	void EncodeBC1();

	bool ReadHeader(std::ifstream& stream);

	bool ReadBitmapInfo(std::istream_iterator<uint8_t> it);

public:

	PIXELDATA* PixelData;

	BITMAPFILEHEADER Header;
	BITMAPINFOHEADER BitmapInfo;

	BMPFile();

	void ReadArrayPixel(std::istream_iterator<uint8_t> it);

	void WriteFile();
	~BMPFile();
};

