#version 330 core
in vec3 position;
in vec3 normal;
in vec2 texture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_texture;

void main() 
{
    gl_Position = projection * view * model * vec4(position, 1.0);

    fragment_position = vec3(model * vec4(position, 1.0));
    fragment_normal = mat3(transpose(inverse(model))) * normal;
    fragment_texture = texture;
}