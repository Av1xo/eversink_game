#version 450 core

layout (location = 0) in vec3 aPos;
out vec4 vertexColor;
out vec3 outPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(1.0, 0.902, 0.0, 1.0);
    outPos = aPos;
}