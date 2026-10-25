#version 460
#include "../CommonShaders/constants.glsl"
#include "../CommonShaders/lightsUBO.glsl"

#extension GL_EXT_ray_query : require
#extension GL_EXT_ray_tracing : require

layout (set = 1, binding = UNIFORM_TEXTURE_SAMPLER_BINDING) uniform sampler2D texSampler;

layout (set = 0, binding = 4) uniform accelerationStructureEXT topLevelAS;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;//inColor is not used here	
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inPosWorld;

layout (location = 0) out vec4 outColor;

bool isOccluded(vec3 origin, vec3 direction, float tMin, float tMax){
    rayQueryEXT query;

    rayQueryInitializeEXT(
        query,
        topLevelAS,
        gl_RayFlagsOpaqueEXT |
        gl_RayFlagsTerminateOnFirstHitEXT,
        0xFF,              // instance mask
        origin,
        tMin,
        direction,         // 调用方传入归一化方向
        tMax
    );

    //rayQueryProceedEXT() 做什么？
    //每次调用，它会继续遍历加速结构，直到出现以下一种情况：
    //返回 true：找到了需要 shader 处理的候选交点，暂停遍历，进入循环体。例如非 opaque 三角形，需要你做 alpha test，再决定是否确认命中。
    //返回 false：查询已经结束，退出循环。此时再读取最终的 committed intersection。
    //对于其中的不透明三角形，查询会自动确认命中；再加上 TerminateOnFirstHit，确认第一个命中后就结束，不需要你在循环体中处理。
    //保留 while 是更通用的写法，方便以后在循环体里增加 alpha-test 或程序化几何的候选处理；
    while (rayQueryProceedEXT(query)) {
        // 强制视为 opaque 的三角形，无需手动确认 candidate。
    }

    //gl_RayQueryCommittedIntersectionNoneEXT 表示“没有已确认的交点”。所以 != None 就表示存在已确认命中。
    return rayQueryGetIntersectionTypeEXT(query, true)
        != gl_RayQueryCommittedIntersectionNoneEXT;
}

void main() {
	vec4 tex = texture(texSampler, inTexCoord);
	//color = vec3(mix(tex.xyz, vec3(dot(vec3(0.2126,0.7152,0.0722), tex.xyz)), 0.65));	//Desaturate tex color
	vec3 N = normalize(inNormal); //N:物体在世界坐标系中的法线

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


	//ray query to generate shadow
	//inPosWorld:物体在世界坐标系中的位置

	vec3 toLight = lightsUBO.lights[0].lightPos.xyz - inPosWorld;
	float distanceToLight = length(toLight);

	//从“当前着色点指向光源”的 阴影光线（shadow ray）。
	//它用来检查：这个点到光源之间有没有东西挡住。
	//命中不透明遮挡物就意味着这条直接光照路径不通。
	bool shadowed = isOccluded(
		inPosWorld,
		toLight / distanceToLight,
		0.001,                       // 示例值，需按场景尺度调整
		distanceToLight//10000.0
	);

	if (shadowed) outColor = vec4(0.0, 0.0, 0.0, 1.0);
	//else outColor = vec4(0.0, 0.0, 1.0, 1.0); //test

}