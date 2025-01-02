#version 460 core

out vec4 FragColor;

uniform vec3 color;          // Uniform color passed from the application

void main()
{
    FragColor = vec4(color, 1.0);
}