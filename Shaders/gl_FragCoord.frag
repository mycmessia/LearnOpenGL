#version 330 core
out vec4 color;

void main()
{
    // x is width set by glViewport (0, 0, width, height);
    if (gl_FragCoord.x < 800)
        color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else
        color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}