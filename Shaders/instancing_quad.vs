#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fColor;

uniform vec2 offsets[100];

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(position + offset, 0.0f, 1.0f);
    fColor = color;
}