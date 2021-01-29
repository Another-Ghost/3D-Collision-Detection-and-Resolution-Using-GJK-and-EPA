/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "TextureWriter.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "./stb/stb_image_write.h"
#include "Assets.h"
using namespace NCL;

void TextureWriter::WritePNG(const std::string& filename, char* data, int width, int height, int channels) {
	stbi_write_png(filename.c_str(), width, height, channels, data, width * channels);
}
