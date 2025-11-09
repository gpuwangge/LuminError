#version 450
#include "../Common/constants.glsl"
#include "../Common/objectUBO.glsl"
#include "../Common/globalUBO.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal; //normal is not used here

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = globalUBO.mainCameraProj * globalUBO.mainCameraView * objectUBO.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}