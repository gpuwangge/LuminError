#version 450 core
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

layout(set = 0, binding = 1) uniform lightsBufferObject { 
	LightAttribute lights[LIGHT_MAX];
	vec4 mainCameraPos;
	int lightNum; //number of lights, max is LIGHT_MAX
} lightsUBO;

//layout(location = 0) in vec3 fragColor;
//layout(location = 1) in vec2 fragTexCoord;
//layout(location = 0) in vec3 pos;
//layout(location = 1) in vec3 normal;

// layout (location = 0) in vec3 inPos;
// layout (location = 1) in vec3 inNormal_environmentmap;
// layout (location = 2) in vec3 inNormal_shadowmap;
// layout (location = 3) in vec3 inViewVec;
// layout (location = 4) in vec3 inLightVec;
// layout (location = 5) in vec4 inShadowCoord;
// layout (location = 6) in vec2 inTexCoord;
// layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inPosWorld;

layout (location = 0) out vec4 outColor;

/**********for environmentmap**********/
//layout (set = 1, binding = 0) uniform samplerCube cubemapSampler;
layout (set = 1, binding = 0) uniform sampler2D texSampler;

/**********for shadowmap**********/
#define AMBIENT 0.1
float textureProj(vec4 shadowCoord, vec2 off){
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture( texSampler, shadowCoord.st + off ).r; //TODO: add texSampler
        //float dist = texture( cubemapSampler, vec3(0,0,0) ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z ) 
		{
			shadow = AMBIENT;
		}
	}
	return shadow;
}
float filterPCF(vec4 sc){
	ivec2 texDim = textureSize(texSampler, 0);
    //ivec2 texDim = textureSize(cubemapSampler, 0); //TODO: add texSampler
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx*x, dy*y));
			count++;
		}
	
	}
	return shadowFactor / count;
}

void main() {
    /**********for environmentmap**********/
    ///vec3 I = normalize(inPos - customUBO.cameraPos);
    //vec3 R = reflect(I, normalize(normal)); //for reflect
    ///float ratio = 1.00 / 1.52; 
    ///vec3 Ref = refract(I, normalize(inNormal_environmentmap), ratio);//for refraction
    //outColor = texture(cubemapSampler, R);

    /**********for shadowmap**********/
	//float shadow = textureProj(inShadowCoord / inShadowCoord.w, vec2(0.0));

	//vec3 N = normalize(inNormal_shadowmap);
	//vec3 L = normalize(inLightVec);
	//vec3 V = normalize(inViewVec);
	//vec3 R = normalize(-reflect(L, N));
	//vec3 diffuse = max(dot(N, L), ambient) * texture(cubemapSampler, Ref).xyz;

    //outColor = vec4(diffuse * shadow, 1.0);

    /**********for phong Lighting**********/
    //vec3 color = texture(cubemapSampler, Ref).xyz;
	// vec4 tex = texture(texSampler, inTexCoord);
	// vec3 color = tex.xyz;
	// vec3 ambientVec = color * vec3(0.1f);
	// vec3 N = normalize(inNormal_shadowmap);
	// vec3 L = normalize(inLightVec);
	// vec3 V = normalize(inViewVec);
	// vec3 R = reflect(-L, N);
	// vec3 diffuse = max(dot(N, L), 0.0) * color;
	// vec3 specular = pow(max(dot(R, V), 0.0), 32.0) * vec3(0.35);
	// outColor = vec4(ambientVec + diffuse * 0.5f + specular, 1.0); 

	vec4 tex = texture(texSampler, inTexCoord);
	//color = vec3(mix(tex.xyz, vec3(dot(vec3(0.2126,0.7152,0.0722), tex.xyz)), 0.65));	//Desaturate tex color
	vec3 N = normalize(inNormal);

	outColor = vec4(0,0,0,0);
	for(int i = 0; i < lightsUBO.lightNum; i++){
		vec3 viewVec = lightsUBO.mainCameraPos.xyz - inPosWorld;		
		vec3 lightVec = lightsUBO.lights[i].lightPos.xyz - inPosWorld;
		float ambientIntensity = lightsUBO.lights[i].ambientIntensity * lightsUBO.lights[i].dimmerSwitch;
		float diffuseIntensity = lightsUBO.lights[i].diffuseIntensity * lightsUBO.lights[i].dimmerSwitch;
		float specularIntensity = lightsUBO.lights[i].specularIntensity * lightsUBO.lights[i].dimmerSwitch;

		vec3 L = normalize(lightVec); //point from pos to light
		vec3 V = normalize(viewVec); //point from pos to camera
		vec3 R = reflect(-L, N); //calculate from L
		float distCoff = pow(lightVec.x, 2) + pow(lightVec.y, 2) + pow(lightVec.z, 2);
		vec3 ambient = tex.xyz / distCoff;
		vec3 diffuse = max(dot(N, L), 0.0) * tex.xyz / distCoff; 
		vec3 specular = pow(max(dot(R, V), 0.0), 32.0) * vec3(0.35) / distCoff;

		outColor += vec4(ambient * ambientIntensity + diffuse * diffuseIntensity + specular * specularIntensity, 0.0);
	}

}