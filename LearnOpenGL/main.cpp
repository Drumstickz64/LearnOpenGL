#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "config.h"
#include "fs_util.h"
#include "model.h"
#include "shader_program.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static auto camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

static void glfw_error_callback(int error_code, const char* description) {
	std::cerr << "ERROR::GLFW\n"
			  << "ERROR CODE: " << error_code << "\n"
			  << "DESCRIPTION: " << description << "\n"
			  << std::endl;
}

static void GLAPIENTRY opengl_message_callback(GLenum source,
											   GLenum type,
											   GLuint id,
											   GLenum severity,
											   GLsizei length,
											   const GLchar* message,
											   const void* userParam) {
	std::cerr << "MESSAGE::OPENGL" << std::endl;
	std::cerr << "SOURCE: " << source << std::endl;
	std::cerr << "TYPE: " << type << std::endl;
	std::cerr << "ID: " << id << std::endl;
	std::cerr << "SEVERITY: " << severity << std::endl;
	std::cerr << "MESSAGE: " << message << std::endl;
}

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
	if (!glfwInit()) {
		std::cerr << "failed to initialize GLFW" << std::endl;
		return -1;
	}

	if constexpr (constants::DEBUG) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	} else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window =
		glfwCreateWindow(constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "failed to create GLFW window" << std::endl;
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

	if constexpr (constants::DEBUG) {
		glfwSetErrorCallback(glfw_error_callback);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);	// Ensures callback messages are synchronized
		glDebugMessageCallback(opengl_message_callback, nullptr);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glViewport(0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

#pragma region shader
	std::string object_vertex_shader = fs_util::read_file(constants::SHADER_PATH / "object.vert");
	std::string object_fragment_shader = fs_util::read_file(constants::SHADER_PATH / "object.frag");
	std::string solid_color_fragment_shader = fs_util::read_file(constants::SHADER_PATH / "solid_color.frag");
	Shader_Program object_shader = Shader_Program(object_vertex_shader, object_fragment_shader);
	Shader_Program solid_color_shader = Shader_Program(object_vertex_shader, solid_color_fragment_shader);
#pragma endregion

#pragma region textures
	Texture cube_texture = Texture(constants::ASSET_PATH / "marble.jpg", "texture_diffuse");
	Texture floor_texture = Texture(constants::ASSET_PATH / "metal.png", "texture_diffuse");
#pragma endregion

#pragma region static_data
	float cube_vertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,	-0.5f, -0.5f, 1.0f, 0.0f, 0.5f,	 0.5f,	-0.5f, 1.0f, 1.0f,
		0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f,	 0.0f, 0.0f, 0.5f,	-0.5f, 0.5f,  1.0f, 0.0f, 0.5f,	 0.5f,	0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

		-0.5f, 0.5f,  0.5f,	 1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,	0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,	 1.0f, 0.0f, 0.5f,	0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,	 -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,	-0.5f, 0.5f,  0.0f, 0.0f, 0.5f,	 0.5f,	0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,	-0.5f, -0.5f, 1.0f, 1.0f, 0.5f,	 -0.5f, 0.5f,  1.0f, 0.0f,
		0.5f,  -0.5f, 0.5f,	 1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,	0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,	 0.5f,	0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,	 1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,	-0.5f, 0.0f, 1.0f};

	float plane_vertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture
		// wrapping mode). this will cause the floor texture to repeat)
		5.0f, -0.5f, 5.0f, 2.0f, 0.0f, -5.0f, -0.5f, 5.0f,	0.0f, 0.0f, -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

		5.0f, -0.5f, 5.0f, 2.0f, 0.0f, -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, 5.0f,  -0.5f, -5.0f, 2.0f, 2.0f};

	// cube VAO
	unsigned int cube_vao, cube_vbo;
	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);
	glBindVertexArray(cube_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// plane _vao
	unsigned int plane_vao, plane_vbo;
	glGenVertexArrays(1, &plane_vao);
	glGenBuffers(1, &plane_vbo);
	glBindVertexArray(plane_vao);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	object_shader.use();
#pragma endregion

#pragma region loop
	float last_frame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		float time = static_cast<float>(glfwGetTime());
		float delta_time = time - last_frame;
		last_frame = time;

		process_input(window, delta_time);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 view = camera.calculate_view_matrix();
		glm::mat4 projection = camera.calculate_projection_matrix();

		object_shader.use();
		object_shader.set_mat4("view", view);
		object_shader.set_mat4("projection", projection);

		solid_color_shader.use();
		solid_color_shader.set_mat4("view", view);
		solid_color_shader.set_mat4("projection", projection);

		object_shader.use();
		// floor
		glStencilMask(0x00);
		glBindVertexArray(plane_vao);
		object_shader.set_texture("texture1", floor_texture, 0);
		object_shader.set_mat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cubes
		object_shader.set_texture("texture1", cube_texture, 0);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		// cube 1
		glBindVertexArray(cube_vao);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		object_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// cube 2
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		object_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		solid_color_shader.use();
		// cube 1 border
		glBindVertexArray(cube_vao);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(1.1f));
		solid_color_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// cube 2 border
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.1f));
		solid_color_shader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region shutdown
	glfwTerminate();
#pragma endregion

	return 0;
}
