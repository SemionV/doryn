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
    //fColor = vec4(0.5, 0.4, 0.8, 1.0);
    fColor = colors.hippieColor;    
}
