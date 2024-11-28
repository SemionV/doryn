#version 450 core

out vec4 fColor;
uniform vec4 triangleColor;

uniform ColorsBlock
{
    vec4 brightColor;
    vec4 hippieColor;
    vec4 darkColor;
} colors;

void main()
{
    fColor = vec4(0.2, 0.2, 0.9, 1.0);
    //fColor = colors.hippieColor;
}
