#pragma once
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include <cstdlib>
#include <cstdio>

#include <cmath>

//------------------------------------------------------------------------
//BITMAP
#pragma  pack(2)
typedef struct                       /**** BMP file header structure ****/
{
	uint16_t	bfType;           /* Magic number for file */
	uint32_t	bfSize;           /* Size of file */
	uint16_t	bfReserved1;      /* Reserved */
	uint16_t	bfReserved2;      /* ... */
	uint16_t	bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;
#pragma pack()

typedef struct                       /**** BMP file info structure ****/
{
	uint32_t		biSize;           /* Size of info header */
	int32_t			biWidth;          /* Width of image */
	int32_t			biHeight;         /* Height of image */
	uint16_t		biPlanes;         /* Number of color planes */
	uint16_t		biBitCount;       /* Number of bits per pixel */
	uint32_t		biCompression;    /* Type of compression to use */
	uint32_t		biSizeImage;      /* Size of image data */
	int32_t			biXPelsPerMeter;  /* X pixels per meter */
	int32_t         biYPelsPerMeter;  /* Y pixels per meter */
	uint32_t		biClrUsed;        /* Number of colors used */
	uint32_t		biClrImportant;   /* Number of important colors */
} BITMAPINFOHEADER;

typedef struct PIXEL
{
	uint8_t bRed = 0x0;
	uint8_t bGreen = 0x0;
	uint8_t bBlue = 0x0;

	void Black()
	{
		bRed = 0x0;
		bGreen = 0x0;
		bBlue = 0x0;
	}

	PIXEL() :bRed(0), bGreen(0), bBlue(){}

	PIXEL(unsigned int Red, unsigned int Green, unsigned int Blue) : bRed(0), bGreen(0), bBlue()
	{
		bRed = Red;
		bGreen = Green;
		bBlue = Blue;
	}

	void Print()
	{
		std::cout <<" R: "<< (int)bRed << " G: " << (int)bGreen << " B: " << (int)bBlue << std::endl;
	}
	bool operator > (const PIXEL& Pixle)
	{
		return (Pixle.bRed > bRed || Pixle.bGreen > bGreen || Pixle.bBlue > bBlue) ? true : false;
	}

	bool operator <= (const PIXEL& Pixle)
	{
		return (Pixle.bRed <= bRed || Pixle.bGreen <= bGreen || Pixle.bBlue <= bBlue) ? true : false;
	}

	unsigned int Distance(PIXEL& Pixel2)
	{
		return (unsigned int) sqrt(abs(pow(bRed - Pixel2.bRed, 2) + pow(bGreen - Pixel2.bGreen, 2) + pow(bBlue - Pixel2.bBlue, 2)));
	}

}PIXEL;

typedef struct PIXELDATA
{
private:
	int height;
	int wight;

public:

	PIXELDATA(int Height, int Width): 
		height(0), wight(0)
	{
		this->height = Height;
		this->wight = Width;
		this->pixel = new PIXEL*[Height];

		for (int i = 0; i < Height; ++i)
		{
			this->pixel[i] = new PIXEL[Width];
		}
	}
	~PIXELDATA()
	{
		if (pixel != nullptr)
		{
			for (int i = 0; i < height; i++)
			{
				delete[] pixel[i];
				pixel[i] = nullptr;
			}
			delete[] pixel;
			pixel = nullptr;
		}
	};

	PIXEL** pixel = nullptr; //2D Array

}PIXELDATA;

//BITMAP
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//DDS
typedef struct DDSPIXELFORMAT {
	uint32_t dwSize				= 0x0;
	uint32_t dwFlags			= 0x0;
	uint32_t dwFourCC			= 0x0;
	uint32_t dwRGBBitCount		= 0x0;
	uint32_t dwRBitMask			= 0x0;
	uint32_t dwGBitMask			= 0x0;
	uint32_t dwBBitMask			= 0x0;
	uint32_t dwABitMask			= 0x0;
}DDSPIXELFORMAT;


typedef struct
{
	uint32_t dwSize					= 0x0;
	uint32_t dwFlags				= 0x0;
	uint32_t dwHeight				= 0x0;
	uint32_t dwWidth				= 0x0;
	uint32_t dwPitchOrLinearSize	= 0x0;
	uint32_t dwDepth				= 0x0;
	uint32_t dwMipMapCount			= 0x0;
	uint32_t dwReserved1[11];
	DDSPIXELFORMAT ddspf;
	uint32_t dwCaps1				= 0x0;
	uint32_t dwCaps2				= 0x0;
	uint32_t dwReserved2[3];

}DDSHEADER;

typedef struct COLOR565
{

public:

	uint16_t Value;

	COLOR565() :Value(0)
	{}

	COLOR565(uint16_t value) :Value(0)
	{
		Value = value;
	}

	COLOR565(PIXEL RGB) :Value(0)
	{
		Value = FromRGB(RGB).Value;
	}

	COLOR565(uint8_t R, uint8_t G, uint8_t B):Value(0)
	{
		Value = FromRGB(R,G,B).Value;
	}

	COLOR565 FromRGB(uint8_t R, uint8_t G, uint8_t B)
	{
		PIXEL Color;

		Color.bRed = R;
		Color.bBlue = B;
		Color.bGreen = G;
		
		return this->FromRGB(Color);
	}

	PIXEL ToRGB()
	{
		unsigned char b = (((Value) & 0x001F) << 3);
		unsigned char g = (((Value) & 0x07E0) >> 3);
		unsigned char r = (((Value) & 0xF800) >> 8);

		return PIXEL(r, g, b);
	}

	COLOR565 FromRGB(PIXEL& Pixel)
	{
		/*uint32_t red = (Pixel.bRed >> 16) & 0xFF;
		uint32_t green= (Pixel.bGreen >> 8) & 0xFF;
		uint32_t blue = Pixel.bBlue & 0xFF;

		uint8_t  B = (blue >> 3) & 0x001F;
		uint8_t  G = ((green >> 2) < 5) & 0x07E0;
		uint8_t  R = ((red >> 3) < 11) & 0xF800;
		
		COLOR565 Colors(R|G|B);
		*/
		COLOR565 Colors(((Pixel.bRed % 32) << 11) + ((Pixel.bGreen % 64) << 6) + (Pixel.bBlue % 32));

		return Colors;
	}
	
	COLOR565 & operator  = (PIXEL& Pixle)
	{
		Value = FromRGB(Pixle).Value;

		return *this;
	}

} COLOR565;

typedef struct
{
	COLOR565			rgb565_1	= 0x0;
	COLOR565			rgb565_2	= 0x0;
	uint32_t			Colors		= 0x0;

}TEXEL;

typedef struct
{
	uint32_t		dwMagic;
	DDSHEADER		Header;
	TEXEL*			Texel;
	int				TexelLenght;
}DXT;

//DDS
//------------------------------------------------------------------------

