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

	glViewport(0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

#pragma region shader
	std::string object_vertex_shader = fs_util::read_file(constants::SHADER_PATH / "object.vert");
	std::string object_fragment_shader = fs_util::read_file(constants::SHADER_PATH / "object.frag");
	Shader_Program object_shader = Shader_Program(object_vertex_shader, object_fragment_shader);
#pragma endregion

#pragma region model
	stbi_set_flip_vertically_on_load(true);
	auto backpack_model = Model(constants::ASSET_PATH / "backpack_model" / "backpack.obj");
#pragma endregion

#pragma region loop
	float last_frame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		float time = static_cast<float>(glfwGetTime());
		float delta_time = time - last_frame;
		last_frame = time;

		process_input(window, delta_time);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		object_shader.use();

		glm::mat4 view = camera.calculate_view_matrix();
		object_shader.set_mat4("view", view);
		glm::mat4 projection = camera.calculate_projection_matrix();
		object_shader.set_mat4("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));	 // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	 // it's a bit too big for our scene, so scale it down
		object_shader.set_mat4("model", model);
		backpack_model.draw(object_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region shutdown
	glfwTerminate();
#pragma endregion

	return 0;
}
