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
layout(location = 1) in vec3 inColor;   //no use
layout(location = 2) in vec2 inTexCoord; //no use
layout(location = 3) in vec3 inNormal; 
//layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 fragTexCoord;
//layout(location = 0) out vec3 pos;
//layout(location = 1) out vec3 normal;

// layout (location = 0) out vec3 outPos;
// layout (location = 1) out vec3 outNormal_environmentmap;
// layout (location = 2) out vec3 outNormal_shadowmap;
// layout (location = 3) out vec3 outViewVec;
// layout (location = 4) out vec3 outLightVec;
// layout (location = 5) out vec4 outShadowCoord;
// layout (location = 6) out vec2 outTexCoord;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outTexCoord;
layout (location = 3) out vec3 outPosWorld;

/**********for environmentmap**********/
//NA

/**********for shadowmap**********/
//biasMat is to prevent shadwo acne
const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

void main() {
    ///gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    ///outTexCoord = inTexCoord;

    /**********for environmentmap**********/
    //fragColor = inColor;
    //fragTexCoord = inTexCoord;
    ///outPos = vec3(ubo.model * vec4(inPosition, 1.0f));
    ///outNormal_environmentmap = mat3(transpose(inverse(ubo.model))) * inNormal;

    /**********for shadowmap**********/
	//outNormal_shadowmap = mat3(ubo.model) * inNormal;
	//outLightVec = normalize(customUBO.lightPos - inPosition.xyz);
	//outViewVec = -outPos.xyz;		
	//outShadowCoord = (biasMat * customUBO.light Space * ubo.model ) * vec4(inPosition, 1.0);

    /**********for phong Lighting**********/
	// outNormal_shadowmap = inNormal; //mat3(ubo.model) * inNormal;
	// vec3 lPos = customUBO.lightPos; //mat3(ubo.model) * customUBO.lightPos;
	// outLightVec = lPos - outPos.xyz;
	// outViewVec = -outPos.xyz;		

    gl_Position = mvpUBO.mainCameraProj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition, 1.0);

    outNormal = mat3(mvpUBO.model) * inNormal;
	outColor = inColor;
	outTexCoord = inTexCoord;
	outPosWorld = vec3(mvpUBO.model * vec4(inPosition, 1.0));
}