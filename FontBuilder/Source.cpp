#include <iostream>
#include <fstream>
#include <string>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../Common/stb/stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../Common/stb/stb_image_write.h"

using std::string;

int main(void) {

	string inFont	= "PressStart2P.ttf";
	string outTex	= "PressStart2P.png";
	string outData	= "PressStart2P.fnt";

	int xSize = 512;
	int ySize = 512;

	unsigned char* bitmapData	= new unsigned char[xSize * ySize];

	unsigned char* fontData		= new unsigned char[1 << 20];
	fread(fontData, 1, 1 << 20, fopen(inFont.c_str(), "rb"));

	int startChar	= 32;
	int numChars	= 96;

	stbtt_bakedchar cdata[96];

	stbtt_BakeFontBitmap(fontData, 0, 48.0, bitmapData, xSize, ySize, startChar, numChars, cdata);

	stbi_write_png(outTex.c_str(), xSize, ySize, 1, bitmapData, xSize * 1);

	delete fontData;
	delete bitmapData;


	std::ofstream fntFile(outData, std::ios::out);

	fntFile << xSize	 << std::endl;
	fntFile << ySize	 << std::endl;
	fntFile << startChar << std::endl;
	fntFile << numChars  << std::endl;

	for (int i = 0; i < 96; ++i) {
		fntFile << cdata[i].x0		<< " "
				<< cdata[i].y0		<< " "
				<< cdata[i].x1		<< " "
				<< cdata[i].y1		<< " "
				<< cdata[i].xoff	<< " "
				<< cdata[i].yoff	<< " "
				<< cdata[i].xadvance
				<< std::endl;
	}

	return 0;
}
