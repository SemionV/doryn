#version 450 core

out vec4 fColor;
in vec4 fs_color;

uniform MaterialProperties
{
    vec4 color;
    vec4 color2;
    vec4 color3;
} material;

void main()
{
    fColor = material.color;
}
