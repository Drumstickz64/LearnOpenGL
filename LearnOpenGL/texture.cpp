#include <iostream>
#include <unordered_map>

#include "texture.h"

static std::unordered_map<std::filesystem::path, Texture> textures_loaded;

Texture::Texture(std::filesystem::path image_path, std::string type) : type(type) {
	if (textures_loaded.contains(image_path)) {
		*this = textures_loaded[image_path];
		return;
	}

	unsigned char* image_data = stbi_load(image_path.string().c_str(), &width, &height, &num_chans, 0);
	if (!image_data) {
		std::cerr << "ERROR::TEXTURE\n" << "failed to load image '" << image_path << "'" << std::endl;
		return;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format;
	if (num_chans == 1) {
		format = GL_RED;
	} else if (num_chans == 3) {
		format = GL_RGB;
	} else if (num_chans == 4) {
		format = GL_RGBA;
	} else {
		std::cerr << "WARNING: unsupported number of channels (" << num_chans << ") for image '" << image_path
				  << "', falling back to RGB" << std::endl;
		format = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	textures_loaded[image_path] = *this;

	stbi_image_free(image_data);
}

void Texture::bind(GLenum slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
	glActiveTexture(GL_TEXTURE0);
}
