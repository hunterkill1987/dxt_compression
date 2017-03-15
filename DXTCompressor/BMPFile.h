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

	BITMAPFILEHEADER Header;
	BITMAPINFOHEADER BitmapInfo;

	void EncodeBC1();
public:

	PIXELDATA* PixelData;

	void SetHeader(BITMAPFILEHEADER _Header) { Header = _Header; };
	void SetBitmapInfo(BITMAPINFOHEADER _Info) { BitmapInfo = _Info; };

	BMPFile();

	void ReadArrayPixel(std::ifstream& stream);

	void WriteFile();
	~BMPFile();
};

