// DXTCompressor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileLoader.h"

int main()
{
	FileLoader* FileLoader = FileLoader::GetInstance();
	FileLoader->LoadFile("DDSTest123.dds");
	FileLoader->DeleteInstance();
	system("PAUSE");
    return 0;
}

