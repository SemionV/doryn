#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec3 vNormal;
layout( location = 2 ) in vec4 vColor;
uniform mat4x4 viewProjectionTransform;
uniform mat4x4 modelTransform;

out vec4 fs_color;

void main()
{
    vec4 homogenousPosition = vec4(vPosition, 1.0);
    gl_Position = viewProjectionTransform * modelTransform * homogenousPosition;

    fs_color = vColor;
}