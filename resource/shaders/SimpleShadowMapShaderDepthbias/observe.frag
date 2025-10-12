#version 450
#define LIGHT_MAX 64

//layout (set = 0, binding = 3) uniform sampler2D depthSampler; //single sample
layout (set = 0, binding = 2) uniform sampler2DMS depthSampler; //msaa
layout (set = 0, binding = 3) uniform sampler2DMS lightDepthSampler; //msaa

layout (location = 0) in vec2 inTexCoord;
layout (location = 0) out vec4 outColor;

void main() {
	// vec2 screenSize = vec2(800,800); //assume the window is 800x800. if render to a smaller quad that doesnt take up all the window, still works but show part of the depth image
	// vec2 texCoords = gl_FragCoord.xy / screenSize; //gl_FragCoord.xy is screen pixel coords
	// vec4 color = texelFetch(depthSampler, ivec2(texCoords * textureSize(depthSampler)), 7); 

	//float depthValue = texture(depthSampler, inTexCoord).r; //single sample
	

	//vec4 color = texelFetch(depthSampler, ivec2(inTexCoord * textureSize(depthSampler)), 7); //msaa
	vec4 color = texelFetch(lightDepthSampler, ivec2(inTexCoord * textureSize(lightDepthSampler)), 7); //msaa
	float depthValue = color.r;

	depthValue = pow(depthValue, 50.0);
	outColor = vec4(vec3(depthValue), 1.0);
}