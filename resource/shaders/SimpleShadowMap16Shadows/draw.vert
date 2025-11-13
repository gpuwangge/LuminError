#version 450
#include "../Common/constants.glsl"
#include "../Common/graphicsGlobalUBO.glsl"

layout(set = 0, binding = UNIFORM_OBJECT_BINDING) uniform StructObjectBuffer {
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

	// outShadowCoord = (biasMat * customUBO.light Space * ubo.model ) * vec4(inPosition, 1.0);

	gl_Position = graphicsGlobalUBO.mainCameraProj * graphicsGlobalUBO.mainCameraView * objectUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(objectUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = objectUBO.model * vec4(inPosition, 1.0);

	//outFragPosLightSpace = objectUBO.lightCameraProj * objectUBO.lightCameraView * objectUBO.model * vec4(inPosition, 1.0); 
	//outFragPosLightSpace = objectUBO.lightCameraView * objectUBO.model * vec4(inPosition, 1.0); 
}