#version 450 core

out vec4 fColor;

uniform material
{
    vec4 color;
    vec4 color2;
    vec4 color3;
};

void main()
{
    fColor = color;
}
