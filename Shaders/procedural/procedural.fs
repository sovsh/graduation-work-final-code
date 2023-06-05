#version 410

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
struct Light 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light; 
uniform vec3 view_position;

in vec3 fragment_position;
in vec3 fragment_normal;

void main()
{
    vec3 ambient = material.ambient * light.ambient;

    vec3 _fragment_normal = normalize(fragment_normal);
    vec3 light_direction = normalize(light.direction);

    float diffuse_strength = max(dot(_fragment_normal, -light_direction), 0.0);
    vec3 diffuse = diffuse_strength * material.diffuse * light.diffuse;

    vec3 view_direction = normalize(view_position - fragment_position);
    vec3 reflected_light_direction = reflect(light_direction, _fragment_normal);

    float specular_strength_calculated = pow(max(dot(view_direction, reflected_light_direction), 0.0), material.shininess);
    vec3 specular = material.specular * specular_strength_calculated * light.specular;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}