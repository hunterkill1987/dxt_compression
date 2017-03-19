#pragma once
#include "FileLoader.h"
#include "BCCompression.h"
#include "DDSFile.h"

#define BI_RGB	0
#define FHEADERSIZE 14

class DDSfile;

class BMPFile
{

friend class FileLoader;

private:

	void EncodeBC1();

	bool ReadHeader(std::ifstream& stream);

	bool ReadBitmapInfo(std::istream_iterator<uint8_t> it);

	void WriteHeader(std::ofstream& outstream);

	void WritePixelArray(std::ofstream& outstream);

public:

	PIXELDATA* PixelData;

	BITMAPFILEHEADER Header;
	BITMAPINFOHEADER BitmapInfo;

	BMPFile();

	void ReadArrayPixel(std::istream_iterator<uint8_t> it);

	void SaveFile(const char* Filename);
	~BMPFile();
};

