#include <iostream>
#include <cstdint>
#include <cmath>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.h"
#include "fs_util.h"
#include "shader_program.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static auto light_pos = glm::vec3(1.2f, 1.0f, 2.0f);

static auto camera = Camera(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)
);

static void glfw_error_callback(int error_code, const char* description) {
	std::cerr << "ERROR::GLFW\n"
		<< "ERROR CODE: " << error_code << "\n"
		<< "DESCRIPTION: " << description << "\n"
		<< std::endl;
}

static void GLAPIENTRY opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cerr << "ERROR::OPENGL" << std::endl;
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
	stbi_set_flip_vertically_on_load(true);

	if (!glfwInit()) {
		std::cerr << "failed to initialize GLFW" << std::endl;
		return -1;
	}

	if constexpr (constants::DEBUG) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	}
	else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
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
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Ensures callback messages are synchronized
		glDebugMessageCallback(opengl_error_callback, nullptr);
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma endregion

#pragma region shader
	std::string object_vertex_shader = fs_util::read_file(constants::SHADER_PATH / "object.vert");
	std::string object_fragment_shader = fs_util::read_file(constants::SHADER_PATH / "object.frag");
	Shader_Program object_shader = Shader_Program(object_vertex_shader, object_fragment_shader);

	std::string light_vertex_shader = fs_util::read_file(constants::SHADER_PATH / "light.vert");
	std::string light_fragment_shader = fs_util::read_file(constants::SHADER_PATH / "light.frag");
	Shader_Program light_shader = Shader_Program(light_vertex_shader, light_fragment_shader);
#pragma endregion

#pragma region textures
	auto diffuse_map_texture = Texture(constants::ASSET_PATH / "container2.png");
	auto specular_map_texture = Texture(constants::ASSET_PATH / "container2_specular.png");
#pragma endregion

#pragma region static_data
	float vertices[] = {
		// position           // normal            // texture coord
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
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

	GLuint object_vao;
	glGenVertexArrays(1, &object_vao);

	GLuint light_vao;
	glGenVertexArrays(1, &light_vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindVertexArray(object_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(light_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
#pragma endregion

#pragma region loop
	float last_frame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		float time = static_cast<float>(glfwGetTime());
		float delta_time = time - last_frame;
		last_frame = time;

		process_input(window, delta_time);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.calculate_view_matrix();
		glm::mat4 projection = camera.calculate_projection_matrix();

		object_shader.use();

		glm::mat4 model = glm::mat4(1.0f);
		object_shader.set_mat4("model", model);
		object_shader.set_mat4("view", view);
		object_shader.set_mat4("projection", projection);

		object_shader.set_vec3("viewPos", camera.pos);

		object_shader.set_texture("material.diffuse", diffuse_map_texture, 0);
		object_shader.set_texture("material.specular", specular_map_texture, 1);
		object_shader.set_float("material.shininess", 64.0f);

		object_shader.set_vec3("light.position", light_pos);
		object_shader.set_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		object_shader.set_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		object_shader.set_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		object_shader.set_float("light.constant", 1.0f);
		object_shader.set_float("light.linear", 0.09f);
		object_shader.set_float("light.quadratic", 0.032f);

		glBindVertexArray(object_vao);
		for (size_t i = 0; i < 10; i++) {
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			object_shader.set_mat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		light_shader.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.2f));

		light_shader.set_mat4("model", model);
		light_shader.set_mat4("view", view);
		light_shader.set_mat4("projection", projection);

		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region shutdown
	glfwTerminate();
#pragma endregion

	return 0;
}
