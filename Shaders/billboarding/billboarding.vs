#version 410

in vec4 vertex;

void main() 
{
    gl_Position = vec4(vertex.xyz, 1.0);
}