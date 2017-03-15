#pragma once

#include "DataTypas.h"
#include <stdlib.h>
#include <cmath>

#define TEXEL_SIZE 16
#define TEXEL_WIDTH 4
#define COLOURS_SIZE 4

#define _MODE_565_RGB16BIT(r,g,b) ((b%32) + ((g%64) << 6) + ((r%32) << 11))

class BCCompression
{

private:

	static void ColorSpace(PIXEL*& Color, PIXEL*& Pixel);

	//static void Lerp(uint8_t c0, uint8_t c1, uint8_t& c2, uint8_t& c3);

	static PIXEL Lerp(PIXEL& c0, PIXEL& c1);
	static PIXEL Blend(PIXEL& c0, PIXEL& c1);

	static unsigned int GetClosestPixel(PIXEL* Colors, const int Size, PIXEL targetColor);

public:

	static TEXEL EncodeBC1(PIXEL* Pixel);
	static PIXEL* DecodeBC1(TEXEL TEXEL);
};

