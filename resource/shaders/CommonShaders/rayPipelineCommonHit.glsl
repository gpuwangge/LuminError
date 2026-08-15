#ifndef RAY_COMMONHIT_GLSL
#define RAY_COMMONHIT_GLSL

#include "../CommonShaders/rayPipelineCommonUBO.glsl"

#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT PrimaryPayloadStruct primaryPayload;
layout(location = 1) rayPayloadEXT ShadowPayloadStruct shadowPayload;
layout(set = 0, binding = 1) uniform accelerationStructureEXT topLevelAS;

const int MATERIAL_SIZE = 64;//assume max 64 materials for now
layout(set = 0, binding = 3) uniform MaterialUniformInfo {
   MaterialStruct materials[MATERIAL_SIZE];
} materialUBO;

const int RTLIGHT_SIZE = 64;//assume max 64 rt lights for now
layout(set = 0, binding = 6) uniform RtLightUniformInfo {
    RtLightStruct lights[RTLIGHT_SIZE];
} rtLightUBO;

const int INSTANCE_SIZE = 256;//assume max 256 instances for now
layout(set = 0, binding = 7) uniform InstanceUniformInfo {
   InstanceStruct instances[INSTANCE_SIZE];
} instanceUBO;

#ifndef DISABLE_TEXTURE
const uint MAX_GLOBAL_TEXTURES = 69u;
layout(set = 0, binding = 10) uniform sampler2D texarray[MAX_GLOBAL_TEXTURES];
//texture(texarray[texId], uv);
#endif

struct HitInfoStruct{
    vec3 hitPos;
    vec3 V; //视向向量，观察方向
    vec3 I; // 入射方向：射线前进方向
    vec3 N; // N 始终朝向入射光
    vec3 Ngeom;
    float cosTheta;

    bool airToMedium;
    bool mediumToAir;

    //material related
    uint material_type;
    vec3 albedo;
    vec3 emission;
    float metallic;
    float roughness;
    float transmission;
    float specular;
    float ior;
    float alpha;
    vec3 transmissionColor;

    vec3 F0;
    vec3 F;

    uint state;
};

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

    //Cosine-weighted Hemisphere Sampling（Malley's Method）
    float r = sqrt(u1);
    float theta = 2.0 * 3.14159265359 * u2;
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(1.0 - u1);

    vec3 tangent = normalize(abs(normal.x) > 0.1 ? cross(normal, vec3(0,1,0)) : cross(normal, vec3(1,0,0)));
    vec3 bitangent = cross(tangent, normal);

    vec3 direction = x * tangent + y * bitangent + z * normal;
    
    // float cosTheta = max(dot(normal, direction), 0.0);
    // pdf = cosTheta / PI;

    return normalize(direction);
}

/**************
Light functions
**************/
bool isDirectionalLight(RtLightStruct light){
    return false; //disable this function for now
    return light.type < 0.5;
}

bool isPointLight(RtLightStruct light){
    return true; //only implemented point light
    return light.type >= 0.5 && light.type < 1.5;
}

bool isSpotLight(RtLightStruct light){
    return false; //disable this function for now
    return light.type >= 1.5 && light.type < 2.5;
}

vec3 getLightDirAndRadiance(RtLightStruct light, vec3 hitPos, out float maxT, out vec3 radiance){
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

vec3 SampleDiskLight(RtLightStruct light, vec3 shadingPos, inout uint state){ //for path tracing NEE
    // 光源法线（与你 Whitted 保持一致）
    vec3 lightNormal = normalize(shadingPos - light.position.xyz);

    vec3 T, B;
    buildOrthonormalBasis(lightNormal, T, B);

    // 在单位圆盘随机采样
    vec2 d = sampleDisk(state) * light.radius;

    // 返回圆盘上的一点
    return light.position.xyz +
           T * d.x +
           B * d.y;
}

/**************
Sky function
**************/
vec3 SampleSky(vec3 dir){
    //纯色天空
    //return vec3(0.2, 0.3, 0.4);

    //渐变天空
    dir = normalize(dir);
    float t = 0.5 * (dir.y + 1.0);
    return mix(vec3(1.0),vec3(0.5,0.7,1.0),t);

    //种太阳
    // dir = normalize(dir);
    // float t = 0.5 * (dir.y + 1.0);
    // vec3 sky = mix(vec3(1.0),vec3(0.5,0.7,1.0),t);
    // vec3 sunDir = normalize(vec3(0.3, 0.8, 0.2));
    // float sun = pow(max(dot(dir, sunDir),0.0),512.0);
    // sky += sun * vec3(20.0);
    // return sky;
}

/**************
Texture function
**************/
#ifndef DISABLE_TEXTURE
vec4 SampleTexture(uint texId, vec2 uv){
    texId = min(texId, MAX_GLOBAL_TEXTURES - 1u);
    return texture(texarray[texId], uv);
}
#endif

/**************
Shadow functions
**************/
float traceShadowVisibility(vec3 origin, vec3 dir, float tMax){ //inout ShadowPayload shadowPayload inout相当于引用, for whitted and path tracing(NEE)
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

//soft shadow只对whitted style有效
float traceSoftShadowVisibility(vec3 origin, vec3 hitpos, vec3 N, vec3 lightCenter, float radius, uint sampleCount, uint baseSeed) {
    float visible = 0.0;

    //vec3 lightNormal = normalize(hitpos); // disk faces shading point
    vec3 lightNormal = normalize(origin - lightCenter);
    vec3 T, B;
    buildOrthonormalBasis(lightNormal, T, B);

    for (int s = 0; s < sampleCount; ++s) {
        //uint rng = baseSeed ^ uint(s) * 1664525u + 1013904223u;
        uint rng = baseSeed;
        rng ^= uint(s) * 747796405u;
        rng *= 2891336453u;

        vec2 d = sampleDisk(rng) * radius;
        vec3 samplePos = lightCenter + T * d.x + B * d.y;

        vec3 toLight = samplePos - hitpos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 1e-4);

        float NdotL = dot(N, L);
        if (NdotL <= 0.0) continue;

        shadowPayload.visibility = 0u;

        const float EPS = 0.001;

        traceRayEXT(
            topLevelAS,
            gl_RayFlagsTerminateOnFirstHitEXT |
            gl_RayFlagsSkipClosestHitShaderEXT,
            0xFF,
            1,   // sbtRecordOffset
            1,   // sbtRecordStride
            1,   // missIndex
            origin,
            SHADOW_BIAS,//EPS,
            L,
            max(dist - EPS, EPS),
            1
        );

        visible += (shadowPayload.visibility == 1u) ? 1.0 : 0.0;
    }

    return visible / float(sampleCount);
}

//目前发光材质和NEE最好只enable其中一个，不要一起开，需要进一步测试
vec3 EstimateDirectLightingNEE(in HitInfoStruct hitInfo, inout uint state){ //for path tracing
    //return vec3(20,0,0); //test
    uint lightCount = min(configUBO.lightCount, uint(RTLIGHT_SIZE)); 

    if(configUBO.enableNEE == 0u || lightCount == 0u) return vec3(0.0);

    // 暂时只在具有漫反射分量的表面做 NEE。
    // 金属和玻璃当前使用近似 delta 路径，不在这里处理。
    if(hitInfo.metallic > 0.8 || hitInfo.transmission > 0.01) return vec3(0.0);

    /*
     * 从所有注册光源中随机选择一个。
     *
     * 每个光源被选择的概率：
     * lightPdf = 1 / lightCount
     */
    uint lightIndex = min(uint(Rand(state) * float(lightCount)), lightCount - 1u);

    RtLightStruct light = rtLightUBO.lights[lightIndex];

    vec3 direct = vec3(0.0);
    const int LIGHT_SAMPLES = 1;//4; 1已经足够，因为有frame accumulate
    for(int i = 0; i < LIGHT_SAMPLES; ++i){
        //使用点光源做NEE
        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitInfo.hitPos, maxT, lightRadiance);

        //使用disk light做NEE，效果比较一般，需要调整
        // vec3 samplePos = SampleDiskLight(light, hitInfo.hitPos, state);
        // vec3 toLight = samplePos - hitInfo.hitPos;
        // float dist = length(toLight);
        // vec3 L = toLight / dist;
        // float maxT = dist - SHADOW_BIAS;
        // float dist2 = max(dot(toLight,toLight),1e-6);
        // vec3 lightRadiance = light.color.rgb * light.intensity / dist2;
    
        // vec3 lightNormal = normalize(hitInfo.hitPos - light.position.xyz);  // 光源法线（与你采样时保持一致）
        // float cosThetaLight = max(dot(lightNormal, -L), 1e-4);// 光源面朝向 shading point
        // if (cosThetaLight <= 0.0) continue;
        // float pdfArea = 1.0 / (PI * light.radius * light.radius); // 面积 PDF
        // float pdfSolidAngle = pdfArea * dist2 / cosThetaLight;  // 转成立体角 PDF

        

        float NdotL = max(dot(hitInfo.N, L), 0.0);
        if(NdotL <= 0.0) continue;//return vec3(0.0);

        // 发射 shadow ray，判断采样到的光源是否可见
        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N * SHADOW_BIAS;

        float visibility = traceShadowVisibility(shadowOrigin,L,maxT);

        if(visibility <= 0.0) continue;//return vec3(0.0);
        
        /*
        * Lambert BRDF：
        *
        * f = kd / PI
        *
        * 这里加入：
        * 1. 菲涅尔留下的漫反射能量 (1-F)
        * 2. 非金属部分
        * 3. 非透射部分
        * 4. alpha
        */
        vec3 kd =
            (vec3(1.0) - hitInfo.F) *
            (1.0 - hitInfo.metallic) *
            (1.0 - hitInfo.transmission) *
            hitInfo.alpha *
            hitInfo.albedo;

        vec3 diffuseBRDF = kd / PI;

        /*
        * Monte Carlo estimator：
        *
        * direct =
        *     f * Li * cosTheta * visibility
        *     --------------------------------
        *              lightPdf
        *
        * lightPdf = 1 / lightCount
        *
        * 所以等价于乘以 lightCount。
        */
        float lightPdf = 1.0 / float(lightCount);
        direct += diffuseBRDF * lightRadiance * NdotL * visibility / lightPdf; //使用点光源做NEE
        //return diffuseBRDF * lightRadiance * NdotL * visibility / lightPdf;

        //direct += diffuseBRDF * lightRadiance * NdotL * visibility / pdfSolidAngle; //使用disk light做NEE
    }

    return direct / float(LIGHT_SAMPLES);
}

/**************
Core
in是只读，	相当于T&
inout是可读写，	相当于T&
**************/
//目前没用
//如果以后支持树叶、围栏等 alpha 材质，可以把它升级成 stochastic alpha test，这样它就会真正发挥作用。
// bool earlyExit(inout Material mat){
//     if(mat.alpha < 0.5)
//     {
//         if(Rand(primaryPayload.state) > mat.alpha){// stochastic alpha test
//             return true;
//         }
//     }
//     return false;
// }

void UploadNextRays(in HitInfoStruct hitInfo){
    vec3 nextOrigin = vec3(0.0);
    vec3 nextDir = vec3(0.0);
    vec3 nextThroughputMul = vec3(1.0);
    bool spawnNextRay = false;

    //float cosTheta = clamp(dot(V, N), 0.0, 1.0);
    float fresnelScalar = fresnelSchlickScalar(hitInfo.cosTheta, hitInfo.ior);
    fresnelScalar = pow(fresnelScalar,0.75);
    //vec3 F = fresnelSchlick(cosTheta, F0);

    //bool hasReflection = (specular > 0.01 || metallic > 0.01 || mat.reflectance > 0.01);
    bool hasReflection = (hitInfo.metallic > 0.8);
    bool hasTransmission = hitInfo.transmission > 0.01;

    if(hitInfo.material_type == MATERIAL_GOLD){
    //if(hasReflection && hitInfo.transmission < 0.01){ //金属
        vec3 R = normalize(reflect(hitInfo.I, hitInfo.N));

        primaryPayload.nextRay[0].origin = hitInfo.hitPos + hitInfo.N * EPSILON;
        primaryPayload.nextRay[0].dir = normalize(mix(R, RandomDirectionInHemisphere(hitInfo.N, hitInfo.state), hitInfo.roughness * hitInfo.roughness));
        primaryPayload.nextRay[0].throughputMul = mix(vec3(0.04), hitInfo.albedo, hitInfo.metallic);

        primaryPayload.spawnRayCount = 1u;
        primaryPayload.done = 0u;
    }else if(hitInfo.material_type == MATERIAL_GLASS || hitInfo.material_type == MATERIAL_JADE){
    //if(hasTransmission){
        //float eta = frontFace ? (1.0 / ior) : ior;

        float n1 = primaryPayload.insideMedium == 1u ? hitInfo.ior : 1.0;
        float n2 = primaryPayload.insideMedium == 1u ? 1.0    : hitInfo.ior;
        float eta = n1 / n2;

        vec3 R = safeNormalize(reflect(hitInfo.I, hitInfo.N));
        vec3 T = refract(hitInfo.I, hitInfo.N, eta);

        bool tir = dot(T, T) < 1e-8;

        primaryPayload.nextRay[0].currentIOR = primaryPayload.currentIOR;
        primaryPayload.nextRay[0].insideMedium = primaryPayload.insideMedium;
        primaryPayload.nextRay[0].mediumEntryPos = primaryPayload.mediumEntryPos;
        primaryPayload.nextRay[1].currentIOR = primaryPayload.currentIOR;
        primaryPayload.nextRay[1].insideMedium = primaryPayload.insideMedium;
        primaryPayload.nextRay[1].mediumEntryPos = primaryPayload.mediumEntryPos;
        if(tir){ // 全反射
            //primaryPayload.nextRayOrigin0 = hitPos + R * EPSILON;
            vec3 Roff = dot(R, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;
            primaryPayload.nextRay[0].origin = hitInfo.hitPos + Roff * EPSILON;
            primaryPayload.nextRay[0].dir = R;
            primaryPayload.nextRay[0].throughputMul = vec3(1.0);
            primaryPayload.spawnRayCount = 1u;
        }
        else{ //有反射和折射
            T = safeNormalize(T);
            vec3 Toff = dot(T, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;
            vec3 Roff = dot(R, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;
            float F2 = pow(fresnelScalar,0.5);

            uint rayIndex = 0; //第一条射线，查询折射
            if(primaryPayload.depth < configUBO.maxRefractionDepth ){ //&& Rand(hitInfo.state) < hitInfo.transmission
                primaryPayload.spawnRayCount = 1u;
                //折射
                primaryPayload.nextRay[rayIndex].origin = hitInfo.hitPos + Toff * EPSILON;
                primaryPayload.nextRay[rayIndex].dir = T;
                primaryPayload.nextRay[rayIndex].throughputMul = hitInfo.transmissionColor * (1.0 - F2);

                //这段代码实现的是折射特性里面的 Beer-Lambert Law：光在介质里面走得越远，能量损失越多。
                if(hitInfo.airToMedium){
                    primaryPayload.nextRay[rayIndex].currentIOR = hitInfo.ior;
                    primaryPayload.nextRay[rayIndex].insideMedium = 1u;
                    primaryPayload.nextRay[rayIndex].mediumEntryPos = hitInfo.hitPos;
                }else if(hitInfo.mediumToAir){
                    float distanceInGlass = length(hitInfo.hitPos - primaryPayload.mediumEntryPos);
                    
                    //vec3 sigmaA = vec3(0.12, 0.06, 0.03);
                    vec3 sigmaA = vec3(0.03, 0.02, 0.01);
                    vec3 beer = exp(-sigmaA * distanceInGlass); //光穿过玻璃走得越远，剩下的能量越少
                    primaryPayload.nextRay[rayIndex].throughputMul *= beer;

                    primaryPayload.nextRay[rayIndex].currentIOR = 1.0;
                    primaryPayload.nextRay[rayIndex].insideMedium = 0u;
                    primaryPayload.nextRay[rayIndex].mediumEntryPos = vec3(0.0);
                }
            }

            rayIndex++;//第二条射线，查询反射
            if(primaryPayload.depth < configUBO.maxReflectionDepth ){
                primaryPayload.spawnRayCount = 2u;
                //反射
                primaryPayload.nextRay[rayIndex].origin = hitInfo.hitPos + Roff * EPSILON;
                primaryPayload.nextRay[rayIndex].dir = R;
                primaryPayload.nextRay[rayIndex].throughputMul = vec3(F2);
            }

        }

        primaryPayload.done = 0u;
    }
}

void WhittedStyleRayTracing(in HitInfoStruct hitInfo){//没有随机分支，稳定
    vec3 directDiffuse = vec3(0.0);
    vec3 directSpecular = vec3(0.0);

    uint lightNum = min(configUBO.lightCount, uint(RTLIGHT_SIZE));

    // vec3 kd = (1.0 - metallic) * albedo;
    // vec3 diffuseBRDF = kd / PI;
    // // diffuse 抑制：为了玻璃材质
    // kd *= (1.0 - transmission);
    // kd *= mat.alpha;
    vec3 kd = (1.0 - hitInfo.metallic) * hitInfo.albedo;
    kd *= (1.0 - hitInfo.transmission);
    kd *= hitInfo.alpha;
    vec3 diffuseBRDF = kd / PI;

    //直接光线(漫反射，高光，阴影)
    for(uint i = 0u; i < lightNum; ++i){
        RtLightStruct light = rtLightUBO.lights[i];

        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitInfo.hitPos, maxT, lightRadiance);

        float NdotL = max(dot(hitInfo.N, L), 0.0);
        if(NdotL <= 0.0) continue;

        //给每一个light发射一根shadowray
        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N * SHADOW_BIAS;
        float visibility = 1.0f; //default is disable shadow
        if(configUBO.softShadowEnable == 0){
            visibility = traceShadowVisibility(shadowOrigin, L, maxT);
        }else{
            visibility = traceSoftShadowVisibility(
                shadowOrigin, hitInfo.hitPos, hitInfo.N,
                vec3(rtLightUBO.lights[i].position),
                rtLightUBO.lights[i].radius,
                configUBO.softShadowSampleNumber,
                hitInfo.state
            );
        }
        if(visibility <= 0.0) continue;

        vec3 H = safeNormalize(L + hitInfo.V);
        float NdotH = max(dot(hitInfo.N, H), 0.0);
        float VdotH = max(dot(hitInfo.V, H), 0.0);

        vec3 F1 = fresnelSchlick(VdotH, hitInfo.F0);

        //这是 Blinn-Phong。
        float shininess = mix(128.0, 4.0, hitInfo.roughness);
        float specFactor = pow(NdotH, shininess) * hitInfo.specular;
        directDiffuse += diffuseBRDF * lightRadiance * NdotL * visibility;
        //directSpecular += F * specFactor * lightRadiance * NdotL * visibility;
        // if(metallic > 0.9){
        //     directSpecular += albedo * specFactor * lightRadiance * NdotL * visibility;
        // } else {
        //     directSpecular += F * specFactor * lightRadiance * NdotL * visibility;
        // }
        if(hitInfo.metallic > 0.5){
            directDiffuse = vec3(0.0);
            directSpecular += hitInfo.albedo * specFactor * lightRadiance * NdotL * visibility;
        }else {
            directSpecular += F1 * specFactor * lightRadiance * NdotL * visibility;
        }
    }

    //vec3 localRadiance = emission + directDiffuse + directSpecular;
    vec3 localRadiance = hitInfo.emission;
    if(hitInfo.transmission < 0.01){
        localRadiance += directDiffuse + directSpecular;
    } else {
        localRadiance += directSpecular;
    }
    float ambientIntensity = 0.2;
    vec3 ambient = diffuseBRDF * SampleSky(hitInfo.N) * PI * ambientIntensity; //增加天空漫反射
    localRadiance += ambient;

    primaryPayload.radiance = localRadiance;
    primaryPayload.spawnRayCount = 0u;
    primaryPayload.done = 1u;

    UploadNextRays(hitInfo); //二次光线(折射，反射)
}

struct ScatterResult{
    vec3 direction;
    vec3 throughputMul;
    uint valid;
};

ScatterResult ScatterMetal(in HitInfoStruct hitInfo){// 只处理金属
    ScatterResult result;

    // 金属材质主要进行镜面反射
    vec3 reflectedDir = reflect(hitInfo.I, hitInfo.Ngeom);
    
    // 根据粗糙度添加随机性
    if (hitInfo.roughness > 0.0) {
        reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(hitInfo.Ngeom, hitInfo.state), hitInfo.roughness));
    }

    result.valid = 1u;
    result.direction = reflectedDir;
    result.throughputMul = hitInfo.F * hitInfo.albedo;

    return result;
}

ScatterResult ScatterDiffuse(in HitInfoStruct hitInfo){ // 只处理漫反射/介电反射
    ScatterResult result;

    // 根据菲涅尔项决定反射和漫反射的比例
    float reflectionProbability = (hitInfo.F.r + hitInfo.F.g + hitInfo.F.b) / 3.0;
    
    if (Rand(hitInfo.state) < reflectionProbability) {
        // 镜面反射
        vec3 reflectedDir = reflect(hitInfo.I, hitInfo.Ngeom);
        
        // 根据粗糙度添加随机性
        if (hitInfo.roughness > 0.0) {
            reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(hitInfo.Ngeom, hitInfo.state), hitInfo.roughness));
        }

        result.direction = reflectedDir;
        result.throughputMul = hitInfo.F / reflectionProbability;
    } else {
        result.direction = RandomDirectionInHemisphere(hitInfo.Ngeom, hitInfo.state);// 漫反射 

        vec3 kD = (1.0 - hitInfo.F) * (1.0 - hitInfo.metallic); // 能量守恒：漫反射部分 = (1 - F) * 漫反射颜色
        result.throughputMul = kD * hitInfo.albedo / (1.0 - reflectionProbability);
    }

    result.valid = 1u;
    return result;
}

void MDSPathTracing(in HitInfoStruct hitInfo){ //Mixed-deterministic/stochastic PT
    primaryPayload.spawnRayCount = 0u;

    //NEE = Next Event Estimation
    vec3 localRadiance = hitInfo.emission; // 当前命中点的局部 radiance
    if(configUBO.enableNEE != 0u) localRadiance += EstimateDirectLightingNEE(hitInfo, hitInfo.state);

    ScatterResult scatter; //散射逻辑：TODO 解决Warp Divergence问题
    if(hitInfo.material_type != MATERIAL_GLASS && hitInfo.material_type != MATERIAL_JADE){ //传统PathTracing部分，随机采样的 Monte Carlo Path Tracing
        if(hitInfo.material_type == MATERIAL_GOLD) 
            scatter = ScatterMetal(hitInfo);
        else if(hitInfo.material_type == MATERIAL_PLASTIC || hitInfo.material_type == MATERIAL_CERAMIC || hitInfo.material_type == MATERIAL_LIGHT) 
            scatter = ScatterDiffuse(hitInfo);// 处理电介质材质（混合漫反射和镜面反射）
        vec3 offsetDir = dot(scatter.direction, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom: -hitInfo.Ngeom;
        primaryPayload.spawnRayCount = scatter.valid;
        //primaryPayload.radiance = hitInfo.emission; //跟whitted的最大区别是，前者有rtlight设定，但PT里面没有rtlight，而是靠自发光物体
        primaryPayload.radiance = localRadiance; //NEE
        vec3 origin=hitInfo.hitPos+offsetDir*0.001;
        primaryPayload.nextRay[0].origin = origin; //hitPos + Ngeom * 0.001;
        primaryPayload.nextRay[0].dir = scatter.direction;
        primaryPayload.nextRay[0].throughputMul = scatter.throughputMul;
        primaryPayload.done = scatter.valid == 0u ? 1u : 0u;
    }else{ //whitted-style部分
        UploadNextRays(hitInfo); //对glass/jade，发射二次光线(折射，反射)
    }
}

void updatePayload(in MaterialStruct mat, vec3 Ngeom, vec2 uv){
    //命中信息重建
    HitInfoStruct hitInfo;
    hitInfo.material_type = mat.type;

    hitInfo.albedo = mat.albedo;
    hitInfo.alpha = mat.alpha;
#ifndef DISABLE_TEXTURE
    //if (mat.baseColorTextureIndex != INVALID_TEXTURE_INDEX) {//add texture
        //vec4 baseColor = SampleTexture(mat.baseColorTextureIndex, uv);
        vec4 baseColor = SampleTexture(0, uv); //todo: use real tex id
        hitInfo.albedo *= baseColor.rgb;
        hitInfo.alpha *= baseColor.a;
    //}
#endif

    hitInfo.emission = mat.emissionColor * mat.emissionStrength;
    hitInfo.metallic = clamp(mat.metallic, 0.0, 1.0);
    hitInfo.roughness = clamp(mat.roughness, 0.02, 1.0);
    hitInfo.transmission = clamp(mat.transmission, 0.0, 1.0);
    hitInfo.specular = clamp(mat.specular, 0.0, 1.0);
    hitInfo.ior = max(mat.ior, 1.01);
    
    hitInfo.transmissionColor = mat.transmissionColor;

    hitInfo.hitPos = getWorldHitPos();
    hitInfo.I = safeNormalize(gl_WorldRayDirectionEXT); // 入射方向：射线前进方向
    hitInfo.V = -hitInfo.I; //视向向量，观察方向
    bool frontFace = dot(hitInfo.I, Ngeom) < 0.0; //入射光线落在表面的哪一侧（正面还是背面）
    vec3 N = frontFace ? Ngeom : -Ngeom; // N 始终朝向入射光
    hitInfo.N = N;
    hitInfo.Ngeom = Ngeom;

    hitInfo.airToMedium = (primaryPayload.insideMedium == 0u) && !frontFace;
    hitInfo.mediumToAir = (primaryPayload.insideMedium == 1u) && frontFace;
    //bool invalid1 = (primaryPayload.insideMedium == 1u && !frontFace);
    //bool invalid2 = (primaryPayload.insideMedium == 0u && frontFace);

    // 计算基础反射率 F0
    float f0Scalar = pow((1.0 - mat.ior) / (1.0 + mat.ior), 2.0);
    vec3 dielectricF0 = vec3(f0Scalar);
    hitInfo.F0 = mix(dielectricF0, mat.albedo, mat.metallic);
    // 计算菲涅尔项
    //float cosTheta = abs(dot(Ngeom, -I));
    //float cosTheta = abs(dot(N, -I));
    hitInfo.cosTheta = clamp(dot(hitInfo.N,-hitInfo.I),0.0,1.0);
    hitInfo.F = hitInfo.F0 + (1.0 - hitInfo.F0) * pow(1.0 - hitInfo.cosTheta, 5.0);

    // if (isGlass(hitInfo.transmission, hitInfo.alpha)) {}
    // else if (isMetal(hitInfo.metallic)){}
    // else{}

    uint state = gl_LaunchIDEXT.x;
    state = state * 747796405u + gl_LaunchIDEXT.y;
    state = state * 747796405u + uint(customUBO.frameCount);

    state ^= primaryPayload.sampleIndex;
    state *= 747796405u;

    state ^= primaryPayload.depth;
    state *= 747796405u;

    state ^= floatBitsToUint(hitInfo.hitPos.x);
    state *= 747796405u;
    state ^= floatBitsToUint(hitInfo.hitPos.y);
    state *= 747796405u;
    state ^= floatBitsToUint(hitInfo.hitPos.z);
    state *= 747796405u;

    state ^= floatBitsToUint(hitInfo.I.x);
    state *= 747796405u;
    state ^= floatBitsToUint(hitInfo.I.y);
    state *= 747796405u;
    state ^= floatBitsToUint(hitInfo.I.z);
    state *= 747796405u;

    state ^= uint(gl_PrimitiveID);
    state *= 747796405u;

    state ^= gl_InstanceCustomIndexEXT;
    state *= 747796405u;

    hitInfo.state = state;

    //primaryPayload.state = state; //更新primary payload的state，给RR使用

    if(customUBO.renderMode == 0) WhittedStyleRayTracing(hitInfo);
    else if(customUBO.renderMode == 1) MDSPathTracing(hitInfo);

}


#endif


