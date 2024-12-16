
#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
uniform mat4x4 viewProjectionTransform;
uniform mat4x4 modelTransform;

void main()
{
    vec4 homogenousPosition;
    homogenousPosition.x = vPosition.x;
    homogenousPosition.y = vPosition.y;
    homogenousPosition.z = vPosition.z;
    homogenousPosition.w = 1;
    gl_Position = viewProjectionTransform * modelTransform * homogenousPosition;
}