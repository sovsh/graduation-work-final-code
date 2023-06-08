#version 410

in vec2 fragment_texture;

uniform sampler2D uniform_texture;

void main() 
{
    vec4 fragment_color = texture(uniform_texture, fragment_texture);
    if (fragment_color.a < 0.05) 
        discard;
        
    gl_FragColor = fragment_color;
}