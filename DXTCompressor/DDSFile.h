#pragma once
#include "FileLoader.h"
#include "BMPFile.h"

#define DDS_MAGIC				0x20534444 
#define DDS_HEADER_SIZE			124
#define DDS_PIXELFORMAT_SIZE	32
#define DDSD_CAPS 				0x1
#define DDSD_PIXELFORMAT		0x1000
#define DDSCAPS_TEXTURE			0x1000
#define DDPF_FOURCC				0x4
#define DXT1_FOURCC				'1TXD'


class BMPFile;

class DDSFile 
{
private:

	bool ReadData(std::ifstream& stream);

	void ReadHeaderData(std::istream_iterator<uint8_t>& it, DDSHEADER& Header);

public:
	
	DXT* DDSHeader;

	DDSFile();

	virtual void WriteFile(std::ostream& outstream);

	void SaveFile();

	void DecodeBC1();
	~DDSFile();
};

