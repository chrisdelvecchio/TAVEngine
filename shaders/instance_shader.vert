#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 aInstancePos;

out vec3 FragPos;       // Position of the fragment (for lighting calculations)
out vec3 Normal;        // Normal of the fragment
out vec2 TexCoords;     // Texture coordinates passed to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int instanceCount;

void main()
{
    TexCoords = aTexCoord;

    mat4 camMatrix = projection * view;

    gl_Position = camMatrix * aInstancePos * vec4(aPos, 1.0);
}