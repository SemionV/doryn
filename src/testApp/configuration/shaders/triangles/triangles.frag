#version 450 core

out vec4 fColor;
uniform vec4 triangleColor;

void main()
{
    //fColor = vec4(0.5, 0.4, 0.8, 1.0);
    fColor = triangleColor;    
}
