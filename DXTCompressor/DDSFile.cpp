#include "stdafx.h"
#include "DDSFile.h"


DDSFile::DDSFile()
{	
	DDSHeader = new DXT();
	FileLoader::GetInstance()->Stream.unsetf(std::ios::skipws);
	if (!ReadData(FileLoader::GetInstance()->Stream))
	{
		DDSHeader->dwMagic = DDS_MAGIC;

		DDSHeader->Header.dwSize = DDS_HEADER_SIZE;
		DDSHeader->Header.dwFlags = DDSD_PIXELFORMAT | DDSD_CAPS;

		DDSHeader->Header.dwDepth = 0;
		DDSHeader->Header.dwMipMapCount = 0;
		DDSHeader->Header.dwReserved1[11];
		for (int i = 0; i < 11; i++)
		{
			DDSHeader->Header.dwReserved1[i] = 0;
		}

		DDSHeader->Header.ddspf.dwSize = DDS_PIXELFORMAT_SIZE;
		DDSHeader->Header.ddspf.dwFlags = DDPF_FOURCC;
		DDSHeader->Header.ddspf.dwFourCC = DXT1_FOURCC;
		DDSHeader->Header.ddspf.dwRGBBitCount = 0;
		DDSHeader->Header.ddspf.dwRBitMask = 0;
		DDSHeader->Header.ddspf.dwGBitMask = 0;
		DDSHeader->Header.ddspf.dwBBitMask = 0;
		DDSHeader->Header.ddspf.dwABitMask = 0;

		DDSHeader->Header.dwCaps1 = DDSCAPS_TEXTURE;
		DDSHeader->Header.dwCaps2 = 0;
		DDSHeader->Header.dwReserved2[3];
		for (int i = 0; i < 3; i++)
		{
			DDSHeader->Header.dwReserved1[i] = 0;
		}
	}
	else
	{
		DecodeBC1();
	}
}

void DDSFile::ReadHeaderData(std::istream_iterator<uint8_t>& it, DDSHEADER& Header)
{
	int size = 0;

	FileLoader::ReadData(it,Header.dwSize);
	FileLoader::ReadData(it,Header.dwFlags);
	FileLoader::ReadData(it,Header.dwHeight);
	FileLoader::ReadData(it,Header.dwWidth);
	FileLoader::ReadData(it,Header.dwPitchOrLinearSize);
	FileLoader::ReadData(it,Header.dwDepth);
	FileLoader::ReadData(it,Header.dwMipMapCount);

	size = sizeof(Header.dwReserved1) / sizeof(uint32_t);
	for (int i = 0; i < size; i++)
	{
		FileLoader::ReadData(it,Header.dwReserved1[i]);
	}

	FileLoader::ReadData(it, Header.ddspf.dwSize);
	FileLoader::ReadData(it, Header.ddspf.dwFlags);
	FileLoader::ReadData(it, Header.ddspf.dwFourCC);
	FileLoader::ReadData(it, Header.ddspf.dwRGBBitCount);
	FileLoader::ReadData(it, Header.ddspf.dwRBitMask);
	FileLoader::ReadData(it, Header.ddspf.dwGBitMask);
	FileLoader::ReadData(it, Header.ddspf.dwBBitMask);
	FileLoader::ReadData(it, Header.ddspf.dwABitMask);

	FileLoader::ReadData(it,Header.dwCaps1);
	FileLoader::ReadData(it,Header.dwCaps2);

	size = sizeof(Header.dwReserved2) / sizeof(uint32_t);
	for (int i = 0; i < size; i++)
	{
		FileLoader::ReadData(it, Header.dwReserved2[i]);
	}
}

bool DDSFile::ReadData(std::ifstream& stream)
{  
	stream.seekg(0, std::ios::end);
	int length = (int)stream.tellg();
	stream.seekg(0, std::ios::beg);

	if (length < DDS_HEADER_SIZE) return false;

	std::istream_iterator<uint8_t> it(stream);

	FileLoader::ReadData(it, DDSHeader->dwMagic);

	if (DDSHeader->dwMagic != DDS_MAGIC) return false;

	ReadHeaderData(it, DDSHeader->Header);

	if (DDSHeader->Header.dwSize != DDS_HEADER_SIZE ) return false;

	if (DDSHeader->Header.dwSize != DDS_HEADER_SIZE || DDSHeader->Header.ddspf.dwSize != DDS_PIXELFORMAT_SIZE) return false;

	int dataLength = length - DDS_HEADER_SIZE - sizeof(DDSHeader->dwMagic);
	DDSHeader->TexelLenght = dataLength / sizeof(TEXEL);
	DDSHeader->Texel = new TEXEL[DDSHeader->TexelLenght];

	for (int i = 0; i < DDSHeader->TexelLenght; i++)
	{
		FileLoader::ReadData(it, DDSHeader->Texel[i].rgb565_1.Value);
		FileLoader::ReadData(it, DDSHeader->Texel[i].rgb565_2.Value);
		FileLoader::ReadData(it, DDSHeader->Texel[i].Colors);

	}
	return true;
}

void DDSFile::DecodeBC1()
{
	BMPFile* BMP = new BMPFile();

	uint32_t HeaderSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	BMP->Header.bfSize = FHEADERSIZE + sizeof(BITMAPINFOHEADER) + sizeof(PIXEL) * DDSHeader->Header.dwWidth * DDSHeader->Header.dwHeight;//((DDSHeader->Header.dwHeight *  DDSHeader->Header.dwHeight) * 3) + HeaderSize;
	BMP->Header.bfSize = sizeof(BMP->Header) + sizeof(PIXEL);

	BMP->BitmapInfo.biHeight = DDSHeader->Header.dwHeight;
	BMP->BitmapInfo.biWidth = DDSHeader->Header.dwWidth;

	BMP->BitmapInfo.biSizeImage = DDSHeader->Header.dwWidth * DDSHeader->Header.dwHeight * 3;// (DDSHeader->Header.dwDepth / sizeof(uint8_t));

	BMP->PixelData = new PIXELDATA(BMP->BitmapInfo.biHeight, BMP->BitmapInfo.biWidth);

	int texelWidth = DDSHeader->Header.dwHeight / TEXEL_WIDTH;
	int texelHeight = DDSHeader->Header.dwWidth / TEXEL_WIDTH;

	if (DDSHeader->TexelLenght != texelHeight * texelWidth)
		return ;

	int t_idx = 0;
	int x = 0;
	int y = 0;

	for (int i = 0; i < texelHeight; i++)
	{
		for (int j = 0; j < texelWidth; j++)
		{
			PIXEL* ps = BCCompression::DecodeBC1(DDSHeader->Texel[t_idx]);

			int ps_idx = 0;

			t_idx++;

			for (int k = 0; k < TEXEL_WIDTH; k++)
			{
				for (int l = 0; l < TEXEL_WIDTH; l++)
				{
					x = DDSHeader->Header.dwWidth - ((j * TEXEL_WIDTH) + l);

					y = DDSHeader->Header.dwHeight - (((i * TEXEL_WIDTH) + k) + 1);

					BMP->PixelData->pixel[y][x] = ps[ps_idx];

					ps_idx++;
				}
			}
		}
	}

	BMP->SaveFile();
	BMP = nullptr;
	delete BMP;
}

void DDSFile::SaveFile()
{
	std::ofstream outstream;
	outstream.open(FileLoader::GetFileName(), std::ios::out | std::ios::binary);
	outstream.unsetf(std::ios::skipws);

	if (outstream.good() && DDSHeader != nullptr)
	{
		WriteFile(outstream);
		outstream.close();
	}

}

void DDSFile::WriteFile(std::ostream& outstream)
{
	outstream.write((char*)&DDSHeader->dwMagic, sizeof(uint32_t));

	outstream.write((char*)&(DDSHeader->Header.dwSize), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.dwFlags), sizeof(uint32_t));

	outstream.write((char*)&(DDSHeader->Header.dwHeight), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.dwWidth), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.dwPitchOrLinearSize), sizeof(uint32_t));

	outstream.write((char*)&(DDSHeader->Header.dwDepth), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.dwMipMapCount), sizeof(uint32_t));

	int size = sizeof(DDSHeader->Header.dwReserved1) / sizeof(uint32_t);
	for (int i = 0; i < size; i++)
	{
		outstream.write((char*)&(DDSHeader->Header.dwReserved1[i]), sizeof(uint32_t));
	}
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwSize), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwFlags), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwFourCC), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwRGBBitCount), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwRBitMask), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwGBitMask), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwBBitMask), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.ddspf.dwABitMask), sizeof(uint32_t));

	outstream.write((char*)&(DDSHeader->Header.dwCaps1), sizeof(uint32_t));
	outstream.write((char*)&(DDSHeader->Header.dwCaps2), sizeof(uint32_t));
	size = sizeof(DDSHeader->Header.dwReserved2) / sizeof(uint32_t);

	for (int i = 0; i < 3; i++)
	{
		outstream.write((char*)&(DDSHeader->Header.dwReserved2[i]), sizeof(uint32_t));
	}
	
	
	for (int i = 0; i < DDSHeader->TexelLenght; i++)
	{
		outstream.write((char*)&(DDSHeader->Texel[i].rgb565_1.Value), sizeof(uint16_t));
		outstream.write((char*)&(DDSHeader->Texel[i].rgb565_2.Value), sizeof(uint16_t));

		outstream.write((char*)&(DDSHeader->Texel[i].Colors), sizeof(uint32_t));
	}
	
}

DDSFile::~DDSFile()
{
	delete[] DDSHeader->Texel;
	delete DDSHeader;
}
