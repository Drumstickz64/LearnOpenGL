#pragma once

#include <filesystem>

#include <GLFW/glfw3.h>
#include <stb_image.h>

class Texture {
public:
	stbi_uc* image_data = nullptr;
	int width = 0;
	int height = 0;
	int num_chans = 0;

	GLuint id = 0;

	Texture(std::filesystem::path image_path);
	void bind(GLenum slot) const;
};
