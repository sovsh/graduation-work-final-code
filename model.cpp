#define _CRT_SECURE_NO_DEPRECATE

#include "model.h"

void Model::load_obj(std::string obj_file)
{
    FILE* file = fopen(obj_file.c_str(), "r");
    if (file == NULL) {
        std::cout << "Error: Impossible to open the file" << obj_file << " \n";
        return;
    }

    std::vector<glm::vec3> temp_position_vertices;
    std::vector<glm::vec3> temp_normal_vertices;
    std::vector<glm::vec2> temp_texture_vertices;

    std::vector<unsigned int>position_indices, normal_indices, texture_indices;

    while (1) {

        char line_header[128];
        int res = fscanf(file, "%s", line_header);
        if (res == EOF)
            break;

        else
        {
            if (strcmp(line_header, "v") == 0) 
            {
                glm::vec3 position_vertex;
                fscanf(file, "%f %f %f\n", &position_vertex.x, &position_vertex.y, &position_vertex.z);
                temp_position_vertices.push_back(position_vertex);
            }
            else if (strcmp(line_header, "vn") == 0) 
            {
                glm::vec3 normal_vertex;
                fscanf(file, "%f %f %f\n", &normal_vertex.x, &normal_vertex.y, &normal_vertex.z);
                temp_normal_vertices.push_back(normal_vertex);
            }
            else if (strcmp(line_header, "vt") == 0) 
            {
                glm::vec2 texture_vertex;
                fscanf(file, "%f %f\n", &texture_vertex.x, &texture_vertex.y);
                temp_texture_vertices.push_back(texture_vertex);
            }
            else if (strcmp(line_header, "f") == 0) 
            {
                unsigned int position_index[3], texture_index[3], normal_index[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &position_index[0], &texture_index[0], &normal_index[0], &position_index[1], &texture_index[1], &normal_index[1], &position_index[2], &texture_index[2], &normal_index[2]);
                if (matches == 9) {
                    position_indices.push_back(position_index[0]);
                    position_indices.push_back(position_index[1]);
                    position_indices.push_back(position_index[2]);
                    texture_indices.push_back(texture_index[0]);
                    texture_indices.push_back(texture_index[1]);
                    texture_indices.push_back(texture_index[2]);
                    normal_indices.push_back(normal_index[0]);
                    normal_indices.push_back(normal_index[1]);
                    normal_indices.push_back(normal_index[2]);
                }
            }
        }
    }

    for (unsigned int i = 0; i < position_indices.size(); i++) {
        unsigned int position_index = position_indices[i];
        glm::vec3 position_vertex = temp_position_vertices[position_index - 1];
        this->position_vertices.push_back(position_vertex);

        unsigned int norm_coord_index = normal_indices[i];
        glm::vec3 normal_vertex = temp_normal_vertices[norm_coord_index - 1];
        this->normal_vertices.push_back(normal_vertex);

        unsigned int texture_index = texture_indices[i];
        glm::vec2 texture_vertex = temp_texture_vertices[texture_index - 1];
        this->texture_vertices.push_back(texture_vertex);
    }

    this->vertices_count = this->position_vertices.size();
}

void Model::load_texture(std::string texture_file)
{
    if (!this->image.loadFromFile(texture_file))
    {
        std::cout << "Error: Model::load_texture" << std::endl;
        return;
    }
    image.flipVertically();
}

void Model::get_extremums()
{
    this->mins = position_vertices[0];
    this->maxes = position_vertices[0];

    for (int i = 1; i < this->position_vertices.size(); i++)
    {
        glm::vec3 vertex = this->position_vertices[i];

        if (abs(vertex.x - 1.156253) < 0.001)
            continue;

        if (vertex.x < this->mins.x)
            this->mins.x = vertex.x;
        if (vertex.y < this->mins.y)
            this->mins.y = vertex.y;
        if (vertex.z < this->mins.z)
            this->mins.z = vertex.z;

        if (vertex.x > this->maxes.x)
            this->maxes.x = vertex.x;
        if (vertex.y > this->maxes.y)
            this->maxes.y = vertex.y;
        if (vertex.z > this->maxes.z)
            this->maxes.z = vertex.z;
    }
}

Model::Model() { this->center = glm::vec3(0.0f, 0.0f, 0.0f); this->collision_radius = 0.0f; }

Model::Model(std::string obj_file, std::string texture_file)
{
    this->load_obj(obj_file);
    this->load_texture(texture_file);

    this->get_extremums();

    float x = (this->maxes.x + this->mins.x) / 2;
    float y = (this->maxes.y + this->mins.y) / 2;
    float z = (this->maxes.z + this->mins.z) / 2;

    this->center = glm::vec3(x, y, z);
    
    float radius_x = abs(this->maxes.x - this->mins.x);
    float radius_z = abs(this->maxes.z - this->mins.z);

    if (radius_x > radius_z)
        this->collision_radius = radius_x;
    else
        this->collision_radius = radius_z;
}

