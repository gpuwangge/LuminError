#version 450
#include "../Common/constants.glsl"

layout(set = 0, binding = UNIFORM_GLOBAL_BINDING) uniform GlobalBufferObject {
	mat4 mainCameraProj;
    mat4 mainCameraView;
    float tanHalfFovY;
    float aspect;
    float padding[30];
} globalUBO;

layout(set = 0, binding = UNIFORM_OBJECT_BINDING) uniform MVPBufferObject {
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

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;
layout (location = 3) out vec4 outPosWorld;

//layout (location = 4) out vec4 outFragPosLightSpace;

//biasMat is to prevent shadow acne
// const mat4 biasMat = mat4( 
// 	0.5, 0.0, 0.0, 0.0,
// 	0.0, 0.5, 0.0, 0.0,
// 	0.0, 0.0, 1.0, 0.0,
// 	0.5, 0.5, 0.0, 1.0 );

void main() 
{
	// outNormal = inNormal;
	// outColor = inColor;
	// fragTexCoord = inTexCoord;
	
	// gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	
	// vec4 pos = ubo.model * vec4(inPosition, 1.0);
	// outNormal = mat3(ubo.model) * inNormal;
	// //vec3 lPos = mat3(ubo.model) * customUBO.lightPos;
	// outLightVec = normalize(customUBO.lightPos - inPosition.xyz);
	// outViewVec = -pos.xyz;		

	// outShadowCoord = (biasMat * customUBO.lightSpace * ubo.model ) * vec4(inPosition, 1.0);

	gl_Position = globalUBO.mainCameraProj * globalUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(mvpUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = mvpUBO.model * vec4(inPosition, 1.0);

	//outFragPosLightSpace = mvpUBO.lightCameraProj * mvpUBO.lightCameraView * mvpUBO.model * vec4(inPosition, 1.0); 
}