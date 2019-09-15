#version 430 core

layout(binding = 1) uniform Material
{
    vec3 ambient;
    vec3 diffuse;
}
material;

layout(binding = 2) uniform Light
{
    vec3 position;
    vec3 color;
    float intensity;
}
light;

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 worldNormal;

layout(location = 0) out vec4 color;

void main()
{
    vec3 lightDir = normalize(light.position - worldPosition);
    float diff = max(dot(worldNormal, lightDir), 0.0f);
    vec3 c = (material.ambient + material.diffuse * diff) * light.color *
             light.intensity;
    color = vec4(c, 1.0f);
}
