#version 450
#include "../CommonShaders/constants.glsl"
#include "../CommonShaders/lightsUBO.glsl"

layout(push_constant) uniform PushConstant{
    int value; //the index of shadowmap renderpass
} pc;

layout(set = 0, binding = UNIFORM_OBJECT_BINDING) uniform StructObjectBuffer {
	mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} objectUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //not used
layout(location = 2) in vec2 inTexCoord; //not used
layout(location = 3) in vec3 inNormal; //not used

void main() {
	gl_Position = lightsUBO.lights[pc.value].lightCameraProj * lightsUBO.lights[pc.value].lightCameraView * objectUBO.model * vec4(inPosition, 1.0);
}