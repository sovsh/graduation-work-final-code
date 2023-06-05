#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>;

class Shader
{
private:
	GLuint shader_program;

	void create_shader(std::string shader_file, GLuint& shader, int type);

	void create_shader_program(GLuint vertex_shader, GLuint fragment_shader);
	void create_shader_program(GLuint vertex_shader, GLuint geometry_shader, GLuint fragment_shader);
	void create_shader_program(GLuint vertex_shader, GLuint tess_control_shader, GLuint tess_evaluation_shader, GLuint fragment_shader);
	void create_shader_program(GLuint vertex_shader, GLuint tess_control_shader, GLuint tess_evaluation_shader, GLuint geometry_shader, GLuint fragment_shader);

	void check_errors(unsigned int shader, std::string type);
	void check_errors_shader(unsigned int shader);
	void check_errors_program();

	void check_error();
public:
	Shader();
	Shader(std::string vertex_shader_file, std::string fragment_shader_file);
	Shader(std::string vertex_shader_file, std::string geometry_shader_file, std::string fragment_shader_file);
	Shader(std::string vertex_shader_file, std::string tess_control_shader_file, std::string tess_evaluation_shader_file, std::string fragment_shader_file);
	Shader(std::string vertex_shader_file, std::string tess_control_shader_file, std::string tess_evaluation_shader_file, std::string geometry_shader_file, std::string fragment_shader_file);

	void use();
	void unuse();

	GLuint get_attribute_position(const GLchar* name);

	void set_uniform_int(const std::string& name, int value) const;
	void set_uniform_float(const std::string& name, float value) const;
	void set_uniform_vec3(const std::string& name, glm::vec3 value) const;
	void set_uniform_vec4(const std::string& name, glm::vec4 value) const;
	void set_uniform_mat4(const std::string& name, glm::mat4 value) const;
};