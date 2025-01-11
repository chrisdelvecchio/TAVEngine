#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords;     // Texture coordinates passed to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 camMatrix = projection * view;
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
    
    // Pass the texture coordinates to the fragment shader
    TexCoords = aTexCoord;
}