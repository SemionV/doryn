#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;

out vec4 fs_color;

void main()
{
    gl_Position = vPosition;
    fs_color = vColor;
}
