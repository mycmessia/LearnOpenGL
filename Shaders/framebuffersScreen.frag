#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D screenTexture;

void main()
{
    color = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}