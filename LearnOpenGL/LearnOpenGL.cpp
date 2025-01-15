// LearnOpenGL.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int32_t WINDOW_WIDTH = 800;
const int32_t WINDOW_HEIGHT = 600;
const size_t TRIANGLE_COUNT = 2;

const GLchar* VERTEX_SHADER_SOURCE = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const GLchar* FRAGMENT_SHADER_SOURCES[TRIANGLE_COUNT] = {
	// first shader
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0",
	// second shader
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
	"}\n\0",
};

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	glViewport(0, 0, width, height);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &VERTEX_SHADER_SOURCE, nullptr);
	glCompileShader(vertex_shader);
	GLint vertex_shader_compile_success;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compile_success);
	if (!vertex_shader_compile_success) {
		GLchar info_log[512];
		glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
		return -1;
	}

	GLuint fragment_shaders[TRIANGLE_COUNT];
	GLuint shader_programs[TRIANGLE_COUNT];

	for (size_t i = 0; i < TRIANGLE_COUNT; i++) {
		fragment_shaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
		GLuint fragment_shader = fragment_shaders[i];

		glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER_SOURCES[i], nullptr);
		glCompileShader(fragment_shader);
		GLint fragment_shader_compile_success;
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_success);
		if (!fragment_shader_compile_success) {
			GLchar info_log[512];
			glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
			return -1;
		}

		shader_programs[i] = glCreateProgram();
		GLuint shader_program = shader_programs[i];

		glAttachShader(shader_program, vertex_shader);
		glAttachShader(shader_program, fragment_shader);
		glLinkProgram(shader_program);
		GLint shader_program_link_success;
		glGetProgramiv(shader_program, GL_COMPILE_STATUS, &shader_program_link_success);
		if (!shader_program_link_success) {
			GLchar info_log[512];
			glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
			std::cout << "ERROR::SHADER::PROGRAM::CREATION_FAILED\n" << info_log << std::endl;
			return -1;
		}

		glDeleteShader(fragment_shader);
	}
	glDeleteShader(vertex_shader);


	float triangle_vertices[TRIANGLE_COUNT][9] = {
		{
			-0.5f, 0.0f, 0.0f,
			 0.5f, 0.0f, 0.0f,
			 0.0f, 1.0f, 0.0f,
		},

		{
			-0.25f, -0.1f, 0.0f,
			 0.25f, -0.1f, 0.0f,
			 0.0f,  -0.6f, 0.0f,
		},
	};

	GLuint vaos[TRIANGLE_COUNT];
	glGenVertexArrays(TRIANGLE_COUNT, vaos);
	GLuint vbos[TRIANGLE_COUNT];
	glGenBuffers(TRIANGLE_COUNT, vbos);

	for (size_t i = 0; i < TRIANGLE_COUNT; i++) {
		glBindVertexArray(vaos[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (size_t i = 0; i < TRIANGLE_COUNT; i++) {
			glUseProgram(shader_programs[i]);
			glBindVertexArray(vaos[i]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}

	glfwTerminate();
	return 0;
}
