#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    //mat4 model;
    mat4 view;
    mat4 proj;
} vpUBO;

//The maximum push constant per shader is 128b~256b. One 4x4 matrix takes 4x4x4=64 bytes
layout(push_constant) uniform PushConstant{
    mat4 model;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal; //normal is not used here

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = vpUBO.proj * vpUBO.view * pc.model * vec4(inPosition, 1.0); //vpUBO.model
    fragColor = inColor;
	fragTexCoord = inTexCoord;
}