#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "fs_util.h"
#include "shader_program.h"
#include "camera.h"

static auto camera = Camera(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)
);

static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow* window, float delta_time) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	auto input_direction = glm::vec2(0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		input_direction.y += 1.0f;
	}
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		input_direction.y -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		input_direction.x -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		input_direction.x += 1.0f;
	}

	if (glm::length(input_direction) > 1.0f) {
		input_direction = glm::normalize(input_direction);
	}

	camera.move(input_direction, delta_time);
}

static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	camera.update_look_direction(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	camera.change_zoom(static_cast<float>(y_offset));
}

int main() {
#pragma region init
	stbi_set_flip_vertically_on_load(true);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

#pragma region shader
	std::string vertex_shader = fs_util::read_file(constants::SHADER_PATH / "shader.vert");
	std::string fragment_shader = fs_util::read_file(constants::SHADER_PATH / "shader.frag");
	Shader_Program program = Shader_Program(vertex_shader, fragment_shader);
#pragma endregion

#pragma region textures
	std::filesystem::path container_image_path = constants::ASSET_PATH / "container.jpg";
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

	std::filesystem::path face_image_path = constants::ASSET_PATH / "awesomeface.png";
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
#pragma endregion

#pragma region static_data
	float vertices[] = {
		  // vertex position      texture coordinate
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cube_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
#pragma endregion

#pragma region loop
	float last_frame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		float time = static_cast<float>(glfwGetTime());
		float delta_time = time - last_frame;
		last_frame = time;

		process_input(window, delta_time);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.use();

		glBindVertexArray(vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, face_texture);
		program.set_int("texture1", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, container_texture);
		program.set_int("texture1", 1);
		
		glm::mat4 view = camera.calculate_view_matrix();
		program.set_mat4("view", view);

		glm::mat4 projection = camera.calculate_projection_matrix();
		program.set_mat4("projection", projection);

		for (size_t i = 0; i < 10; i++) {
			auto model = glm::mat4(1.0f);
			model = glm::translate(model, cube_positions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
			program.set_mat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region shutdown
	glfwTerminate();
#pragma endregion

	return 0;
}
