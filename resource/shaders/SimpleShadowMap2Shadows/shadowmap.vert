#version 450
#define LIGHT_MAX 64 

layout(push_constant) uniform PushConstant{
    int value; //the index of shadowmap renderpass
} pc;

struct LightAttribute{
	mat4 lightCameraProj;
    mat4 lightCameraView;
	vec4 lightPos;
	vec4 lightDir;
    vec4 lightColor; //RGBA
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float dimmerSwitch;
	float spotInnerAngle;
    float spotOuterAngle;
};

layout(set = 0, binding = 1) uniform MVPBufferObject {
    mat4 model;
    bool identityCameraProj;
    bool identityCameraView;
    bool padding_bool[2];
    vec4 padding[11];
} mvpUBO;

layout(set = 0, binding = 3) uniform UniformLightsBufferObject { 
	LightAttribute lights[LIGHT_MAX];
	vec4 mainCameraPos; 
	int lightNum; //number of lights, max is LIGHT_MAX
} lightsUBO;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; //not used
layout(location = 2) in vec2 inTexCoord; //not used
layout(location = 3) in vec3 inNormal; //not used

void main() {
	gl_Position = lightsUBO.lights[pc.value].lightCameraProj * lightsUBO.lights[pc.value].lightCameraView * mvpUBO.model * vec4(inPosition, 1.0);
}