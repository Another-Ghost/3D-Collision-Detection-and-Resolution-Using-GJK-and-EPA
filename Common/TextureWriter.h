/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <string>

namespace NCL {
	class TextureWriter
	{
	public:
		static void WritePNG(const std::string& filename, char* data, int width, int height, int channels);
	};
}

