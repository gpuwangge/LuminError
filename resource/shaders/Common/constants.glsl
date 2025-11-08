#ifndef CONSTANTS_GLSL
#define CONSTANTS_GLSL

//const float PI = 3.14159265359;
//const float EPSILON = 1e-5;
#define LIGHT_MAX 64

//Set=0
const int UNIFORM_GLOBAL_BINDING = 0;
const int UNIFORM_OBJECT_BINDING = 1;
const int UNIFORM_TEXT_BINDING = 2;
const int UNIFROM_LIGHT_BINDING = 3;
//uniform_graphics_custom
//uniform_graphics_depth_image_sampler
//uniform_graphics_lightdepth_image_sampler
//uniform_graphics_lightdepth_image_sampler_hardware


//Set=1
const int UNIFORM_TEXTURE_SAMPLER_BINDING = 0;


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


#endif