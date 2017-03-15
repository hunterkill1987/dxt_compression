#include "stdafx.h"
#include "BMPFile.h"


BMPFile::BMPFile()
{
	FileLoader* Fl = FileLoader::GetInstance();

	Fl->Stream.seekg(0, std::ios::beg);

	Fl->Stream.read((char*)&Header, sizeof(Header));
	Fl->Stream.read((char*)&BitmapInfo, sizeof(BitmapInfo));

	if (Header.bfSize > 0)
	{
		PixelData = new PIXELDATA(BitmapInfo.biHeight, BitmapInfo.biWidth);

		ReadArrayPixel(Fl->Stream);

		if (PixelData != nullptr)
		{
			EncodeBC1();
		}

	}
}

void BMPFile::WriteFile()
{

}

void BMPFile::ReadArrayPixel(std::ifstream& stream)
{
	uint8_t data[3];
	std::istream_iterator<uint8_t> it(stream);

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

	DXT* Dxt = DDS->GetDDSHeader();

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

	DDS->SetDDSHeader(Dxt);

	DDS->SaveFile("DDSTest123.dds");
}

BMPFile::~BMPFile()
{
}
