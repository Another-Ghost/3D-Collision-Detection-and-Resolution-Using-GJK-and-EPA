/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <string>
#include <vector>
#include "TextureBase.h"

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;
		class Vector4;
	}
	namespace Rendering {
		class SimpleFont
		{
		public:
			SimpleFont(const std::string&fontName, const std::string&texName);
			~SimpleFont();

			int BuildVerticesForString(std::string &text, Maths::Vector2&startPos, Maths::Vector4&colour, float size, std::vector<Maths::Vector3>&positions, std::vector<Maths::Vector2>&texCoords, std::vector<Maths::Vector4>&colours);

			const TextureBase* GetTexture() const {
				return texture;
			}

		protected:
			//matches stbtt_bakedchar
			struct FontChar {
				unsigned short x0;
				unsigned short y0;
				unsigned short x1;
				unsigned short y1;
				float xOff;
				float yOff;
				float xAdvance;
			};

			FontChar*		allCharData;
			TextureBase*	texture;

			int startChar;
			int numChars;

			float texWidth;
			float texHeight;
			float texWidthRecip;
			float texHeightRecip;
		};
	}
}

