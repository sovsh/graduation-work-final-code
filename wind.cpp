#include "wind.h"

Wind::Wind() {}

Wind::Wind(float speed_min, float speed_max, float turbulence_min, float turbulence_max) 
{ 
	this->direction = glm::vec3(1.0f, 0.0f, 0.0f); this->speed = 3000.0f; this->turbulence = 0.3f;
	this->speed_min = speed_min; this->speed_max = speed_max; this->turbulence_min = turbulence_min; this->turbulence_max = turbulence_max; 
}

void Wind::change_speed(float value) 
{ 
	float val = this->speed + value;
	if (val > this->speed_min && val < this->speed_max)
		this->speed = val;
}

void Wind::change_turbulence(float value) 
{ 
	float val = this->turbulence + value;
	if (val > this->turbulence_min && val < this->turbulence_max)
		this->turbulence = val;
}

void Wind::change_direction(glm::vec3 value) 
{
	if (value.x == 1.0f)
	{
		if (this->direction.x == 1.0f)
		{
			this->direction.x = -1.0f;
			return;
		}
		this->direction.x += 1.0f;
		return;
	}	

	else if (value.y == 1.0f)
	{
		if (this->direction.y == 1.0f)
		{
			this->direction.y = -1.0f;
			return;
		}
		this->direction.y += 1.0f;
		return;
	}

	if (this->direction.z == 1.0f)
	{
		this->direction.z = -1.0f;
		return;
	}
	this->direction.z += 1.0f;
	return;

}
