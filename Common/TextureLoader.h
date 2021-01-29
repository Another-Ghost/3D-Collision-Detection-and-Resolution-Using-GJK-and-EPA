/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <map>
#include <functional>
#include <string>
using std::map;

#include "TextureBase.h"

namespace NCL {

	typedef std::function<bool(const std::string& filename, char*& outData, int& width, int &height, int &channels, int&flags)> TextureLoadFunction;

	typedef std::function<Rendering::TextureBase*(const std::string& filename)> APILoadFunction;

	class TextureLoader	{
	public:
		static bool LoadTexture(const std::string& filename, char*& outData, int& width, int &height, int &channels, int&flags);

		static void RegisterTextureLoadFunction(TextureLoadFunction f, const std::string&fileExtension);

		static void RegisterAPILoadFunction(APILoadFunction f);

		static Rendering::TextureBase* LoadAPITexture(const std::string&filename);
	protected:

		static std::string GetFileExtension(const std::string& fileExtension);

		static std::map<std::string, TextureLoadFunction> fileHandlers;

		static APILoadFunction apiFunction;
	};
}

