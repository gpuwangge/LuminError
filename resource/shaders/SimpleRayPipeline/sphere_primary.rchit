#version 460
#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;
layout(location = 1) rayPayloadEXT ShadowPayload shadowPayload;
hitAttributeEXT vec2 attribs;
//hitAttributeEXT vec3 hitNormal;

layout(set = 0, binding = 1) uniform accelerationStructureEXT topLevelAS;

struct Material {
    vec3 albedo;
    vec3 emissionColor;
    vec3 transmissionColor;
    float metallic;
    float roughness;
    float alpha;
    float emissionStrength;
    float reflectance;
    float specular;
    float ior;
    float transmission;
};
layout(set = 0, binding = 5, std430) readonly buffer SBOMaterial {
   Material materials[];
} sboMaterial;

layout(set = 0, binding = 7) uniform CustomBufferObject {
    int frameCount;
    bool cameraInMotion;
    uint lightCount;
    uint materialCount;
} customUBO;

struct RtLightInfo{
    vec4 position;
    vec4 color;
    vec4 direction;
    float intensity;
    float radius;
    float angle;
    float type;
};  //total size: 16+16+16+4*4=64 bytes
const int RTLIGHT_SIZE = 64;//assume max 64 rt lights for now
layout(set = 0, binding = 8, std430) readonly buffer SBORtLightBuffer {
    RtLightInfo lights[RTLIGHT_SIZE];
} sboRtLightBuffer;

/*结构
0.早退判断
1.命中信息重建
2.硬阴影：对light循环，面光源和软阴影。包含直接漫反射和高光。
3.二次光线：镜面反射，折射，用于镜面和玻璃
4.写回payload
（最终看到的效果要包含如下部分：漫反射Lambert，镜面高光项Phong/Blinn，阴影shadowray）
*/

/**************
Untility functions
**************/

bool isDirectionalLight(RtLightInfo light){
    return false;
    return light.type < 0.5;
}

bool isPointLight(RtLightInfo light){
    return true;
    return light.type >= 0.5 && light.type < 1.5;
}

bool isSpotLight(RtLightInfo light){
    return false;
    return light.type >= 1.5 && light.type < 2.5;
}


vec3 getWorldHitPos(){
    return gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;
}

vec3 getObjectHitPos(){
    return gl_ObjectRayOriginEXT + gl_HitTEXT * gl_ObjectRayDirectionEXT;
}

vec3 getSphereWorldNormal(){
    // 假设 sphere 在 object space 下球心为 (0,0,0)
    vec3 objHitPos = getObjectHitPos();
    vec3 objN = safeNormalize(objHitPos);

    // 如果只有均匀缩放/旋转/平移，这样基本可用
    vec3 worldN = normalize((gl_ObjectToWorldEXT * vec4(objN, 0.0)).xyz);
    return worldN;
}

vec3 getLightDirAndRadiance(
    RtLightInfo light,
    vec3 hitPos,
    out float maxT,
    out vec3 radiance
){
    vec3 L;

    if(isDirectionalLight(light)){
        L = safeNormalize(-light.direction.xyz);
        maxT = 1e32;
        radiance = light.color.rgb * light.intensity;
        return L;
    }

    vec3 toLight = light.position.xyz - hitPos;
    float dist2 = max(dot(toLight, toLight), 1e-6);
    float dist = sqrt(dist2);
    L = toLight / dist;
    maxT = dist - SHADOW_BIAS;

    float attenuation = 1.0 / max(dist2, 1e-6);

    if(isSpotLight(light)){
        vec3 spotDir = safeNormalize(-light.direction.xyz);
        float cosTheta = dot(L, spotDir);
        float cosOuter = cos(light.angle);
        float spotFactor = smoothstep(cosOuter, min(1.0, cosOuter + 0.05), cosTheta);
        attenuation *= spotFactor;
    }

    radiance = light.color.rgb * light.intensity * attenuation;
    return L;
}

float traceShadowVisibility(vec3 origin, vec3 dir, float tMax){
    shadowPayload.visibility = 0u;

    uint flags =
        gl_RayFlagsTerminateOnFirstHitEXT |
        //gl_RayFlagsOpaqueEXT | //临时去掉
        gl_RayFlagsSkipClosestHitShaderEXT;

    traceRayEXT(
        topLevelAS,
        flags,
        0xFF,
        0, 0, 1,   // missIndex = 1，假设你的 shadow miss 在 index 1
        origin,
        SHADOW_BIAS,
        dir,
        tMax,
        1          // payload location = 1
    );

    return float(shadowPayload.visibility);
}
void main(){

    /**************
    0.早退判断
    **************/
    uint materialIndex = uint(gl_InstanceCustomIndexEXT);

    //if(materialIndex < 0 || materialIndex >= uint(customUBO.materialCount)){//customUBO.materialCount is buggy
    if(materialIndex < 0){
        primaryPayload.radiance = vec3(1.0, 0.0, 1.0);
        primaryPayload.done = 1u;
        return;
    }

    Material mat = sboMaterial.materials[materialIndex];

    // 如果 alpha 表示完全不可见，建议透传，而不是直接终止
    if(mat.alpha <= 0.001){
        primaryPayload.radiance = vec3(0.0);
        primaryPayload.nextOrigin = getWorldHitPos() + safeNormalize(gl_WorldRayDirectionEXT) * EPSILON;
        primaryPayload.nextDir = safeNormalize(gl_WorldRayDirectionEXT);
        primaryPayload.done = 0u;
        return;
    }

    /**************
    1.命中信息重建
    **************/
    vec3 hitPos = getWorldHitPos();
    vec3 Ngeom = getSphereWorldNormal();
    //vec3 Ngeom = normalize((gl_ObjectToWorldEXT * vec4(hitNormal, 0.0)).xyz);//test
    vec3 I = safeNormalize(gl_WorldRayDirectionEXT); // 入射方向：射线前进方向

    //test
    // vec3 objHitPos = getObjectHitPos();
    // // primaryPayload.radiance = objHitPos * 0.5 + vec3(0.5);
    // primaryPayload.radiance = Ngeom * 0.5 + vec3(0.5);
    // primaryPayload.done = 1u;
    // return;

    //test
    // primaryPayload.radiance = I * 0.5 + vec3(0.5);
    // primaryPayload.done = 1u;
    // return;

    //test
    // float d = dot(I, Ngeom);
    // primaryPayload.radiance = vec3(d * 0.5 + 0.5);
    // primaryPayload.done = 1u;
    // return;

    //test: 所有的球都变成绿色了
    //说明 dot(I, Ngeom) 对所有命中都 < 0
    // float d = dot(I, Ngeom);
    // primaryPayload.radiance = vec3(
    //     d > 0.0 ? 1.0 : 0.0,
    //     d < 0.0 ? 1.0 : 0.0,
    //     0.0);
    // primaryPayload.done = 1u;
    // return;


    vec3 V = -I;

    bool frontFace = dot(I, Ngeom) < 0.0;
    vec3 N = frontFace ? Ngeom : -Ngeom; // N 始终朝向入射光
    //N = Ngeom;//test

    vec3 albedo = mat.albedo;
    vec3 emission = mat.emissionColor * mat.emissionStrength;

    float metallic = clamp(mat.metallic, 0.0, 1.0);
    float roughness = clamp(mat.roughness, 0.02, 1.0);
    float transmission = clamp(mat.transmission, 0.0, 1.0);
    float specular = clamp(mat.specular, 0.0, 1.0);
    float ior = max(mat.ior, 1.01);

    float f0Scalar = pow((1.0 - ior) / (1.0 + ior), 2.0);
    vec3 dielectricF0 = vec3(f0Scalar);
    vec3 F0 = mix(dielectricF0, albedo, metallic);

    /**************
    2.直接光 + 阴影
    **************/
    vec3 directDiffuse = vec3(0.0);
    vec3 directSpecular = vec3(0.0);

    uint lightNum = min(customUBO.lightCount, uint(RTLIGHT_SIZE));

    for(uint i = 0u; i < lightNum; ++i){
        RtLightInfo light = sboRtLightBuffer.lights[i];

        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitPos, maxT, lightRadiance);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL <= 0.0) continue;


        vec3 shadowOrigin = hitPos + N * SHADOW_BIAS;
        float visibility = traceShadowVisibility(shadowOrigin, L, maxT);

        //if(visibility <= 0.0) continue;
        visibility = 1.0;//test, disable shadow

        //test
        // primaryPayload.radiance = vec3(visibility);
        // primaryPayload.done = 1u;
        // return;

        // vec3 kd = (1.0 - metallic) * albedo;
        // vec3 diffuseBRDF = kd / PI;
        // // diffuse 抑制：为了玻璃材质
        // kd *= (1.0 - transmission);
        // kd *= mat.alpha;

        vec3 kd = (1.0 - metallic) * albedo;
        kd *= (1.0 - transmission);
        kd *= mat.alpha;
        vec3 diffuseBRDF = kd / PI;


        vec3 H = safeNormalize(L + V);
        float NdotH = max(dot(N, H), 0.0);
        float VdotH = max(dot(V, H), 0.0);

        vec3 F = fresnelSchlick(VdotH, F0);

        //这是 Blinn-Phong。
        float shininess = mix(128.0, 4.0, roughness);
        float specFactor = pow(NdotH, shininess) * specular;
        directDiffuse += diffuseBRDF * lightRadiance * NdotL * visibility;
        directSpecular += F * specFactor * lightRadiance * NdotL * visibility;


        //这是Cook-Torrance Specular (buggy)
        // float a = roughness * roughness;
        // float a2 = a * a;

        // float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
        // float D = a2 / (PI * denom * denom);

        // float k = (roughness + 1.0);
        // k = (k * k) / 8.0;

        // float Gv = NdotL / (NdotL * (1.0 - k) + k);
        // float Gl = max(dot(N, V), 0.0) /
        //         (max(dot(N, V),0.0) * (1.0 - k) + k);

        // float G = Gv * Gl;

        // vec3 specBRDF =
        //     D * G * F /
        //     max(4.0 * NdotL * max(dot(N,V),0.0), 1e-4);

        // directSpecular +=
        //     specBRDF *
        //     lightRadiance *
        //     NdotL *
        //     visibility;
    }

    //vec3 localRadiance = emission + directDiffuse + directSpecular;

    //test
    // vec3 localRadiance = emission;
    // // 对 transmission 材质，先不算 directDiffuse / directSpecular
    // if(transmission < 0.01){
    //     localRadiance += directDiffuse + directSpecular;
    // }

    //test
    vec3 localRadiance = emission + directSpecular;
    if(transmission < 0.01){
        localRadiance += directDiffuse;
    }

    //test
    // primaryPayload.radiance = emission + directDiffuse + directSpecular;
    // primaryPayload.done = 1u;
    // return;

    /**************
    3. 二次光线
    **************/
    vec3 nextOrigin = vec3(0.0);
    vec3 nextDir = vec3(0.0);
    vec3 nextThroughputMul = vec3(1.0);
    bool spawnNextRay = false;

    float cosTheta = clamp(dot(V, N), 0.0, 1.0);
    float fresnelScalar = fresnelSchlickScalar(cosTheta, ior);
    vec3 F = fresnelSchlick(cosTheta, F0);

    bool hasReflection = (specular > 0.01 || metallic > 0.01 || mat.reflectance > 0.01);
    bool hasTransmission = transmission > 0.01; //recover

    if(hasTransmission){
        float eta = frontFace ? (1.0 / ior) : ior;

        //test
        // primaryPayload.radiance = frontFace
        //     ? vec3(0.0, 1.0, 0.0)   // 外表面 = 绿色
        //     : vec3(1.0, 0.0, 0.0);  // 内表面 = 红色
        // primaryPayload.done = 1u;
        // return;

        vec3 T = refract(I, N, eta);
        bool tir = dot(T, T) < 1e-8;

        //test 折射变彩球
        // primaryPayload.radiance = abs(T);
        // primaryPayload.done = 1u;
        // return;


        if(tir){
            vec3 R = reflect(I, N);
            //nextOrigin = hitPos + N * SHADOW_BIAS;
            nextDir    = safeNormalize(R); 
            nextOrigin = hitPos + nextDir * EPSILON;//test
            nextThroughputMul = mix(vec3(specular), albedo, metallic);
            spawnNextRay = true;
        }else{
            // 反射/折射可按 Fresnel 概率做 Russian roulette；
            // 如果你现在只走一条路径，建议先固定优先折射，别用 fresnel>0.5 这种硬阈值
            //nextOrigin = hitPos - N * SHADOW_BIAS;
            nextDir = safeNormalize(T);
            nextOrigin = hitPos + nextDir * EPSILON;//test
            //nextOrigin = hitPos + T * 0.01;//test
            
            nextThroughputMul = mat.transmissionColor * transmission * (1.0 - fresnelScalar); //这是给玻璃用的
            //nextThroughputMul = vec3(100.0);//test
            spawnNextRay = true;
        }
    }
    else if(hasReflection){
        vec3 R = reflect(I, N);
        nextDir = safeNormalize(R);
        //nextOrigin = hitPos + N * SHADOW_BIAS;
        nextOrigin = hitPos + nextDir * EPSILON;//test
        nextThroughputMul = mix(vec3(specular), albedo, metallic) * fresnelScalar;
        spawnNextRay = true;

        // vec3 R = reflect(I, N);
        // nextOrigin = hitPos + R * SHADOW_BIAS;
        // nextDir = safeNormalize(R);
        // //nextThroughputMul = mix(vec3(specular), albedo, metallic) * fresnelScalar;
        // nextThroughputMul = F;
        // spawnNextRay = true;


    }

    /**************
    4. 写回 payload
    注意：这里不要提前把 throughput 乘成“下一跳”的值，
    否则 rgen 若用 throughput * radiance 累加，会把本跳权重搞错
    **************/
    primaryPayload.radiance = localRadiance;

    if(spawnNextRay && luminance(nextThroughputMul) > 1e-4){
        primaryPayload.nextOrigin = nextOrigin;
        primaryPayload.nextDir = nextDir;

        // 关键点：
        // 不要在这里直接 *= nextThroughputMul，
        // 除非你的 rgen 是先 accum 当前 radiance，再更新 throughput。
        // 如果你当前 rgen 不是这样，这里先改成“由 rgen 负责乘”。
        primaryPayload.done = 0u;

        // 如果你暂时又不想改 rgen，可以保留这一行，但必须调整 rgen 顺序
        primaryPayload.throughput *= nextThroughputMul;
    }
    else{
        primaryPayload.done = 1u;
    }
}