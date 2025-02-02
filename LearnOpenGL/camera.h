#pragma once

#include <glm/glm.hpp>

#include "config.h"

class Camera {
   public:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	float speed = 2.5f;

	Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up);

	glm::mat4 calculate_view_matrix() const;

	glm::mat4 calculate_projection_matrix() const;

	void move(const glm::vec2& input_direction, float delta_time);

	void update_look_direction(float mouse_x, float mouse_y);

	void change_zoom(float scroll_y_offset);

   private:
	float m_yaw = -90.0f;
	float m_pitch = 0.0f;
	float m_last_mouse_x = constants::WINDOW_WIDTH / 2.0f;
	float m_last_mouse_y = constants::WINDOW_HEIGHT / 2.0f;
	bool m_mouse_first_entered = true;
	float m_fov = 45.0f;
};