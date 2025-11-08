#version 450

layout(set = 0, binding = 0) uniform GlobalBufferObject {
	mat4 mainCameraProj;
    mat4 mainCameraView;
    float tanHalfFovY;
    float aspect;
    float padding[30];
} globalUBO;

layout(set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //no use
layout(location = 2) in vec2 inTexCoord; //no use
layout(location = 3) in vec3 inNormal; //no use

layout(location = 0) out vec3 outPos;

void main() {
	mat4 view = mat4(mat3(globalUBO.mainCameraView)); //remove translation of skybox
    vec4 pos = globalUBO.mainCameraProj * view * mvpUBO.model * vec4(inPosition, 1.0);
    gl_Position = pos.xyww; //cheat in depth test
    outPos = inPosition; //pass input position to fragment for cubemap sampler
}