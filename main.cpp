#define _CRT_SECURE_NO_DEPRECATE

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "grass_model.h"
#include "wind.h"

using namespace std;

unsigned int WIDTH = 1920;
unsigned int HEIGHT = 1080;

bool is_procedual_mode = true;

Shader shader_model, shader_grass_proc, shader_grass_bil;
Camera camera;
Model field;
GrassModel grass;

glm::vec3 light_direction = glm::vec3(-0.2f, -1.0f, 0.2f);

glm::vec3 field_position = { 0.0f, 0.0f, 0.0f };

GLuint field_vao, grass_proc_vao, grass_bil_vao;
// position, normal, texture
GLuint field_vbo[3];
GLuint field_texture;
float field_shininess;

GLuint grass_vbo, grass_vbo_bil;
GLuint grass_texture;
glm::vec3 grass_ambient, grass_diffuse, grass_specular;
float grass_shininess;

glm::mat4 field_translate, field_rotate, field_scale;

glm::mat4 model, view, projection;

Wind wind;
float wind_speed_min, wind_speed_max, wind_turbulence_min, wind_turbulence_max;

int frame_current = 0;
const int frame_max = 60000;

void init();
void draw_scene();

int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 0;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Final Project", sf::Style::Close, settings);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);

	sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2));

	glewInit();

	init();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case(sf::Keyboard::W):camera.move(1.0f); break;
				case(sf::Keyboard::S):camera.move(-1.0f); break;
				case(sf::Keyboard::A):camera.rotate(-7.0f); break;
				case(sf::Keyboard::D):camera.rotate(7.0f); break;

				case(sf::Keyboard::Up):wind.change_speed(1000.f); break;
				case(sf::Keyboard::Down):wind.change_speed(-1000.f); break;
				case(sf::Keyboard::Right):wind.change_turbulence(0.1f); break;
				case(sf::Keyboard::Left):wind.change_turbulence(-0.1f); break;

				case(sf::Keyboard::X):wind.change_direction(glm::vec3(1.0f, 0.0f, 0.0f)); break;
				case(sf::Keyboard::Y):wind.change_direction(glm::vec3(0.0f, 1.0f, 0.0f)); break;
				case(sf::Keyboard::Z):wind.change_direction(glm::vec3(0.0f, 0.0f, 1.0f)); break;

				case(sf::Keyboard::Space):is_procedual_mode = !is_procedual_mode; break;
				case(sf::Keyboard::Escape):window.close(); break;
				default: break;
				}
			}
			else if (event.type == sf::Event::MouseMoved)
			{
				camera.rotate(sf::Mouse::getPosition(window));
			}

		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_scene();

		window.display();
	}

	glDeleteBuffers(1, &field_vao); 
	glDeleteBuffers(1, &grass_proc_vao);
	glDeleteBuffers(1, &grass_bil_vao);

	shader_model.delete_program();
	shader_grass_proc.delete_program();
	shader_grass_bil.delete_program();

	return 0;
}

void check_error()
{
	GLenum error_code;
	// https://www.khronos.org/opengl/wiki/OpenGL_Error
	if ((error_code = glGetError()) != GL_NO_ERROR)
		cout << "OpenGl error!: " << error_code << endl;
}

void init_shaders()
{
	shader_model = Shader("Shaders/object_shader.vs", "Shaders/object_shader.fs");
	shader_grass_proc = Shader("Shaders/procedural/procedural.vs", "Shaders/procedural/procedural.tesc", "Shaders/procedural/procedural.tese", "Shaders/procedural/procedural.gs", "Shaders/procedural/procedural.fs");
	shader_grass_bil = Shader("Shaders/billboarding/billboarding.vs", "Shaders/billboarding/billboarding.gs", "Shaders/billboarding/billboarding.fs");

	check_error();
}

void init_camera()
{
	glm::vec3 position = glm::vec3(0.0f, 5.0f, 10.0f); 
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

	float sensitivity = 0.05f;
	float speed = 0.1f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float last_x = WIDTH / 2.0;
	float last_y = HEIGHT / 2.0;

	camera = Camera(position, front, sensitivity, speed, yaw, pitch, last_x, last_y);
}

void init_projection()
{
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
}

void generate_model_vao(Model model, GLuint& vao, GLuint vbo[3])
{
	GLuint attributes[3];

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices_count * sizeof(glm::vec2), &model.texture_vertices[0], GL_STATIC_DRAW);
	attributes[2] = shader_model.get_attribute_position("texture");
	glVertexAttribPointer(attributes[2], 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attributes[2]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices_count * sizeof(glm::vec3), &model.normal_vertices[0], GL_STATIC_DRAW);
	attributes[1] = shader_model.get_attribute_position("normal");
	glVertexAttribPointer(attributes[1], 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attributes[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, model.vertices_count * sizeof(glm::vec3), &model.position_vertices[0], GL_STATIC_DRAW);
	attributes[0] = shader_model.get_attribute_position("position");
	glVertexAttribPointer(attributes[0], 3, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(attributes[0]);

	glBindVertexArray(0);

	glDisableVertexAttribArray(attributes[0]);
	glDisableVertexAttribArray(attributes[1]);
	glDisableVertexAttribArray(attributes[2]);

	glDeleteBuffers(3, vbo);

	check_error();
}

void generate_grass_proc_vao()
{
	GLuint attribute;

	glGenVertexArrays(1, &grass_proc_vao);

	glBindVertexArray(grass_proc_vao);

	glBindBuffer(GL_ARRAY_BUFFER, grass_vbo);
	glBufferData(GL_ARRAY_BUFFER, grass.vertices.size() * sizeof(glm::vec4), &grass.vertices[0], GL_STATIC_DRAW);
	attribute = shader_grass_proc.get_attribute_position("vertex");
	glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(attribute);

	glBindVertexArray(0);
	glDisableVertexAttribArray(attribute);

	glDeleteBuffers(1, &grass_vbo);
}

void generate_grass_bil_vao()
{
	GLuint attribute;

	glGenVertexArrays(1, &grass_bil_vao);

	glBindVertexArray(grass_bil_vao);

	glBindBuffer(GL_ARRAY_BUFFER, grass_vbo_bil);
	glBufferData(GL_ARRAY_BUFFER, grass.vertices.size() * sizeof(glm::vec4), &grass.vertices[0], GL_STATIC_DRAW); 
	attribute = shader_grass_bil.get_attribute_position("vertex");
	glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(attribute);

	glBindVertexArray(0);

	glDisableVertexAttribArray(attribute);

	glDeleteBuffers(1, &grass_vbo_bil);

	check_error();
}


void generate_texture(Model model, GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, model.image.getSize().x, model.image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, model.image.getPixelsPtr());

	glGenerateMipmap(GL_TEXTURE_2D);


	glBindTexture(GL_TEXTURE_2D, 0);
}

void generate_texture_grass(GrassModel model, GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, model.image.getSize().x, model.image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, model.image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void init_field() 
{
	field = Model("Models/Field/Field.obj", "Models/Field/Field.png");
	generate_model_vao(field, field_vao, field_vbo);
	generate_texture(field, field_texture);
	field_shininess = 4.0f;

	field_translate = glm::translate(glm::mat4(1.0f), field_position);
	field_rotate = glm::mat4(1.0f);
	field_scale = glm::mat4(1.0f);
}

void init_grass()
{
	int grass_count = 500;
	float field_min = -19.5f;
	float field_max = 19.5f;
	int grass_min = 5;
	int grass_max = 8;

	grass = GrassModel("Models/Grass/Grass.png", grass_count, field_min, field_max, field_min, field_max, grass_min, grass_max);

	generate_grass_proc_vao();
	generate_grass_bil_vao();
	generate_texture_grass(grass, grass_texture);

	grass_ambient = glm::vec3(156.0f / 255, 222.0f / 255, 53.0f / 255);
	grass_diffuse = glm::vec3(161.0f / 255, 238.0f / 255, 40.f / 255); 
	grass_specular = glm::vec3(0.5f, 0.5f, 0.5f);
	grass_shininess = 16.0f;
}

void init_vbos()
{
	glGenBuffers(3, field_vbo);
	glGenBuffers(1, &grass_vbo);
	glGenBuffers(1, &grass_vbo_bil);
}

void draw(Model model, GLuint vao, GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, model.position_vertices.size());

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void init_objects()
{
	init_vbos();
	init_field();
	init_grass();
}

void init_wind()
{
	wind_speed_min = 1000.0f;
	wind_speed_max = 9000.0f;
	wind_turbulence_min = 0.1f;
	wind_turbulence_max = 0.9f;
	wind = Wind(wind_speed_min, wind_speed_max, wind_turbulence_min, wind_turbulence_max);
}

void init() {
	init_shaders();
	init_objects();
	init_projection();
	init_camera();
	init_wind();

	glPointSize(2.0f);
	glClearColor(204 / 255.0f, 230 / 255.0f, 255 / 255.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void draw_field()
{
	shader_model.use();

	shader_model.set_uniform_mat4("projection", projection);

	shader_model.set_uniform_vec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	shader_model.set_uniform_vec3("light.direction", glm::vec3(-0.2f, -1.0f, 0.2f));
	shader_model.set_uniform_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader_model.set_uniform_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader_model.set_uniform_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	shader_model.set_uniform_mat4("view", view);
	shader_model.set_uniform_vec3("view_position", camera.position);

	model = field_translate * field_rotate * field_scale;
	shader_model.set_uniform_mat4("model", model);
	shader_model.set_uniform_float("material.shininess", field_shininess);
	draw(field, field_vao, field_texture);
	shader_model.unuse();
}

void draw_grass()
{
	frame_current = (frame_current + 1) % frame_max;
	float frame = float(frame_current) / float(frame_max);

	if (is_procedual_mode)
	{
		shader_grass_proc.use();

		shader_grass_proc.set_uniform_mat4("projection", projection);
		shader_grass_proc.set_uniform_mat4("view", view);
		shader_grass_proc.set_uniform_vec3("camera_position", camera.position);
		shader_grass_proc.set_uniform_vec3("camera_front", camera.front);

		shader_grass_proc.set_uniform_vec3("light.direction", glm::vec3(-0.2f, -1.0f, 0.2f));
		shader_grass_proc.set_uniform_vec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shader_grass_proc.set_uniform_vec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader_grass_proc.set_uniform_vec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader_grass_proc.set_uniform_vec3("view_position", camera.position);

		shader_grass_proc.set_uniform_vec3("material.ambient", grass_ambient);
		shader_grass_proc.set_uniform_vec3("material.diffuse", grass_diffuse);
		shader_grass_proc.set_uniform_vec3("material.specular", grass_specular);
		shader_grass_proc.set_uniform_float("material.shininess", grass_shininess);

		shader_grass_proc.set_uniform_float("frame", frame);

		shader_grass_proc.set_uniform_vec3("wind.direction", wind.direction);
		shader_grass_proc.set_uniform_float("wind.speed", wind.speed);
		shader_grass_proc.set_uniform_float("wind.turbulence", wind.turbulence);

		glBindVertexArray(grass_proc_vao);
		glPatchParameteri(GL_PATCH_VERTICES, 1);
		glDrawArrays(GL_PATCHES, 0, grass.vertices.size());
		glBindVertexArray(0);

		shader_grass_proc.unuse();
		return;
	}

	shader_grass_bil.use();

	shader_grass_bil.set_uniform_mat4("projection", projection);
	shader_grass_bil.set_uniform_mat4("view", view);
	
	shader_grass_bil.set_uniform_float("frame", frame);

	shader_grass_bil.set_uniform_vec3("wind.direction", wind.direction);
	shader_grass_bil.set_uniform_float("wind.speed", wind.speed);
	shader_grass_bil.set_uniform_float("wind.turbulence", wind.turbulence);


	glBindTexture(GL_TEXTURE_2D, grass_texture);
	glBindVertexArray(grass_bil_vao);

	glDrawArrays(GL_POINTS, 0, grass.vertices.size());

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shader_grass_bil.unuse();
}

void draw_scene() 
{
	view = camera.get_view_matrix();

	draw_field();
	draw_grass();
}
