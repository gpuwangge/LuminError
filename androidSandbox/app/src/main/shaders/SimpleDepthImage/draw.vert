#version 450

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

//biasMat is to prevent shadwo acne
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

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

	gl_Position = mvpUBO.mainCameraProj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);

	outNormal = mat3(mvpUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = vec3(mvpUBO.model * vec4(inPosition, 1.0));

}