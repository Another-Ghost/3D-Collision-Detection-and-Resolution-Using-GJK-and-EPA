#include "Assets.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace NCL;

bool Assets::ReadTextFile(const std::string &filepath, std::string& result) {
	std::ifstream file(filepath, std::ios::in);
	if (file) {
		std::ostringstream stream;

		stream << file.rdbuf();

		result = stream.str();

		return true;
	}
	else {
		std::cout << __FUNCTION__ << " can't read file " << filepath << std::endl;
		return false;
	}
}

bool	Assets::ReadBinaryFile(const std::string& filename, char** into, size_t& size) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		return false;
	}

	file.seekg(0, std::ios_base::end);

	std::streamoff filesize = file.tellg();

	file.seekg(0, std::ios_base::beg);

	char* data = new char[(unsigned int)filesize];

	file.read(data, filesize);

	file.close();

	*into = data;
	size = filesize;

	return data == NULL ? true : false;
}