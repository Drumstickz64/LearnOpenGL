#pragma once

#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"

class Shader_Program {
public:
	// the program ID
	GLuint id;

	// constructor reads and builds the shader
	Shader_Program(std::string_view vertexSource, std::string_view fragmentSource);
	// use/activate the shader
	void use() const;
	// utility uniform functions
	void set_bool(std::string_view name, bool value) const;
	void set_int(std::string_view name, GLint value) const;
	void set_float(std::string_view name, GLfloat value) const;
	void set_vec3(std::string_view name, const glm::vec3& value) const;
	void set_mat4(std::string_view name, const glm::mat4& value) const;
	void set_texture(std::string_view name, const Texture& value, GLenum slot) const;

private:
	GLint get_uniform_location(std::string_view name) const;
};
