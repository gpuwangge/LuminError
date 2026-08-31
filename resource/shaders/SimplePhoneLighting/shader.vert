#version 450
#include "../CommonShaders/constants.glsl"
#include "../CommonShaders/objectUBO.glsl"
#include "../CommonShaders/graphicsGlobalUBO.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec4 inTangent; //tangent is not used here

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;
layout (location = 3) out vec3 outPosWorld;

void main(){
	gl_Position = graphicsGlobalUBO.mainCameraProj * graphicsGlobalUBO.mainCameraView * objectUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(objectUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = vec3(objectUBO.model * vec4(inPosition, 1.0));
}