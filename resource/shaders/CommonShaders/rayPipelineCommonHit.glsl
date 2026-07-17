#ifndef RAY_COMMONHIT_GLSL
#define RAY_COMMONHIT_GLSL

#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;
layout(location = 1) rayPayloadEXT ShadowPayload shadowPayload;
layout(set = 0, binding = 1) uniform accelerationStructureEXT topLevelAS;

layout(set = 0, binding = 3, std430) readonly buffer SBOMaterial {
   Material materials[];
} sboMaterial;

layout(set = 0, binding = 5) uniform CustomBufferObject {
    int frameCount;
    bool cameraInMotion;
    uint lightCount;
    uint materialCount;
} customUBO;

const int RTLIGHT_SIZE = 64;//assume max 64 rt lights for now
layout(set = 0, binding = 6, std430) readonly buffer SBORtLightBuffer {
    RtLightInfo lights[RTLIGHT_SIZE];
} sboRtLightBuffer;

layout(set = 0, binding = 7, std430) readonly buffer SBOInstance {
   InstanceInfo instances[];
} sboInstance;

/**************
Untility functions
**************/
vec3 getWorldHitPos(){
    return gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;
}

vec3 getObjectHitPos(){
    return gl_ObjectRayOriginEXT + gl_HitTEXT * gl_ObjectRayDirectionEXT;
}

vec3 safeNormalize(vec3 v){
    float len2 = dot(v, v);
    if(len2 < 1e-12) return vec3(0.0, 0.0, 1.0);
    return v * inversesqrt(len2);
}

uint WangHash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

float Rand(inout uint state) {
    return float(WangHash(state)) / 4294967296.0;
}

vec3 RandomDirectionInHemisphere(vec3 normal, inout uint state){
    float u1 = Rand(state);
    float u2 = Rand(state);

    float r = sqrt(u1);
    float theta = 2.0 * 3.14159265359 * u2;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - u1);

    vec3 tangent = normalize(abs(normal.x) > 0.1 ? cross(normal, vec3(0,1,0)) : cross(normal, vec3(1,0,0)));
    vec3 bitangent = cross(tangent, normal);

    vec3 direction = x * tangent + y * bitangent + z * normal;
    return normalize(direction);
}

/**************
Light functions
**************/
bool isDirectionalLight(RtLightInfo light){
    return false; //disable this function for now
    return light.type < 0.5;
}

bool isPointLight(RtLightInfo light){
    return true; //only implemented point light
    return light.type >= 0.5 && light.type < 1.5;
}

bool isSpotLight(RtLightInfo light){
    return false; //disable this function for now
    return light.type >= 1.5 && light.type < 2.5;
}

vec3 getLightDirAndRadiance(RtLightInfo light, vec3 hitPos, out float maxT, out vec3 radiance){
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

/**************
Shadow functions
**************/
float traceShadowVisibility(vec3 origin, vec3 dir, float tMax){ //inout ShadowPayload shadowPayload inout相当于引用
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

/**************
Core
in是只读，	相当于T&
inout是可读写，	相当于T&
**************/
bool earlyExit(inout Material mat){
    //if(materialIndex < 0 || materialIndex >= uint(customUBO.materialCount)){//customUBO.materialCount is buggy
    // if(materialIndex < 0){
    //     primaryPayload.radiance = vec3(1.0, 0.0, 1.0);
    //     primaryPayload.done = 1u;
    //     return;
    // }

    // 如果 alpha 表示完全不可见，建议透传，而不是直接终止
    if(mat.alpha <= 0.001){
        primaryPayload.radiance = vec3(0.0);
        primaryPayload.nextRayOrigin0 = getWorldHitPos() + safeNormalize(gl_WorldRayDirectionEXT) * EPSILON;
        primaryPayload.nextRayDir0 = safeNormalize(gl_WorldRayDirectionEXT);
        primaryPayload.nextRayOrigin1 = getWorldHitPos() + safeNormalize(gl_WorldRayDirectionEXT) * EPSILON;
        primaryPayload.nextRayDir1 = safeNormalize(gl_WorldRayDirectionEXT);
        primaryPayload.spawnRayCount = 1;//?
        primaryPayload.nextCurrentIOR0 = primaryPayload.currentIOR;
        primaryPayload.nextInsideMedium0 = primaryPayload.insideMedium;
        primaryPayload.nextMediumEntryPos0 = primaryPayload.mediumEntryPos;
        primaryPayload.done = 0u;
        return true;
    }
    return false;
}

/*结构
1.命中信息重建
2.硬阴影：对light循环，面光源和软阴影。包含直接漫反射和高光。
3.二次光线：镜面反射，折射，用于镜面和玻璃
4.写回payload
（最终看到的效果要包含如下部分：漫反射Lambert，镜面高光项Phong/Blinn，阴影shadowray）
*/
void updatePayload(in Material mat, vec3 Ngeom){
    /**************
    1.命中信息重建
    **************/
    vec3 hitPos = getWorldHitPos();
    
    vec3 I = safeNormalize(gl_WorldRayDirectionEXT); // 入射方向：射线前进方向

    vec3 V = -I; //视向向量，观察方向

    bool frontFace = dot(I, Ngeom) < 0.0; //入射光线落在表面的哪一侧（正面还是背面）
    vec3 N = frontFace ? Ngeom : -Ngeom; // N 始终朝向入射光

    bool airToMedium = (primaryPayload.insideMedium == 0u) && !frontFace;
    bool mediumToAir = (primaryPayload.insideMedium == 1u) && frontFace;
    //bool invalid1 = (primaryPayload.insideMedium == 1u && !frontFace);
    //bool invalid2 = (primaryPayload.insideMedium == 0u && frontFace);

    vec3 albedo = mat.albedo;
    vec3 emission = mat.emissionColor * mat.emissionStrength;

    float metallic = clamp(mat.metallic, 0.0, 1.0);
    float roughness = clamp(mat.roughness, 0.02, 1.0);
    float transmission = clamp(mat.transmission, 0.0, 1.0);
    float specular = clamp(mat.specular, 0.0, 1.0);
    float ior = max(mat.ior, 1.01);
    float alpha = mat.alpha;
    vec3 transmissionColor = mat.transmissionColor;

    //float f0Scalar = pow((1.0 - ior) / (1.0 + ior), 2.0);
    //vec3 dielectricF0 = vec3(f0Scalar);
    //vec3 F0 = mix(dielectricF0, albedo, metallic);

    // 计算基础反射率 F0
    //vec3 F0 = mix(vec3(mat.reflectance), albedo, metallic);
    float f0Scalar = pow((1.0 - ior) / (1.0 + ior), 2.0);
    vec3 dielectricF0 = vec3(f0Scalar);
    vec3 F0 = mix(dielectricF0, albedo, metallic);
    // 计算菲涅尔项
    //float cosTheta = abs(dot(Ngeom, -I));
    //float cosTheta = abs(dot(N, -I));
    float cosTheta = clamp(dot(N,-I),0.0,1.0);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    uint state = gl_LaunchIDEXT.x;
    state = state * 747796405u + gl_LaunchIDEXT.y;
    state = state * 747796405u + customUBO.frameCount;
    state = state * 747796405u + 2891336453u;


    /////////////////////
    //Method1： 确定性的 Whitted-style 光追 + 直接光照
    //没有随机分支，稳定
    //INPUT: mat, Ngeom
    //OUTPUT: primaryPayload.throughput, primaryPayload.radiance
    //2.直接光 + 阴影

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
        if(visibility <= 0.0) continue;
        //float visibility = 1.0;//test, disable shadow

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
        //directSpecular += F * specFactor * lightRadiance * NdotL * visibility;
        // if(metallic > 0.9){
        //     directSpecular += albedo * specFactor * lightRadiance * NdotL * visibility;
        // } else {
        //     directSpecular += F * specFactor * lightRadiance * NdotL * visibility;
        // }
        if(metallic > 0.5){
            directDiffuse = vec3(0.0);
            directSpecular += albedo * specFactor * lightRadiance * NdotL * visibility;
        }else {
            directSpecular += F * specFactor * lightRadiance * NdotL * visibility;
        }
    }

    //vec3 localRadiance = emission + directDiffuse + directSpecular;
    vec3 localRadiance = emission;
    if(transmission < 0.01){
        localRadiance += directDiffuse + directSpecular;
    } else {
        localRadiance += directSpecular;
    }

    //3. 二次光线

    vec3 nextOrigin = vec3(0.0);
    vec3 nextDir = vec3(0.0);
    vec3 nextThroughputMul = vec3(1.0);
    bool spawnNextRay = false;

    //float cosTheta = clamp(dot(V, N), 0.0, 1.0);
    float fresnelScalar = fresnelSchlickScalar(cosTheta, ior);
    fresnelScalar = pow(fresnelScalar,0.75);
    //vec3 F = fresnelSchlick(cosTheta, F0);

    //bool hasReflection = (specular > 0.01 || metallic > 0.01 || mat.reflectance > 0.01);
    bool hasReflection = (metallic > 0.8);
    bool hasTransmission = transmission > 0.01; //recover

    primaryPayload.radiance = localRadiance;
    primaryPayload.spawnRayCount = 0u;
    primaryPayload.done = 1u;

    if(hasReflection && transmission < 0.01){ //金属
        vec3 R = normalize(reflect(I, N));

        primaryPayload.nextRayOrigin0 = hitPos + N * EPSILON;
        primaryPayload.nextRayDir0 = normalize(mix(R, RandomDirectionInHemisphere(N, state), roughness * roughness));
        primaryPayload.nextRayThroughputMul0 = mix(vec3(0.04), albedo, metallic);

        primaryPayload.spawnRayCount = 1u;
        primaryPayload.done = 0u;
    }

    if(hasTransmission){
        //float eta = frontFace ? (1.0 / ior) : ior;

        float n1 = primaryPayload.insideMedium == 1u ? mat.ior : 1.0;
        float n2 = primaryPayload.insideMedium == 1u ? 1.0    : mat.ior;
        float eta = n1 / n2;

        vec3 R = safeNormalize(reflect(I, N));
        vec3 T = refract(I, N, eta);

        bool tir = dot(T, T) < 1e-8;

        primaryPayload.nextCurrentIOR0 = primaryPayload.currentIOR;
        primaryPayload.nextInsideMedium0 = primaryPayload.insideMedium;
        primaryPayload.nextMediumEntryPos0 = primaryPayload.mediumEntryPos;
        primaryPayload.nextCurrentIOR1 = primaryPayload.currentIOR;
        primaryPayload.nextInsideMedium1 = primaryPayload.insideMedium;
        primaryPayload.nextMediumEntryPos1 = primaryPayload.mediumEntryPos;
        if(tir){ // 全反射
            //primaryPayload.nextRayOrigin0 = hitPos + R * EPSILON;
            vec3 Roff = dot(R, Ngeom) > 0.0 ? Ngeom : -Ngeom;
            primaryPayload.nextRayOrigin0 = hitPos + Roff * EPSILON;
            primaryPayload.nextRayDir0 = R;
            primaryPayload.nextRayThroughputMul0 = vec3(1.0);
            primaryPayload.spawnRayCount = 1u;
        }
        else{ //有反射和折射
            T = safeNormalize(T);

            vec3 Toff = dot(T, Ngeom) > 0.0 ? Ngeom : -Ngeom;
            vec3 Roff = dot(R, Ngeom) > 0.0 ? Ngeom : -Ngeom;

            float F = pow(fresnelScalar,0.5);

            //折射
            primaryPayload.nextRayOrigin0 = hitPos + Toff * EPSILON;
            primaryPayload.nextRayDir0 = T;
            primaryPayload.nextRayThroughputMul0 = transmissionColor * (1.0 - F);

            if(airToMedium){
                primaryPayload.nextCurrentIOR0 = ior;
                primaryPayload.nextInsideMedium0 = 1u;
                primaryPayload.nextMediumEntryPos0 = hitPos;
            }else if(mediumToAir){
                float distanceInGlass = length(hitPos - primaryPayload.mediumEntryPos);
                
                //test
                //primaryPayload.radiance = vec3(distanceInGlass * 0.5);
                // primaryPayload.radiance = vec3(1,0,0);
                // primaryPayload.spawnRayCount = 0u;
                // primaryPayload.done = 1u;
                // return;
                
                //vec3 sigmaA = vec3(0.12, 0.06, 0.03);
                vec3 sigmaA = vec3(0.03, 0.02, 0.01);
                vec3 beer = exp(-sigmaA * distanceInGlass); //光穿过玻璃走得越远，剩下的能量越少
                primaryPayload.nextRayThroughputMul0 *= beer;

                primaryPayload.nextCurrentIOR0 = 1.0;
                primaryPayload.nextInsideMedium0 = 0u;
                primaryPayload.nextMediumEntryPos0 = vec3(0.0);
            }

            //反射
            primaryPayload.nextRayOrigin1 = hitPos + Roff * EPSILON;
            primaryPayload.nextRayDir1 = R;
            //primaryPayload.nextRayThroughputMul1 = vec3(fresnelScalar);
            //float F = pow(fresnelScalar,0.8);
            primaryPayload.nextRayThroughputMul1 = vec3(F);
            //float reflectionBoost = 2.0;
            //primaryPayload.nextRayThroughputMul1 = vec3(min(fresnelScalar * reflectionBoost,1.0));

            primaryPayload.spawnRayCount = 2u;
        }

        primaryPayload.done = 0u;
    }
}


#endif


