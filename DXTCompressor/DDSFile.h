#pragma once
#include "FileLoader.h"
#include "BMPFile.h"

#define DDS_MAGIC				542327876 
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

	DXT* DDSHeader;

	void ReadTexleData(std::ifstream& stream);

	void ReadTexel(std::istream_iterator<uint8_t>& it, int datasize, uint8_t* out);

public:
	
	DXT* GetDDSHeader() { return DDSHeader; };
	void SetDDSHeader(DXT* Header) { DDSHeader = Header; };

	DDSFile();

	virtual void WriteFile(std::ostream& outstream);

	void SaveFile(const char* DDSfile);

	void DecodeBC1();
	~DDSFile();
};
