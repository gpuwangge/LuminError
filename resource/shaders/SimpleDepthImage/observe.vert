#version 450
#include "../Common/constants.glsl"
#include "../Common/globalUBO.glsl"

layout(set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} objectUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec2 outTexCoord;


void main() {
	mat4 p = objectUBO.identityCameraProj ? mat4(1.0) : globalUBO.mainCameraProj;
	mat4 v = objectUBO.identityCameraView ? mat4(1.0) : globalUBO.mainCameraView;
	gl_Position = p * v * objectUBO.model * vec4(inPosition, 1.0);
	outTexCoord = inTexCoord;
}