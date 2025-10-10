#version 450

// layout(set = 0, binding = 0) uniform UniformBufferObject {
// 	mat4 model;
// 	mat4 mainCameraProj;
// 	mat4 mainCameraView;
// 	mat4 padding0;
// 	mat4 padding1;
// 	mat4 padding2;
// 	mat4 padding3;
// 	mat4 padding4; 
// } mvpUBO;

layout(set = 0, binding = 1) uniform TextUniformBufferObject {
	mat4 model;
	mat4 mainCameraProj;
	mat4 mainCameraView;
	mat4 padding0;
	mat4 padding1;
	mat4 padding2;
	mat4 padding3;
	mat4 padding4; 
} textMvpUBO;

//legacy code: (vertex buffer layout without instance data)
// layout(location = 0) in vec3 inPosition;
// layout(location = 1) in vec3 inColor;//no use
// layout(location = 2) in vec2 inTexCoord;
// layout(location = 3) in vec3 inNormal;//no use

//vertex buffer layout with instance data, for text presentation
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec2 inOffset; //from instance buffer
layout(location = 3) in vec3 inColor; //from instance buffer
layout(location = 4) in vec4 inUVRect; //from instance buffer
layout(location = 5) in vec2 inScale; //from instance buffer

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
   // gl_Position = mvpUBO.mainCameraProj * mvpUBO.mainCameraView * mvpUBO.model * vec4(inPosition+inOffset, 0.0, 1.0);

	gl_Position = textMvpUBO.mainCameraProj * textMvpUBO.mainCameraView * textMvpUBO.model * vec4(inPosition * inScale + inOffset, 0.0, 1.0);

    fragColor = inColor;

	//fragTexCoord = inUV;// * vec2(3.5, 10.1);
	fragTexCoord = inUVRect.xy + inUVRect.zw * inUV; //map the quad UV to a specific char UV in the atlas
}
