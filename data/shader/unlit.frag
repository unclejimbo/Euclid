#version 430 core

layout(binding = 1) uniform Material
{
    vec3 ambient;
    vec3 diffuse;
}
material;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(material.ambient, 1.0f);
}
