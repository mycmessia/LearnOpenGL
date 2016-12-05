#version 330 core

out vec4 color;

void main ()
{
    color = vec4(vec3(gl_FragCoord.z), 1.0f);
}