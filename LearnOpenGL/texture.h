#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

struct Texture {
	int width = 0;
	int height = 0;
	int num_chans = 0;

	GLuint id = 0;
	std::string type;

	Texture() = default;
	Texture(std::filesystem::path image_path,
					 std::string type,
					 GLenum wrap_s = GL_REPEAT,
					 GLenum wrap_t = GL_REPEAT);
	void bind(GLenum slot) const;
};

struct Cubemap {
	GLuint id = 0;

	Cubemap() = default;
	Cubemap(const std::vector<std::filesystem::path>& image_paths);
	void bind(GLenum slot) const;
};
