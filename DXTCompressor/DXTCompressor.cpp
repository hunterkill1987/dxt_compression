// DXTCompressor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileLoader.h"

int main(int argc, char* argv[])
{
	FileLoader* FileLoader = FileLoader::GetInstance();

	FileLoader->LoadFile(argv[1]);

	FileLoader->DeleteInstance();

    return 0;
}

