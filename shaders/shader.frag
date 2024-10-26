#version 460 core

/* Scene Objects */
in vec3 FragPos;      // Fragment position
in vec3 Normal;       // Fragment normal
in vec2 TexCoords;    // Texture coordinates

out vec4 FragColor;

uniform sampler2D texture1;  // Texture sampler
uniform bool useTexture;     // Whether to use texture or not
uniform vec3 color;          // Uniform color passed from the application

void main()
{
	vec3 objectColor = color;

    if (useTexture) {
        objectColor = texture(texture1, TexCoords).rgb * objectColor;
    }

    FragColor = vec4(objectColor, 1.0);
}