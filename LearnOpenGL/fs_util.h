#pragma once

#include <filesystem>
#include <string>

namespace fs_util {

std::string read_file(const std::filesystem::path& filename);

}