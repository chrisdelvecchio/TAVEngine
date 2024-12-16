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
uniform bool isSprite;

void main()
{
    if (isSprite) {
    // Extract sprite position from the model matrix (translation is in the 4th column)
        vec3 spritePos = vec3(aInstancePos[3][0], aInstancePos[3][1], aInstancePos[3][2]);

    // Extract sprite scale from the model matrix (assumes uniform scaling)
        float spriteScale = length(vec3(aInstancePos[0][0], aInstancePos[1][0], aInstancePos[2][0])); // Length of the X-axis row

    // Extract the camera's right and up vectors from the view matrix
        vec3 cameraRight = normalize(vec3(view[0][0], view[1][0], view[2][0])); // First column of view matrix
        vec3 cameraUp = normalize(vec3(view[0][1], view[1][1], view[2][1]));    // Second column of view matrix

    // Offset the quad vertices using the camera vectors and the sprite's scale
        vec3 rightOffset = cameraRight * aPos.x * spriteScale;
        vec3 upOffset = cameraUp * aPos.y * spriteScale;

    // Compute the final position of the vertex in world space
        vec3 worldPos = spritePos + rightOffset + upOffset;

    // Transform the vertex position to clip space
        gl_Position = projection * view * aInstancePos * vec4(worldPos, 1.0);
    } else {
        mat4 camMatrix = projection * view;
        gl_Position = camMatrix * aInstancePos * vec4(aPos, 1.0);
    }

    TexCoords = aTexCoord;

}