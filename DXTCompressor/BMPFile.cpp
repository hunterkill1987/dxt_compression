#include "stdafx.h"
#include "BMPFile.h"


BMPFile::BMPFile()
{
	FileLoader::GetInstance()->Stream.unsetf(std::ios::skipws);
	if (ReadHeader(FileLoader::GetInstance()->Stream))
	{
		if (PixelData != nullptr)
		{
			EncodeBC1();
		}

	}
	else
	{
		Header.bfType = 0x4d42;
		Header.bfReserved1 = 0;
		Header.bfReserved2 = 0;
		Header.bfOffBits = 0x36;

		BitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
		BitmapInfo.biPlanes = 1;
		BitmapInfo.biBitCount = 24;
		BitmapInfo.biCompression = BI_RGB;
		
		BitmapInfo.biXPelsPerMeter = 0x0ec4;
		BitmapInfo.biYPelsPerMeter = 0x0ec4;
		BitmapInfo.biClrUsed = 0;
		BitmapInfo.biClrImportant = 0;
	}
}

bool BMPFile::ReadHeader(std::ifstream& stream)
{
	std::istream_iterator<uint8_t> it(stream);

	FileLoader::ReadData(it, Header.bfSize);
	FileLoader::ReadData(it, Header.bfType);
	FileLoader::ReadData(it, Header.bfReserved1);
	FileLoader::ReadData(it, Header.bfReserved2);
	FileLoader::ReadData(it, Header.bfOffBits);

	return ReadBitmapInfo(it);
}

bool BMPFile::ReadBitmapInfo(std::istream_iterator<uint8_t> it)
{
	FileLoader::ReadData(it, BitmapInfo.biSize);
	FileLoader::ReadData(it, BitmapInfo.biWidth);
	FileLoader::ReadData(it, BitmapInfo.biHeight);
	FileLoader::ReadData(it, BitmapInfo.biPlanes);
	FileLoader::ReadData(it, BitmapInfo.biBitCount);
	FileLoader::ReadData(it, BitmapInfo.biCompression);
	FileLoader::ReadData(it, BitmapInfo.biSizeImage);
	FileLoader::ReadData(it, BitmapInfo.biXPelsPerMeter);
	FileLoader::ReadData(it, BitmapInfo.biYPelsPerMeter);
	FileLoader::ReadData(it, BitmapInfo.biClrUsed);
	FileLoader::ReadData(it, BitmapInfo.biClrImportant);

	if (BitmapInfo.biWidth >= 0 && BitmapInfo.biHeight >= 0)
	{
		PixelData = new PIXELDATA(BitmapInfo.biHeight, BitmapInfo.biWidth);
		ReadArrayPixel(it);
		return true;
	}
	return false;
}
void BMPFile::SaveFile()
{
	std::ofstream outstream;
	outstream.open(FileLoader::GetFileName(), std::ios::out | std::ios::binary | std::ios::trunc);
	//outstream.seekp(0, std::ofstream::beg);
	outstream.unsetf(std::ios::skipws);

	if (outstream.good() && PixelData != nullptr)
	{
		WriteHeader(outstream);
		//WritePixelArray(outstream);
		outstream.close();
	}
}

void BMPFile::WriteHeader(std::ofstream& outstream)
{

	outstream.write((char*)(&Header.bfType),		sizeof(Header.bfType));
	outstream.write((char*)(&Header.bfSize),		sizeof(Header.bfSize));
	outstream.write((char*)(&Header.bfReserved1),	sizeof(Header.bfReserved1));
	outstream.write((char*)(&Header.bfReserved2),	sizeof(Header.bfReserved2));
	outstream.write((char*)(&Header.bfOffBits),		sizeof(Header.bfOffBits));

	outstream.write((char*)&BitmapInfo, sizeof(BITMAPINFOHEADER));

	int count = 0;
	
	for (int i = 0; i < BitmapInfo.biHeight; i++)
	{
		for (int j = 0; j < BitmapInfo.biWidth; j++)
		{
			outstream.write((char*)&(PixelData->pixel[i][j].bRed), sizeof(uint8_t));
			outstream.write((char*)&(PixelData->pixel[i][j].bGreen), sizeof(uint8_t));
			outstream.write((char*)&(PixelData->pixel[i][j].bBlue), sizeof(uint8_t));
			count++;
		}
	}

}

void BMPFile::ReadArrayPixel(std::istream_iterator<uint8_t> it)
{
	uint8_t data[3];
	for (int i = 0; i < BitmapInfo.biHeight; i++)
	{
		for (int j = 0; j < BitmapInfo.biWidth; j++)
		{
			std::istream_iterator<uint8_t> eos;
			for (int i = 0; i < sizeof(data); i++)
			{
				if (it != eos)
				{
					data[i] = *it;
					++it;
				}
			}
			PixelData->pixel[i][j].bRed = (uint8_t)data[0];
			PixelData->pixel[i][j].bGreen = (uint8_t)data[1];
			PixelData->pixel[i][j].bBlue = (uint8_t)data[2];
		}
	}
}

void BMPFile::EncodeBC1()
{
	DDSFile* DDS = new DDSFile();

	DXT* Dxt = DDS->DDSHeader;

	PIXEL* sqr = new PIXEL[TEXEL_WIDTH * TEXEL_WIDTH];

	int TexelWidth = BitmapInfo.biWidth / TEXEL_WIDTH;
	int TexelHeigth = BitmapInfo.biHeight / TEXEL_WIDTH;


	Dxt->Header.dwHeight = BitmapInfo.biHeight;
	Dxt->Header.dwWidth = BitmapInfo.biWidth;
	Dxt->Header.dwPitchOrLinearSize = std::max(1, ((BitmapInfo.biWidth + 3) / 4)) * 8;
	Dxt->TexelLenght = TexelWidth * TexelHeigth;

	Dxt->Texel = new TEXEL[Dxt->TexelLenght];

	int texIdx = 0 , x = 0, y = 0;
	
	for (int i = 0; i < TexelHeigth; i++)
	{
		for (int j = 0; j < TexelWidth; j++)
		{
			int idx = 0;
			for (int k = 0; k < TEXEL_WIDTH; k++)
			{
				for (int l = 0; l < TEXEL_WIDTH; l++)
				{
					x = (j * TEXEL_WIDTH) + k;

					y = BitmapInfo.biHeight - (((i * TEXEL_WIDTH) + l) + 1);

					sqr[idx] = PixelData->pixel[y][x];
					++idx;
				}
			}

			Dxt->Texel[texIdx] = BCCompression::EncodeBC1(sqr);
			++texIdx;
		}
	}

	DDS->DDSHeader = Dxt;

	DDS->SaveFile();
	delete DDS;
}

BMPFile::~BMPFile()
{
	delete PixelData;
}
