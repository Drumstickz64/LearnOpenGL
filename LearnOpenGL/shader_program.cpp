#include <string_view>
#include <cstdlib>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"

Shader_Program::Shader_Program(std::string_view vertex_source, std::string_view fragment_source) {
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_source_c = vertex_source.data();
	const char* fragment_source_c = fragment_source.data();

	glShaderSource(vertex_shader, 1, &vertex_source_c, nullptr);
	glCompileShader(vertex_shader);
	GLint vertex_shader_compile_success;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compile_success);
	if (!vertex_shader_compile_success) {
		GLchar info_log[512];
		glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
		exit(-1);
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source_c, nullptr);
	glCompileShader(fragment_shader);
	GLint fragment_shader_compile_success;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_success);
	if (!fragment_shader_compile_success) {
		GLchar info_log[512];
		glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
		exit(-1);
	}

	id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	glLinkProgram(id);
	GLint program_link_success;
	glGetProgramiv(id, GL_LINK_STATUS, &program_link_success);
	if (!program_link_success) {
		GLchar info_log[512];
		glGetProgramInfoLog(id, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::CREATION_FAILED\n" << info_log << std::endl;
		exit(-1);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader_Program::use() const {
	glUseProgram(id);
}

void Shader_Program::set_bool(std::string_view name, bool value) const {
	GLint location = get_uniform_location(name);
	glUniform1i(location, (GLint)value);
}

void Shader_Program::set_int(std::string_view name, GLint value) const {
	GLint location = get_uniform_location(name);
	glUniform1i(location, value);
}

void Shader_Program::set_float(std::string_view name, GLfloat value) const {
	GLint location = get_uniform_location(name);
	glUniform1f(location, value);
}

void Shader_Program::set_vec3(std::string_view name, const glm::vec3& value) const {
	GLint location = get_uniform_location(name);
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader_Program::set_mat4(std::string_view name, const glm::mat4& value) const {
	GLint location = get_uniform_location(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader_Program::set_texture(std::string_view name, const Texture& value, GLenum slot) const {
	value.bind(slot);
	set_int(name, slot);
}

GLint Shader_Program::get_uniform_location(std::string_view name) const
{
	GLint location = glGetUniformLocation(id, name.data());
	if (location == -1) {
		std::cerr << "ERROR::SHADER\n" << "uniform '" << name << "' does not exist";
		exit(-1);
	}

	return location;
}
