#ifndef LIGHTSUBO_GLSL
#define LIGHTSUBO_GLSL

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

layout(set = 0, binding = UNIFROM_LIGHT_BINDING) uniform LightsBufferObject { 
	LightAttribute lights[LIGHT_MAX];
	vec4 mainCameraPos;
	int lightNum;
} lightsUBO;

#endif