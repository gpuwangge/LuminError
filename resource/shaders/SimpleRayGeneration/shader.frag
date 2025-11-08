#version 450

layout(set = 0, binding = 0) uniform GlobalBufferObject {
	mat4 mainCameraProj;
    mat4 mainCameraView;
    float tanHalfFovY;
    float aspect;
    float padding[30];
}globalUBO;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, fragTexCoord);
}