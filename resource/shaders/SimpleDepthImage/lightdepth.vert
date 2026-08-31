#version 450
#include "../CommonShaders/constants.glsl"
#include "../CommonShaders/objectUBO.glsl"
#include "../CommonShaders/lightsUBO.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //not used
layout(location = 2) in vec2 inTexCoord; //not used
layout(location = 3) in vec3 inNormal; //not used
layout(location = 4) in vec4 inTangent; //tangent is not used here

void main() {
	//gl_Position = lightsUBO.mainCameraProj * lightsUBO.lightCameraView * objectUBO.model * vec4(inPosition, 1.0); //use light camera view to generate light depth image
	gl_Position = lightsUBO.lights[0].lightCameraProj * lightsUBO.lights[0].lightCameraView * objectUBO.model * vec4(inPosition, 1.0);
}