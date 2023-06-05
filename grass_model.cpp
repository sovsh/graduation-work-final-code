#include "grass_model.h"

void GrassModel::generate_vertices_order()
{
	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution<int> stem_generator(this->stem_min, this->stem_max);

	float y = 0.0f;

	for (float x = position_x_min; x <= position_x_max; x += 1.5f)
	{
		for (float z = position_z_min; z <= position_z_max; z += 1.5f)
		{
			int stem = stem_generator(generator);
			this->vertices.push_back(glm::vec4(x, y, z, stem));
		}
	}
	
}


void GrassModel::generate_vertices_random()
{
	const float offset_min = -0.35f;
	const float offset_max = 0.35f;

	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_real_distribution<float> offset_generator(offset_min, offset_max);
	std::uniform_int_distribution<int> stem_generator(this->stem_min, this->stem_max);

	float y = 0.0f;

	for (float x = position_x_min; x <= position_x_max; x += 1.5f)
	{
		for (float z = position_z_min; z <= position_z_max; z += 1.5f)
		{
			int stem = stem_generator(generator);
			float offset_x = offset_generator(generator);
			float offset_z = offset_generator(generator);

			glm::vec3 position(x + offset_x, y, z + offset_z);

			if (position.x > this->position_x_min && position.x < this->position_x_max && position.z > this->position_z_min && position.z < this->position_z_max)
				this->vertices.push_back(glm::vec4(position.x, position.y, position.z, stem));
		}
	}
}

void GrassModel::load_texture(std::string texture_file)
{
	if (!this->image.loadFromFile(texture_file))
	{
		std::cout << "Error: Model::load_texture" << std::endl;
		return;
	}
	image.flipVertically();
}

GrassModel::GrassModel() {}

GrassModel::GrassModel(std::string texture_file, int count, float position_x_min, float position_x_max, float position_z_min, float position_z_max, int stem_min, int stem_max)
{
	this->count = count; 
	this->position_x_min = position_x_min; 
	this->position_x_max = position_x_max; 
	this->position_z_min = position_z_min; 
	this->position_z_max = position_z_max; 
	this->stem_min = stem_min; 
	this->stem_max = stem_max;

	this->load_texture(texture_file);
	//this->generate_vertices_order();
	this->generate_vertices_random();
}
