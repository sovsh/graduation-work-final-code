#version 410

layout(lines) in;
layout(triangle_strip, max_vertices = 256) out;

uniform mat4 view;
uniform mat4 projection;

in vec3 normal[];
in vec3 binormal[];
in float stem_thickness[];

out vec3 fragment_position;
out vec3 fragment_normal;

const float PI = 3.1415926535897932384626433832795;
int stem_sectors = 16; 
float stem_radius_max = 0.035; 

void main()
{
    for (int i = 0; i <= stem_sectors; i++)
    { 
        float t = i / float(stem_sectors); 
        float angle = t * 2.0 * PI; 
        for (int j = 0; j < 2; j++) 
        { 
            vec3 radius = cos(angle) * normal[j] + sin(angle) * binormal[j];
            vec3 position = radius * stem_radius_max * stem_thickness[j] + gl_in[j].gl_Position.xyz; 
            
            gl_Position = projection * view * vec4(position, gl_in[j].gl_Position.w);
            fragment_position = position;
            fragment_normal = radius;
            EmitVertex();
        }
    }
    EndPrimitive();
}