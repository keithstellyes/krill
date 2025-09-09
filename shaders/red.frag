#version 330 core
out vec4 FragColor;

in float red;

uniform float green;

void main()
{
    FragColor = vec4(red, green, 0.0f, 1.0f);
}
