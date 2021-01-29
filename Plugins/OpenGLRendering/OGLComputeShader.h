/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "glad\glad.h"

#include <string>

namespace NCL {
	class OGLComputeShader	{
	public:
		OGLComputeShader(const std::string& s);
		~OGLComputeShader();

		int GetProgramID() const {
			return programID;
		}

		void Bind() const;

		//how many thread groups should be launched?
		//number of threads within a group is determined shader side
		void Execute(int x, int y = 1, int z = 1) const;

		void GetThreadsInGroup(int&x, int& y, int&z) const;

		int GetThreadXCount() const;
		int GetThreadYCount() const;
		int GetThreadZCount() const;

		void Unbind();

	protected:
		GLuint	shaderID;
		GLuint	programID;
		int		programValid;
		GLint	threadsInGroup[3];
	};
}

