#version 450 core

out vec4 fColor;

layout(binding = 0) uniform MaterialProperties
{
    vec4 color;
    vec4 color2;
    vec4 color3;
} material;

void main()
{
    fColor = material.color;
}
