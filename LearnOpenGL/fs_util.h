#pragma once

#include <string>
#include <filesystem>

namespace fs_util {
	std::string read_file(const std::filesystem::path& filename);
}