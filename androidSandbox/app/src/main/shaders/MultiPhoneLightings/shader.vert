#version 450 core

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 mainCameraProj;
	mat4 mainCameraView;
	mat4 padding0;
	mat4 padding1;
	mat4 padding2;
	mat4 padding3;
	mat4 padding4; 
} mvpUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;
layout (location = 3) out vec3 outPosWorld;

// layout (location = 3+0*LIGHT_NUM) out vec3 outViewVec[LIGHT_NUM];
// layout (location = 3+1*LIGHT_NUM) out vec3 outLightVec[LIGHT_NUM];
// layout (location = 3+2*LIGHT_NUM) out float outAmbientIntensity[LIGHT_NUM];
// layout (location = 3+3*LIGHT_NUM) out float outDiffuseIntensity[LIGHT_NUM];
// layout (location = 3+4*LIGHT_NUM) out float outSpecularIntensity[LIGHT_NUM];

void main() {
	gl_Position = mvpUBO.mainCameraProj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(mvpUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = vec3(mvpUBO.model * vec4(inPosition, 1.0));
}