#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

Camera::Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up) : pos(pos), front(front), up(up) {}

glm::mat4 Camera::calculate_view_matrix() const {
	return glm::lookAt(pos, pos + front, up);
}

glm::mat4 Camera::calculate_projection_matrix() const {
	return glm::perspective(glm::radians(m_fov),
							static_cast<float>(constants::WINDOW_WIDTH) / static_cast<float>(constants::WINDOW_HEIGHT),
							0.1f, 100.0f);
}

void Camera::move(const glm::vec2& input_direction, float delta_time) {
	pos += front * input_direction.y * speed * delta_time;
	glm::vec3 right_direction = glm::normalize(glm::cross(front, up));
	pos += right_direction * input_direction.x * speed * delta_time;
}

void Camera::update_look_direction(float mouse_x, float mouse_y) {
	if (m_mouse_first_entered) {
		m_last_mouse_x = mouse_x;
		m_last_mouse_y = mouse_y;
		m_mouse_first_entered = false;
	}

	float x_offset = mouse_x - m_last_mouse_x;
	float y_offset = m_last_mouse_y - mouse_y;	// reversed since y-coordinates range from bottom to top
	m_last_mouse_x = mouse_x;
	m_last_mouse_y = mouse_y;

	m_yaw += x_offset * constants::MOUSE_SENSETIVIY;
	m_pitch = glm::clamp(m_pitch + y_offset * constants::MOUSE_SENSETIVIY, -89.0f, 89.0f);

	auto direction = glm::vec3(cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)), sinf(glm::radians(m_pitch)),
							   sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)));

	front = glm::normalize(direction);
}

void Camera::change_zoom(float scroll_y_offset) {
	m_fov -= static_cast<float>(scroll_y_offset);
	m_fov = glm::clamp(m_fov, 1.0f, 45.0f);
}
