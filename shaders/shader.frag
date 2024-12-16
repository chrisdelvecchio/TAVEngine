#version 460 core

/* Scene Objects */
in vec3 FragPos;      // Fragment position
in vec3 Normal;       // Fragment normal
in vec2 TexCoords;    // Texture coordinates

out vec4 FragColor;

uniform sampler2D texture1;  // Texture sampler

uniform bool useTexture;     // Whether to use texture or not
uniform vec3 color;          // Uniform color passed from the application

// model & mesh
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
	vec4 objectColor = vec4(color, 1.0);

    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        objectColor = texColor * objectColor;
    }

    if (objectColor.a < 0.1) // Discard nearly transparent pixels
        discard;

    FragColor = objectColor;
}