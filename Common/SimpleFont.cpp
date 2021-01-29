/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "SimpleFont.h"
#include "TextureLoader.h"
#include "Assets.h"
#include <fstream>

#include "../Common/Vector2.h"
#include "../Common/Vector3.h"
#include "../Common/Vector4.h"

using namespace NCL;
using namespace Rendering;
using namespace Maths;

SimpleFont::SimpleFont(const std::string&filename, const std::string&texName)
{
	startChar	= 0;
	numChars	= 0;
	allCharData	= nullptr;

	texture		= TextureLoader::LoadAPITexture(texName);

	std::ifstream fontFile(Assets::FONTSSDIR + filename);

	fontFile >> texWidth;
	fontFile >> texHeight;
	fontFile >> startChar;
	fontFile >> numChars;

	allCharData = new FontChar[numChars];

	for (int i = 0; i < numChars; ++i) {
		fontFile >> allCharData[i].x0;
		fontFile >> allCharData[i].y0;
		fontFile >> allCharData[i].x1;
		fontFile >> allCharData[i].y1;

		fontFile >> allCharData[i].xOff;
		fontFile >> allCharData[i].yOff;
		fontFile >> allCharData[i].xAdvance;
	}
	texWidthRecip	= 1.0f / texWidth;
	texHeightRecip	= 1.0f / texHeight;
}


SimpleFont::~SimpleFont()
{
	delete[]	allCharData;
	delete		texture;
}

int SimpleFont::BuildVerticesForString(std::string &text, Vector2&startPos, Vector4&colour, float size, std::vector<Vector3>&positions, std::vector<Vector2>&texCoords, std::vector<Vector4>&colours) {
	int vertsWritten = 0;

	int endChar = startChar + numChars;

	float currentX = 0.0f;

	positions.reserve(positions.size() + (text.length() * 6));
	colours.reserve(colours.size() + (text.length() * 6));
	texCoords.reserve(texCoords.size() + (text.length() * 6));

	for (size_t i = 0; i < text.length(); ++i) {
		int charIndex = (int)text[i];

		if (charIndex < startChar) {
			continue;
		}
		if (charIndex > endChar) {
			continue;
		}
		FontChar& charData = allCharData[charIndex - startChar];

		float scale = size;
		//For basic vertex buffers, we're assuming we should add 6 vertices

		float charWidth  = (float)((charData.x1 - charData.x0)/ texWidth) * scale;
		float charHeight = (float)(charData.y1 - charData.y0);

		float xStart	= ((charData.xOff + currentX) * texWidthRecip) * scale;
		float yStart	= startPos.y;
		float yHeight	= (charHeight * texHeightRecip) * scale;
		float yOff		= ((charHeight + charData.yOff) * texHeightRecip) * scale;

		positions.emplace_back(Vector3(startPos.x + xStart, yStart + yOff, 0));
		positions.emplace_back(Vector3(startPos.x + xStart, yStart + yOff - yHeight, 0));
		positions.emplace_back(Vector3(startPos.x + xStart + charWidth, yStart + yOff - yHeight, 0));

		positions.emplace_back(Vector3(startPos.x + xStart + charWidth, yStart + yOff - yHeight, 0));
		positions.emplace_back(Vector3(startPos.x + xStart + charWidth, yStart + yOff, 0));
		positions.emplace_back(Vector3(startPos.x + xStart, yStart + yOff, 0));

		colours.emplace_back(colour);
		colours.emplace_back(colour);
		colours.emplace_back(colour);

		colours.emplace_back(colour);
		colours.emplace_back(colour);
		colours.emplace_back(colour);

		texCoords.emplace_back(Vector2(charData.x0 * texWidthRecip, charData.y1 * texHeightRecip));
		texCoords.emplace_back(Vector2(charData.x0 * texWidthRecip, charData.y0 * texHeightRecip));
		texCoords.emplace_back(Vector2(charData.x1 * texWidthRecip, charData.y0 * texHeightRecip));

		texCoords.emplace_back(Vector2(charData.x1 * texWidthRecip, charData.y0 * texHeightRecip));
		texCoords.emplace_back(Vector2(charData.x1 * texWidthRecip, charData.y1 * texHeightRecip));
		texCoords.emplace_back(Vector2(charData.x0 * texWidthRecip, charData.y1 * texHeightRecip));

		currentX += charData.xAdvance;
	}

	return vertsWritten;
}