#include "fs.h"

std::string read_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filename << std::endl;
        exit(-1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
