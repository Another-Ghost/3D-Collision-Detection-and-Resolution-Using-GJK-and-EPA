/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/TextureBase.h"
#include "glad\glad.h"

#include <string>

namespace NCL {
	namespace Rendering {
		class OGLTexture : public TextureBase
		{
		public:
			//friend class OGLRenderer;
			 OGLTexture();
			 OGLTexture(GLuint texToOwn);
			~OGLTexture();

			static TextureBase* RGBATextureFromData(char* data, int width, int height, int channels);

			static TextureBase* RGBATextureFromFilename(const std::string&name);

			GLuint GetObjectID() const	{
				return texID;
			}
		protected:						
			GLuint texID;
		};
	}
}

