#version 410 

layout (vertices = 1) out;

uniform vec3 camera_position;
uniform vec3 camera_front;

void main()
{
    vec4 position = gl_in[gl_InvocationID].gl_Position;

    float camera_distance = distance(position.xyz, camera_position);
    float detail_level = 6;
    if (camera_distance < 1.0)
        detail_level = 48;
    else if (camera_distance < 2.0)
        detail_level = 36;
    else if (camera_distance < 6.0)
        detail_level = 24;
    else if (camera_distance < 40.0)
       detail_level = 12;

    float front_or_behind_camera = step(0, dot(position.xyz - camera_position, camera_front));

    gl_out[gl_InvocationID].gl_Position = position;
    gl_TessLevelOuter[0] = position.w;
    gl_TessLevelOuter[1] = detail_level * front_or_behind_camera;
}