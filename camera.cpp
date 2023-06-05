#include "camera.h"

void Camera::update_front()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
}

Camera::Camera() {}

Camera::Camera(glm::vec3 position, glm::vec3 front, float sensitivity, float speed, float yaw, float pitch, float last_x, float last_y)
{
	this->position = position; this->front = front;
	this->sensitivity = sensitivity; this->speed = speed;
	this->yaw = yaw; this->pitch = pitch; this->last_x = last_x; this->last_y = last_y;
}

glm::mat4 Camera::get_view_matrix()
{
	return glm::lookAt(this->position, this->position + this->front, this->global_up);
}

void Camera::rotate(sf::Vector2i mouse_position)
{
    int x_pos = mouse_position.x;
    int y_pos = mouse_position.y;

    float xoffset = x_pos - last_x;
    float yoffset = last_y - y_pos;
    last_x = x_pos;
    last_y = y_pos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    update_front();
}

void Camera::rotate(float step)
{
    step *= sensitivity;

    yaw += step;
    update_front();
}

void Camera::move(float step)
{
    this->position += step * this->speed * this->front;
}
