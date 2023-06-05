#pragma once

#include <glm/glm.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>

class GrassModel
{
private:
	void generate_vertices_order();
	void generate_vertices_random();

	void load_texture(std::string texture_file);
public:
	sf::Image image;
	std::vector<glm::vec4>vertices;

	int count;
	float position_x_min, position_x_max, position_z_min, position_z_max, stem_min, stem_max;

	GrassModel();
	GrassModel(std::string texture_file, int count, float position_x_min, float position_x_max, float position_z_min, float position_z_max, int stem_min, int stem_max);
};

