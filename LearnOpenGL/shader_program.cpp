#include <string>
#include <cstdlib>
#include <iostream>
#include <cassert>

#include "Shader_Program.h"

Shader_Program::Shader_Program(const std::string& vertexSource, const std::string& fragmentSource) {
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_source_c = vertexSource.c_str();
	const char* fragment_source_c = fragmentSource.c_str();

	glShaderSource(vertex_shader, 1, &vertex_source_c, nullptr);
	const char* vertexSourceC = vertexSource.c_str();
	const char* fragmentSourceC = fragmentSource.c_str();

	glShaderSource(vertex_shader, 1, &vertexSourceC, nullptr);
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
	glShaderSource(fragment_shader, 1, &fragmentSourceC, nullptr);
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
	GLint id_link_success;
	glGetProgramiv(id, GL_COMPILE_STATUS, &id_link_success);
	if (!id_link_success) {
		GLchar info_log[512];
		glGetProgramInfoLog(id, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::CREATION_FAILED\n" << info_log << std::endl;
		exit(-1);
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader_Program::use() const {
void Shader_Program::use() {
	glUseProgram(id);
}

void Shader_Program::set_bool(const std::string& name, bool value) const {
	GLint location = glGetUniformLocation(id, name.c_str());
	assert(location != -1);
	glUniform1i(location, (GLint)value);
}

void Shader_Program::set_int(const std::string& name, GLint value) const {
	GLint location = glGetUniformLocation(id, name.c_str());
	assert(location != -1);
	glUniform1i(location, value);
}

void Shader_Program::set_float(const std::string& name, GLfloat value) const {
	GLint location = glGetUniformLocation(id, name.c_str());
	assert(location != -1);
	glUniform1f(location, value);
}
