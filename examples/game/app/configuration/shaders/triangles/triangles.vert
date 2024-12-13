
#version 400 core

layout( location = 0 ) in vec4 vPosition;
uniform mat4x4 viewProjectionTransform;
uniform mat4x4 modelTransform;

void main()
{
    gl_Position = viewProjectionTransform * modelTransform * vPosition;
}
