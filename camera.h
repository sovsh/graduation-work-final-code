#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Window.hpp>
#include <iostream>

class Camera
{
private:
	void update_front();
public:
	glm::vec3 global_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 position, front, up, right;
	float sensitivity, speed, yaw, pitch, last_x, last_y;

	Camera();
	Camera(glm::vec3 position, glm::vec3 front, float sensitivity, float speed, float yaw, float pitch, float last_x, float last_y);
	
	glm::mat4 get_view_matrix();
	void rotate(sf::Vector2i mouse_position);
	void rotate(float step);
	void move(float step);
};

