#include "stdafx.h"
#include "DDSFile.h"


DDSFile::DDSFile()
{	
	DDSHeader = new DXT();

	FileLoader* Fl = FileLoader::GetInstance();

	Fl->Stream.read((char*)&DDSHeader->Header, sizeof(DDSHeader->Header));
	
	if (DDSHeader->Header.dwSize == 0)
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
		ReadTexleData(Fl->Stream);
		//if (DDSHeader->Texel != nullptr)
		//{
			DecodeBC1();
		//}
	}
}

void DDSFile::ReadTexleData(std::ifstream& stream)
{  
	if (DDSHeader->dwMagic != DDS_MAGIC) return ;

	stream.seekg(0, std::ios::end);
	int length = stream.tellg();
	stream.seekg(0, std::ios::beg);

	if (DDSHeader->Header.dwSize != DDS_HEADER_SIZE || DDSHeader->Header.ddspf.dwSize != DDS_PIXELFORMAT_SIZE) return;

	int dataLength = length - DDS_HEADER_SIZE - sizeof(DDSHeader->dwMagic);
	DDSHeader->TexelLenght = dataLength / sizeof(TEXEL);
	DDSHeader->Texel = new TEXEL[DDSHeader->TexelLenght];

	unsigned char* data = new unsigned char[2];
	std::istream_iterator<uint8_t> it;
	for (int i = 0; i < DDSHeader->TexelLenght; i++)
	{
		//TODO:: Fix Texelreading
		uint8_t rgb[2];
		ReadTexel(it, sizeof(rgb), rgb);
		DDSHeader->Texel[i].rgb565_1.Value = *(uint16_t*)rgb;

		ReadTexel(it, sizeof(rgb), rgb);
		DDSHeader->Texel[i].rgb565_2.Value = *(uint16_t*)rgb;

		uint8_t colors[4];
		ReadTexel(it, sizeof(colors), colors);
		DDSHeader->Texel[i].Colors = *(uint32_t*)colors;
	}
}

void DDSFile::ReadTexel(std::istream_iterator<uint8_t>& it, int datasize, uint8_t* out)
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

void DDSFile::DecodeBC1()
{
	BMPFile* BMP = new BMPFile();

	BITMAPFILEHEADER BmpHeader;
	BITMAPINFOHEADER BmpInfo;

	BmpHeader.bfType = (unsigned int)("B");
	BmpHeader.bfSize;
	BmpHeader.bfReserved1 = 0;
	BmpHeader.bfReserved2 = 0;
	BmpHeader.bfOffBits = 54;

	BmpInfo.biSize = 40;          
	BmpInfo.biHeight = DDSHeader->Header.dwHeight;
	BmpInfo.biWidth = DDSHeader->Header.dwWidth;
	BmpInfo.biPlanes = 1;        
	BmpInfo.biBitCount = 24;      
	BmpInfo.biCompression = BI_RGB;   
	BmpInfo.biSizeImage = DDSHeader->Header.dwWidth * DDSHeader->Header.dwHeight * (DDSHeader->Header.dwDepth / sizeof(uint8_t));;   
	BmpInfo.biXPelsPerMeter;
	BmpInfo.biYPelsPerMeter;  
	BmpInfo.biClrUsed = 0;       
	BmpInfo.biClrImportant=0;   

	//BMP->PixelData = new PIXELDATA(BmpInfo.biHeight, BmpInfo.biWidth);

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
			PIXEL* ps = BCCompression::DecodeBC1(DDSHeader->Texel[t_idx++]);
			int ps_idx = 0;

			for (int k = 0; k < TEXEL_WIDTH; k++)
			{
				for (int l = 0; l < TEXEL_WIDTH; l++)
				{
					x = (j * TEXEL_WIDTH) + l;

					// Write pixel data from bottom to top
					y = DDSHeader->Header.dwHeight - (((i * TEXEL_WIDTH) + k) + 1);
					//BMP->PixelData->pixel[i][j] = ps[ps_idx++];
				}
			}
		}
	}

}

void DDSFile::SaveFile(const char* DDSfile)
{
	std::ofstream outstream;
	outstream.open(DDSfile, std::ios::out | std::ios::binary);
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
		DDSHeader->Texel[i].rgb565_1.ToRGB().Print();
		DDSHeader->Texel[i].rgb565_2.ToRGB().Print();

		outstream.write((char*)&(DDSHeader->Texel[i].rgb565_1.Value), sizeof(uint16_t));
		outstream.write((char*)&(DDSHeader->Texel[i].rgb565_1.Value), sizeof(uint16_t));

		outstream.write((char*)&(DDSHeader->Texel[i].Colors), sizeof(uint32_t));
	}
	
}

DDSFile::~DDSFile()
{
}
