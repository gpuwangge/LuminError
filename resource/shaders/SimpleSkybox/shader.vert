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
layout(location = 1) in vec3 inColor; //no use
layout(location = 2) in vec2 inTexCoord; //no use
layout(location = 3) in vec3 inNormal; //no use

//layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 fragTexCoord;
layout(location = 0) out vec3 outPos;

void main() {
	mat4 view = mat4(mat3(mvpUBO.mainCameraView)); //remove translation of skybox
    vec4 pos = mvpUBO.mainCameraProj * view * mvpUBO.model * vec4(inPosition, 1.0);
    gl_Position = pos.xyww; //cheat in depth test
    outPos = inPosition; //pass input position to fragment for cubemap sampler
}