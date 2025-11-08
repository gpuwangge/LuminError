#version 450
#include "../Common/constants.glsl"

layout(set = 0, binding = UNIFORM_GLOBAL_BINDING) uniform GlobalBufferObject {
	mat4 mainCameraProj;
    mat4 mainCameraView;
    float tanHalfFovY;
    float aspect;
    float padding[30];
} globalUBO;

layout(set = 0, binding = UNIFORM_OBJECT_BINDING) uniform UniformBufferObject {
    mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = globalUBO.mainCameraProj * globalUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}
