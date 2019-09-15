#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexColor;

layout(binding = 0) uniform Transform
{
    mat4 modelMatrix;
    mat4 mvp;
    mat4 normalMatrix;
}
transform;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 worldNormal;
layout(location = 2) out vec3 vcolor;

void main()
{
    worldPosition = mat3(transform.modelMatrix) * vertexPosition;
    worldNormal = mat3(transform.normalMatrix) * vertexNormal;
    vcolor = vertexColor;
    gl_Position = transform.mvp * vec4(vertexPosition, 1.0f);
}
