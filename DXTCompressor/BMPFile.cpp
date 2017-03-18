#include "stdafx.h"
#include "BMPFile.h"


BMPFile::BMPFile()
{
	if (ReadHeader(FileLoader::GetInstance()->Stream))
	{
		if (PixelData != nullptr)
		{
			EncodeBC1();
		}

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
void BMPFile::WriteFile()
{

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
			for (int l = 0; l < TEXEL_WIDTH; l++)
			{
				for (int k = 0; k < TEXEL_WIDTH; k++)
				{
					x = (j * TEXEL_WIDTH) + k;

					y = BitmapInfo.biHeight - (((i * TEXEL_WIDTH) + l) + 1);

					sqr[idx] = PixelData->pixel[y][x];

					//sqr[idx].Print();

					++idx;
				}
			}
			 
			Dxt->Texel[texIdx] = BCCompression::EncodeBC1(sqr);
			++texIdx;
		}
	}

	//DDS->SetDDSHeader(Dxt);
	DDS->DDSHeader = Dxt;

	DDS->SaveFile("DDSTest123.dds");
}

BMPFile::~BMPFile()
{
}
