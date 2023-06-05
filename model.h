#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <gl/glew.h>
#include <SFML/Graphics.hpp>
#include "shader.h"

class Model
{
private:
	void load_obj(std::string obj_file);
	void load_texture(std::string texture_file);
	void get_extremums();
public:
	std::vector<glm::vec3> position_vertices;
	std::vector<glm::vec3> normal_vertices;
	std::vector<glm::vec2> texture_vertices;
	unsigned int vertices_count = 0;

	sf::Image image;

	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	float collision_radius = 0.0f;

	glm::vec3 mins;
	glm::vec3 maxes;

	Model();
	Model(std::string obj_file, std::string texture_file);
};

