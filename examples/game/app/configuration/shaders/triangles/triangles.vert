
#version 400 core

layout( location = 0 ) in vec4 vPosition;

void main()
{
    const vec4 position = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    gl_Position = vPosition;
}
