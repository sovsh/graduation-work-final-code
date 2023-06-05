#pragma once

#include <glm/glm.hpp>

class Wind
{
public:
	glm::vec3 direction;
	float speed, turbulence;
	float speed_min, speed_max, turbulence_min, turbulence_max = 0.0f;

	Wind();
	Wind(float speed_min, float speed_max, float turbulence_min, float turbulence_max);

	void change_speed(float value);
	void change_turbulence(float value);
	void change_direction(glm::vec3 value);
};

