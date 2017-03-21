#include "stdafx.h"
#include "BCCompression.h"


void BCCompression::ColorSpace(PIXEL*& Color , PIXEL*& Pixel)
{
	PIXEL RefColor;
	PIXEL LowColor(255,255,255);
	PIXEL HiColor;
	
	unsigned int LowDistance = LowColor.Distance(RefColor);
	unsigned int HiDistance = HiColor.Distance(RefColor);

	for (int i = 0; i < TEXEL_SIZE; i++)
	{
		unsigned int Dist = Pixel[i].Distance(RefColor);

		if (Dist < LowDistance)
		{
			LowColor = Pixel[i];
			LowDistance = Dist;
		}

		if (Dist > HiDistance)
		{
			HiColor = Pixel[i];
			HiDistance = Dist;
		}
	}

	PIXEL MaxColor;
	PIXEL MinColor;

	if (LowColor > HiColor)
	{
		MinColor = HiColor;
		MaxColor = LowColor;
	}
	else
	{
		MinColor = LowColor;
		MaxColor = HiColor;
	}

	Color[0] = MaxColor;
 	Color[1] = MinColor;

	
	if (Color[0] > Color[1])
	{
		Color[2] = Lerp(Color[1], Color[0]);
		Color[3] = Lerp(Color[0], Color[1]);
	}
	else
	{
		Color[2] = Blend(Color[1], Color[0]);
		Color[3].Black();
	}
}

PIXEL BCCompression::Lerp(PIXEL& c0, PIXEL& c1)
{

	uint8_t Red = (c0.bRed * 2 + c1.bRed) / 3;
	uint8_t Green = (c0.bGreen * 2 + c1.bGreen) / 3;
	uint8_t Blue = (c0.bBlue * 2 + c1.bBlue) / 3;

	PIXEL LerpColor(Red , Green, Blue);
	return LerpColor;
	
}

PIXEL BCCompression::Blend(PIXEL& c0, PIXEL& c1)
{
	uint8_t Red = (c0.bRed + c1.bRed) / 2;
	uint8_t Green = (c0.bGreen + c1.bGreen) / 2;
	uint8_t Blue = (c0.bBlue + c1.bBlue) / 2;

	PIXEL BlendColor(Red, Green, Blue);
	return BlendColor;
}

TEXEL BCCompression::EncodeBC1(PIXEL* Pixel)
{
	TEXEL NewTexel;

	if (Pixel != nullptr)
	{
		PIXEL* Colors = new PIXEL[4];

		ColorSpace(Colors, Pixel);
		
		NewTexel.rgb565_1 = Colors[0];
		NewTexel.rgb565_2 = Colors[1];


		for (int i = 0; i < TEXEL_SIZE; i++)
		{
			uint8_t mask = (uint8_t)GetClosestPixel(Colors,COLOURS_SIZE,Pixel[i]);
			int _idx = ((i + 1) * 2) - 1;

			NewTexel.Colors ^= (-((mask >> 0) & 1) ^ NewTexel.Colors) & ((1 << _idx) - 1);
			NewTexel.Colors ^= (-((mask >> 1) & 1) ^ NewTexel.Colors) & (1 << _idx);
		}

	}
	return NewTexel;
}

unsigned int BCCompression::GetClosestPixel(PIXEL* Colors,const int Size, PIXEL targetColor)
{
	PIXEL ClosestColor(0, 0, 0);
	unsigned int ClosestDist = ClosestColor.Distance(PIXEL(255,255,255));
	unsigned int idx = 0;

	for (int i = 0; i < Size; i++)
	{
		unsigned int Dist = Colors[i].Distance(targetColor);
		if (Dist <= ClosestDist)
		{
			ClosestDist = Dist;
			idx = i;
		}
	}
	return idx;
}

PIXEL* BCCompression::DecodeBC1(TEXEL Texel)
{
	PIXEL Colors[4];

	Colors[0] = Texel.rgb565_1.ToRGB();
	Colors[1] = Texel.rgb565_2.ToRGB();
	
	if (Colors[0] > Colors[1])
	{
		Colors[2] = Lerp(Colors[0], Colors[1]);
		Colors[3] = Lerp(Colors[1], Colors[0]);
	}
	else
	{
		Colors[2] = Blend(Colors[0], Colors[1]);
		Colors[3].Black();
	}

	PIXEL* pixels = new PIXEL[TEXEL_SIZE];
	for (int i = 0; i < TEXEL_SIZE ; i++)
	{
		int ret = 0;
		int _idx = ((i + 1) * 2) - 1; 

		int x = (Texel.Colors >> (_idx - 1)) & 1;
		ret ^= (-x ^ ret) & (1 << 0);
		x = (Texel.Colors >> _idx) & 1;
		ret ^= (-x ^ ret) & (1 << 1);

		pixels[i] = Colors[ret];
	}
	return pixels;
}