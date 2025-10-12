#version 450
#define LIGHT_MAX 64 

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

layout(set = 0, binding = 1) uniform UniformLightsBufferObject { 
	LightAttribute lights[LIGHT_MAX];
	vec4 mainCameraPos; 
	int lightNum; //number of lights, max is LIGHT_MAX
} lightsUBO;

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
layout(location = 1) in vec3 inColor; //not used
layout(location = 2) in vec2 inTexCoord; //not used
layout(location = 3) in vec3 inNormal; //not used

void main() {
	//gl_Position = lightsUBO.mainCameraProj * lightsUBO.lightCameraView * mvpUBO.model * vec4(inPosition, 1.0); //use light camera view to generate light depth image
	gl_Position = lightsUBO.lights[0].lightCameraProj * lightsUBO.lights[0].lightCameraView * mvpUBO.model * vec4(inPosition, 1.0);
}