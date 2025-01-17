#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "config.h"
#include "fs_util.h"
#include "shader_program.h"

constexpr int32_t WINDOW_WIDTH = 800;
constexpr int32_t WINDOW_HEIGHT = 600;

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	glViewport(0, 0, width, height);
}

int main() {
	stbi_set_flip_vertically_on_load(true);

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

	std::string vertex_shader = fs_util::read_file(paths::SHADER_PATH / "shader.vert");
	std::string fragment_shader = fs_util::read_file(paths::SHADER_PATH / "shader.frag");
	Shader_Program program = Shader_Program(vertex_shader, fragment_shader);

	std::filesystem::path container_image_path = paths::ASSET_PATH / "container.jpg";
	int container_image_width, container_image_height, container_image_num_chans;
	uint8_t* container_image_data = stbi_load(container_image_path.string().c_str(), &container_image_width, &container_image_height, &container_image_num_chans, 0);
	if (!container_image_data) {
		std::cout << "ERROR::FILE\n" << "failed to load container_image 'container.jpg'" << std::endl;
		exit(-1);
	}

	GLuint container_texture;
	glGenTextures(1, &container_texture);
	glBindTexture(GL_TEXTURE_2D, container_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, container_image_width, container_image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, container_image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(container_image_data);

	std::filesystem::path face_image_path = paths::ASSET_PATH / "awesomeface.png";
	int face_image_width, face_image_height, face_image_num_chans;
	uint8_t* face_image_data = stbi_load(face_image_path.string().c_str(), &face_image_width, &face_image_height, &face_image_num_chans, 0);
	if (!face_image_data) {
		std::cout << "ERROR::FILE\n" << "failed to load container_image 'awesomeface.jpg'" << std::endl;
		exit(-1);
	}

	GLuint face_texture;
	glGenTextures(1, &face_texture);
	glBindTexture(GL_TEXTURE_2D, face_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, face_image_width, face_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face_image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		program.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, face_texture);
		program.set_int("texture1", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, container_texture);
		program.set_int("texture1", 1);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
