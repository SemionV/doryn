#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
uniform mat4x4 viewProjectionTransform;
uniform mat4x4 modelTransform;

void main()
{
    vec4 homogenousPosition = vec4(vPosition, 1.0);
    gl_Position = viewProjectionTransform * modelTransform * homogenousPosition;
}