/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/ShaderBase.h"
#include "glad\glad.h"

namespace NCL {
	namespace Rendering {
		class OGLShader : public ShaderBase
		{
		public:
			friend class OGLRenderer;
			OGLShader(const string& vertex, const string& fragment, const string& geometry = "", const string& domain = "", const string& hull = "");
			~OGLShader();

			void ReloadShader() override;

			bool LoadSuccess() const {
				return programValid == GL_TRUE;
			}

			int GetProgramID() const {
				return programID;
			}	
			
			static void	PrintCompileLog(GLuint object);
			static void	PrintLinkLog(GLuint program);

		protected:
			void	DeleteIDs();

			GLuint	programID;
			GLuint	shaderIDs[(int)ShaderStages::SHADER_MAX];
			int		shaderValid[(int)ShaderStages::SHADER_MAX];
			int		programValid;
		};
	}
}