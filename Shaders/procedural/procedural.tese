#version 410

layout (isolines, equal_spacing) in;

struct Wind
{
  vec3 direction;
  float speed;
  float turbulence;
};

uniform float frame;
uniform Wind wind;

out vec3 normal;
out vec3 binormal;
out float stem_thickness;

const float PI = 3.1415926535897932384626433832795;
float scaling_min = 0.5;
float scaling_max = 0.9;
float radius = 0.3;

float random_range(vec3 v, float min, float max);
mat3 create_rotation_y_matrix(float angle);
float noise(vec3 p);

void main()
{
    vec3 position;
    vec2 p0 = vec2(0.0, 0.0);
    vec2 p1 = vec2(0.0, 0.63);
    vec2 p2 = vec2(0.53, 1.2);
    vec2 p3 = vec2(1.0, 1.0);
    
    float t = gl_TessCoord.x; 
    float d = 1.0 - t;
    position.xy = p0 * d * d * d + 3.0 * p1 * d * d * t + 3.0 * p2 * t * t * d + p3 * t * t * t;
    position.z = 0.0;

    position.x += radius;

    float angle = gl_TessCoord.y * 2.0 * PI;
    mat3 rotation_y = create_rotation_y_matrix(angle);
    position = rotation_y * position;

    float grass_size = random_range(gl_in[0].gl_Position.xyz, scaling_min, scaling_max);
    position *= grass_size;
    
    position += gl_in[0].gl_Position.xyz;

    normal.x = 3.0 * p0.y * d * d + 6.0 * p1.y * t * d + 3.0 * p1.y * d * d + 6.0 * p2.y * t * d + 3.0 * p2.y * t * t + 3.0 * p3.y * t * t;
    normal.y = -(3.0 * p0.x * d * d + 6.0 * p1.x * t * d + 3.0 * p1.x * d * d + 6.0 * p2.x * t * d + 3.0 * p2.x * t * t + 3.0 * p3.x * t * t);
    normal.z = 0.0;
    normal = rotation_y * normalize(normal);

    binormal = rotation_y * vec3(0.0, 0.0, 1.0);   

    stem_thickness = 1.0 - gl_TessCoord.x;

    vec3 noise_param = position;
    noise_param.x += frame * wind.speed;
    noise_param *= wind.turbulence;
    float flexibility = t * t * 0.3; 

    vec3 wind = wind.direction * flexibility * (noise(noise_param) + 0.5);

    gl_Position = vec4(position + wind, 1.0);
}

float random(vec3 v)
{
  return fract(sin(dot(v, vec3(12.9898, 78.233, 23.1842))) * 43758.5453);
}

float random_range(vec3 v, float min, float max)
{
  return min + (random(v) * (max - min));
}

mat3 create_rotation_y_matrix(float angle)
{
    float angle_sin = sin(angle);
    float angle_cos = cos(angle);

    return mat3(
                angle_cos,  0.0,  angle_sin,  
                0.0,        1.0,  0.0,       
                -angle_sin, 0.0,  angle_cos);
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