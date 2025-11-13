#version 450
#include "../Common/constants.glsl"
#include "../Common/objectUBO.glsl"
#include "../Common/graphicsGlobalUBO.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;
layout (location = 3) out vec3 outPosWorld;

void main(){
	gl_Position = globalUBO.mainCameraProj * globalUBO.mainCameraView * objectUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(objectUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = vec3(objectUBO.model * vec4(inPosition, 1.0));
}