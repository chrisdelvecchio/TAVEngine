#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 camMatrix = projection * view;
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
}