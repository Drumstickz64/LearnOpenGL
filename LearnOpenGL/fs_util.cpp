#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "fs_util.h"

std::string fs_util::read_file(const std::filesystem::path& filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filename << std::endl;
		exit(-1);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
