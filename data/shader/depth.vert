#version 430 core

layout(location = 0) in vec3 vertexPosition;

layout(binding = 0) uniform Transform
{
    mat4 modelMatrix;
    mat4 mvp;
    mat4 normalMatrix;
}
transform;

void main()
{
    gl_Position = transform.mvp * vec4(vertexPosition, 1.0f);
}
