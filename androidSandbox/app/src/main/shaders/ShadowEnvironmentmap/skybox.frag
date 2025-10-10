#version 450

layout(set = 1, binding = 0) uniform samplerCube cubemapSampler;

//layout(location = 0) in vec3 fragColor;
//layout(location = 1) in vec2 fragTexCoord;
layout(location = 0) in vec3 pos;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(cubemapSampler, pos);
}