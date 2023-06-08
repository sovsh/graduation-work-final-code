#version 410

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

struct Wind
{
  vec3 direction;
  float speed;
  float turbulence;
};

uniform mat4 view;
uniform mat4 projection;
uniform float frame;
uniform Wind wind;

out vec2 fragment_texture;

const float PI = 3.141592653589793;
float scaling_min = 0.9;
float scaling_max = 1.5;

mat3 create_rotation_x_matrix(float angle);
mat3 create_rotation_z_matrix(float angle);

float noise(vec3 point);
float random_range(vec3 v, float min, float max);

void main()
{
    vec3 position = gl_in[0].gl_Position.xyz;
    float grass_size = random_range(position, scaling_min, scaling_max);

    // left down, right down, left up, right up
    vec4 billboard_pos[4];
    vec2 billboard_text[4];
    
    billboard_pos[0] = vec4(-0.5, 0.0, 0.0, 0.0); 	
    billboard_pos[1] = vec4( 0.5, 0.0, 0.0, 0.0); 
    billboard_pos[2] = vec4(-0.5, 1.0, 0.0, 0.0);
    billboard_pos[3] = vec4( 0.5, 1.0, 0.0, 0.0);  

    billboard_text[0] = vec2(0.0, 0.0); 
    billboard_text[1] = vec2(1.0, 0.0);
    billboard_text[2] = vec2(0.0, 1.0); 
    billboard_text[3] = vec2(1.0, 1.0); 

    // mat[col][row]
    vec3 camera_right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camera_up = vec3(view[0][1], view[1][1], view[2][1]);

    for (int i = 0; i < 4; i++) 
    {   
      vec3 noise_param = position;
      noise_param.x += frame * wind.speed;
      noise_param *= wind.turbulence;
      vec3 noise_res = wind.direction * (noise(noise_param) + 0.5) * 0.5; 

      mat3 rotation_x = create_rotation_z_matrix((noise_res.x - 0.5 * noise_res.y) * 0.25 * PI);
      mat3 rotation_z = create_rotation_x_matrix(-(noise_res.z - 0.5 * noise_res.y) * 0.25 * PI);
      mat3 wind = rotation_z * rotation_x;

      gl_Position = projection * view * (gl_in[0].gl_Position + vec4(camera_right * billboard_pos[i].x * grass_size + wind * camera_up * billboard_pos[i].y * grass_size, 0.0));
      fragment_texture = billboard_text[i];        
      EmitVertex();
    }
    EndPrimitive();
}

float random(vec3 v)
{
  return fract(sin(dot(v, vec3(12.9898, 78.233, 23.1842))) * 43758.5453);
}

float random_range(vec3 v, float min, float max)
{
  return min + (random(v) * (max - min));
}

mat3 create_rotation_x_matrix(float angle)
{
  float angle_sin = sin(angle);
  float angle_cos = cos(angle);

  return mat3(
              1.0,  0.0,        0.0,        
              0.0,  angle_cos,  -angle_sin,
              0.0,  angle_sin,  angle_cos);
}

mat3 create_rotation_z_matrix(float angle)
{
  float angle_sin = sin(angle);
  float angle_cos = cos(angle);

  return mat3(
              angle_cos,  -angle_sin, 0.0,  
              angle_sin,  angle_cos,  0.0,  
              0.0,        0.0,        1.0);
}

vec3 gradients[20] = 
{
  vec3(0.92763, -0.51658, 0.07379),
  vec3(0.33349, -0.22820, 0.76129),
  vec3(0.39301, -0.51065, -0.30318),
  vec3(0.47075, 0.60003, 0.37545),
  vec3(0.91821, 0.53778, -0.94557),
  vec3(-0.94421, 0.72059, 0.74721),
  vec3(-0.44332, 0.80823, -0.22368),
  vec3(0.59167, -0.31907, 0.23878),
  vec3(-0.51525, -0.44456, 0.25479),
  vec3(-0.71634, -0.90992, -0.08300),
  vec3(0.33276, -0.59601, 0.61344),
  vec3(-0.30259, 0.59041, -0.68306),
  vec3(0.94636, 0.88358, -0.88606),
  vec3(-0.53157, -0.51713, -0.92584),
  vec3(-0.22741, 0.35045, 0.81120),
  vec3(-0.06787, 0.47332, 0.38434),
  vec3(0.61068, -0.28547, -0.99607),
  vec3(0.72278, -0.86660, 0.48078),
  vec3(-0.03875, 0.93421, 0.74847),
  vec3(-0.09994, 0.87064, 0.63276)
};

vec3 get_gradient(vec3 point)
{
  float num = random(point); // 0.0 - 0.999
  num *= 10;
  return gradients[int(num)]; 
}

float noise(vec3 point)
{
  vec3 point_int = floor(point);
  vec3 point_fract = fract(point);
	
  vec3 t = point_fract * point_fract * point_fract * (3.0 * point_fract * (2.0 * point_fract - 5.0) + 10.0);

  vec3 cube_left_down_back = vec3(0.0, 0.0, 0.0);
  vec3 cube_right_down_back = vec3(1.0, 0.0, 0.0);
  vec3 cube_left_up_back = vec3(0.0, 1.0, 0.0);
  vec3 cube_right_up_back = vec3(1.0, 1.0, 0.0);
  vec3 cube_left_down_front = vec3(0.0, 0.0, 1.0);
  vec3 cube_right_down_front = vec3(1.0, 0.0, 1.0);
  vec3 cube_left_up_front = vec3(0.0, 1.0, 1.0);
  vec3 cube_right_up_front = vec3(1.0, 1.0, 1.0);

  float left_down_back = dot(get_gradient(point_int + cube_left_down_back), point_fract - cube_left_down_back);
  float right_down_back = dot(get_gradient(point_int + cube_right_down_back), point_fract - cube_right_down_back);
  float left_up_back = dot(get_gradient(point_int + cube_left_up_back), point_fract - cube_left_up_back);
  float right_up_back = dot(get_gradient(point_int + cube_right_up_back), point_fract - cube_right_up_back);
  float left_down_front = dot(get_gradient(point_int + cube_left_down_front), point_fract - cube_left_down_front);
  float right_down_front = dot(get_gradient(point_int + cube_right_down_front), point_fract - cube_right_down_front);
  float left_up_front = dot(get_gradient(point_int + cube_left_up_front), point_fract - cube_left_up_front);
  float right_up_front = dot(get_gradient(point_int + cube_right_up_front), point_fract - cube_right_up_front);

  float down_back = mix(left_down_back, right_down_back, t.x);
  float up_back = mix(left_up_back, right_up_back, t.x);
  float down_front = mix(left_down_front, right_down_front, t.x);
  float up_front = mix(left_up_front, right_up_front, t.x);

  float back = mix(down_back, down_back, t.y);
  float front = mix(down_front, down_front, t.y);

  return mix(back, front, t.z);
}