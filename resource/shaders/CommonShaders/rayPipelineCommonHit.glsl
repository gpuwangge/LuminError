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
    vec3 N_shade; // N 始终朝向入射光
    vec3 N_geom;
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

vec3 SampleCosineHemisphere(vec3 N, inout uint state){
    float u1 = Rand(state);
    float u2 = Rand(state);

    // Concentric disk sampling（也可换成更简单的 sqrt(u1) 版本）
    float r   = sqrt(u1);
    float phi = 2.0 * PI * u2;

    vec3 localDir = vec3(
        r * cos(phi),
        r * sin(phi),
        sqrt(max(0.0, 1.0 - u1))
    );

    // 建立以 N 为 z 轴的正交基
    vec3 helper = abs(N.z) < 0.999
        ? vec3(0.0, 0.0, 1.0)
        : vec3(0.0, 1.0, 0.0);

    vec3 T = normalize(cross(helper, N));
    vec3 B = cross(N, T);

    return normalize(T * localDir.x + B * localDir.y + N * localDir.z);
}

/**************
Light functions
**************/
bool isDirectionalLight(RtLightStruct light){
    return false; //disable this function for now
    //return light.type < 0.5;
}

bool isPointLight(RtLightStruct light){
    return true; //only implemented point light
    //return light.type >= 0.5 && light.type < 1.5;
}

bool isSpotLight(RtLightStruct light){
    return false; //disable this function for now
    //return light.type >= 1.5 && light.type < 2.5;
}

//只有whitted-style里面调用这个函数
vec3 getLightDirAndRadiance(RtLightStruct light, vec3 hitPos, out float maxT, out vec3 radiance){
    const float EPS = 1e-6;

    float intensity       = light.params.x;
    float sourceRadius    = max(light.params.y, 0.001);
    float range           = light.params.z;
    float falloffExponent = max(light.attenuation.x, 0.01);

    if (isDirectionalLight(light)) {
        vec3 L = safeNormalize(-light.direction.xyz);
        maxT = 1e32;
        radiance = light.color.rgb * intensity;
        return L;
    }

    vec3 toLight = light.position.xyz - hitPos;

    float dist2 = max(dot(toLight, toLight), EPS);
    float dist = sqrt(dist2);
    vec3 L = toLight / dist;

    // Shadow ray 的 tMax 必须仍是灯中心的几何距离。
    maxT = max(dist - SHADOW_BIAS, SHADOW_BIAS);

    // range <= 0：不截断，仅使用艺术距离衰减。
    // range > 0：range 外保证为 0。
    float attenuation = 1.0;

    if (range > EPS) {
        float s = dist / range;

        if (s >= 1.0) {
            radiance = vec3(0.0);
            return L;
        }

        // 在中心是 1，到 range 连续降至 0。
        // p 小 -> 更平坦、打得更远
        // p 大 -> 更集中于灯附近
        float rangeFade = max(1.0 - s * s, 0.0);
        attenuation *= pow(rangeFade, falloffExponent);
    }

    // 有限近场衰减，消除 r=0 处的无限亮。
    //
    // sourceRadius 是“半亮距离”：
    // dist == sourceRadius 时，这一项为 0.5。
    //
    // 注意：这里 exponent 使用一半，因为距离项是 r²。
    // falloff=2 时效果约为 1/(1+(d/R)^2)。
    float normalizedDistance = dist / sourceRadius;
    float nearFieldExponent = 0.5 * falloffExponent;

    attenuation *= 1.0 /
        (1.0 + pow(normalizedDistance, nearFieldExponent));

    if (isSpotLight(light)) {
        // 你当前 struct 中没有独立的 inner/outer angle 字段。
        // 暂时复用：
        // position.w  = outer half-angle, radians
        // direction.w = inner half-angle, radians
        float outerAngle = light.position.w;
        float innerAngle = light.direction.w;

        // 兜底：若 CPU 尚未填 direction.w，就自动取 80% outer。
        if (innerAngle <= 0.0 || innerAngle >= outerAngle) {
            innerAngle = outerAngle * 0.80;
        }

        // direction.xyz 的约定：从灯射向场景的方向。
        vec3 spotDir = safeNormalize(light.direction.xyz);

        // L 是 hitPos -> light；-L 才是 light -> hitPos。
        float cosTheta = dot(-L, spotDir);

        float cosOuter = cos(outerAngle);
        float cosInner = cos(innerAngle);

        // 内锥 1，外锥外 0，中间平滑过渡。
        float spotFactor = smoothstep(cosOuter, cosInner, cosTheta);

        attenuation *= spotFactor;
    }

    radiance = light.color.rgb * intensity * attenuation;
    return L;
}

/*
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
}*/

// vec3 SampleDiskLight(RtLightStruct light, vec3 shadingPos, inout uint state){ //for path tracing NEE
//     // 光源法线（与你 Whitted 保持一致）
//     vec3 lightNormal = normalize(shadingPos - light.position.xyz);

//     vec3 T, B;
//     buildOrthonormalBasis(lightNormal, T, B);

//     // 在单位圆盘随机采样
//     //vec2 d = sampleDisk(state) * light.radius;
//     vec2 d = sampleDisk(state) * light.params.y;

//     // 返回圆盘上的一点
//     return light.position.xyz +T * d.x +B * d.y;
// }

vec3 SampleDiskLight(RtLightStruct light, inout uint state, out vec3 diskNormal){
    // direction.xyz = disk 的固定正面 / 发光方向。
    // 例如天花板向下照：vec3(0.0, -1.0, 0.0)
    float dirLen2 = dot(light.direction.xyz, light.direction.xyz);

    // 防止 direction 未初始化或为零时 normalize(0)。
    // 建议最终在 CPU 端确保 disk light direction 永远有效。
    diskNormal = (dirLen2 > 1e-8)
        ? light.direction.xyz * inversesqrt(dirLen2)
        : vec3(0.0, -1.0, 0.0);

    vec3 T, B;
    buildOrthonormalBasis(diskNormal, T, B);

    float radius = max(light.params.y, 1e-5);

    // 必须是 uniform-area disk sample。
    vec2 diskOffset = sampleDisk(state) * radius;

    return light.position.xyz + T * diskOffset.x + B * diskOffset.y;
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
    return texture(texarray[texId], vec2(uv.x, 1.0 - uv.y));
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

    uint cullmask = 0xFF;
    if(configUBO.shadowRayIgnoreSphere == 1) 
        cullmask = 0x01; //01是三角形,02是球体。这样设置的时候，shadow ray在寻找whitted点光源的时候会忽略(穿透)所有球体。点光源就可以放在球体内部。

    traceRayEXT(
        topLevelAS,
        flags,
        cullmask, 
        0, 0, 1,   // missIndex = 1，假设你的 shadow miss 在 index 1
        origin,
        SHADOW_BIAS,
        dir,
        tMax,
        1          // payload location = 1
    );

    return float(shadowPayload.visibility);
}

//sampleDiskVogel()是为了产生没有噪声的软阴影
// const float GOLDEN_ANGLE = 2.39996322973;
// vec2 sampleDiskVogel(uint i, uint count){// 返回单位圆盘内的确定性采样点 // i: [0, count - 1]
//     float n = float(max(count, 1u));
//     float r = sqrt((float(i) + 0.5) / n);
//     float phi = float(i) * GOLDEN_ANGLE;

//     return r * vec2(cos(phi), sin(phi));
// }

//soft shadow只对whitted style有效
float traceSoftShadowVisibility(vec3 origin, vec3 hitpos, vec3 N, vec3 lightCenter, float radius, uint sampleCount, uint baseSeed) {
    float visible = 0.0;

    //vec3 lightNormal = normalize(hitpos); // disk faces shading point
    vec3 lightNormal = normalize(origin - lightCenter);
    vec3 T, B;
    buildOrthonormalBasis(lightNormal, T, B);

    //做两套软阴影效果：一个使用随机数，一个不使用随机数(但这个效果不好，去除)
    //但这样的话如果要使用随机数软阴影，就必须开accumulate了
    // bool useRandom = false;
    // if(configUBO.accumulate == 1u && customUBO.cameraInMotion == 0u) 
    //     useRandom = true; //如果开启积累，并且摄像机不动的时候
    // else
    //     sampleCount *= 4; //不适用随机的话，需要更多的sampleCount来生成软阴影

    for (int s = 0; s < sampleCount; ++s) {
        //uint rng = baseSeed ^ uint(s) * 1664525u + 1013904223u;
        uint rng = baseSeed;
        rng ^= uint(s) * 747796405u;
        rng *= 2891336453u;

        vec2 d = sampleDisk(rng) * radius;
        //if(useRandom) d = sampleDisk(rng) * radius;//随机软阴影，在开启积累的时候用这个。sample count只需要4。
        //else d = sampleDiskVogel(uint(s), sampleCount) * radius;//确定的软阴影，但要求16~64的sample count，关闭积累用这个。

        vec3 samplePos = lightCenter + T * d.x + B * d.y;

        vec3 toLight = samplePos - hitpos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 1e-4);

        float NdotL = dot(N, L);
        if (NdotL <= 0.0) continue;

        shadowPayload.visibility = 0u;

        const float EPS = 0.001;

        uint cullmask = 0xFF;
        if(configUBO.shadowRayIgnoreSphere == 1) 
            cullmask = 0x01; //01是三角形,02是球体。这样设置的时候，shadow ray在寻找whitted点光源的时候会忽略(穿透)所有球体。点光源就可以放在球体内部。
            
        traceRayEXT(
            topLevelAS,
            gl_RayFlagsTerminateOnFirstHitEXT |
            gl_RayFlagsSkipClosestHitShaderEXT,
            cullmask,
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

//只有PT里的NEE调用这个函数。根据Whitted Style的getLightDirAndRadiance缝合。单位不太正确，不太能量守恒(todo)。
vec3 getLightDirAndRadianceAtPosition(RtLightStruct light,vec3 hitPos,vec3 sampledLightPos,out float maxT,out vec3 radiance){
    const float EPS = 1e-6;

    float intensity       = light.params.x;
    float sourceRadius    = max(light.params.y, 0.001);
    float range           = light.params.z;
    float falloffExponent = max(light.attenuation.x, 0.01);

    // Directional light 不应调用这个函数。
    vec3 toLight = sampledLightPos - hitPos;

    float dist2 = max(dot(toLight, toLight), EPS);
    float dist = sqrt(dist2);
    vec3 L = toLight / dist;

    maxT = max(dist - SHADOW_BIAS, SHADOW_BIAS);

    float attenuation = 1.0;

    if (range > EPS){
        float s = dist / range;

        if (s >= 1.0){
            radiance = vec3(0.0);
            return L;
        }

        float rangeFade = max(1.0 - s * s, 0.0);
        attenuation *= pow(rangeFade, falloffExponent);
    }

    float normalizedDistance = dist / sourceRadius;
    float nearFieldExponent = 0.5 * falloffExponent;

    attenuation *= 1.0 / (1.0 + pow(normalizedDistance, nearFieldExponent));

    if (isSpotLight(light)){
        float outerAngle = light.position.w;
        float innerAngle = light.direction.w;

        if (innerAngle <= 0.0 || innerAngle >= outerAngle) innerAngle = outerAngle * 0.80;

        vec3 spotDir = safeNormalize(light.direction.xyz);

        // sampledLightPos -> hitPos 的方向 = -L。
        float cosTheta = dot(-L, spotDir);

        float cosOuter = cos(outerAngle);
        float cosInner = cos(innerAngle);

        float spotFactor = smoothstep(cosOuter, cosInner, cosTheta);

        attenuation *= spotFactor;
    }

    radiance = light.color.rgb * intensity * attenuation;
    return L;
}

vec3 EstimateDirectLightingNEE(in HitInfoStruct hitInfo){//只有PT里使用NEE
    uint lightCount = min(configUBO.lightCount, uint(RTLIGHT_SIZE));

    if (configUBO.enableNEE == 0u || lightCount == 0u) return vec3(0.0);

    //先固定/随机选一盏解析灯。
    //lightCount == 1，lightIndex 永远为 0。
    uint lightIndex = min(uint(Rand(hitInfo.state) * float(lightCount)), lightCount - 1u);

    RtLightStruct light = rtLightUBO.lights[lightIndex];

    vec3 direct = vec3(0.0);

    for (int i = 0; i < configUBO.NEESampleCount; ++i){
        vec3 sampledLightPos;

        if (configUBO.NEESoftShadow == 0u){ 
            sampledLightPos = light.position.xyz;
        }
        else{
            vec3 diskNormal = safeNormalize(light.direction.xyz);

            vec3 T, B;
            buildOrthonormalBasis(diskNormal, T, B);

            float radius = max(light.params.y, 1e-5);
            vec2 d = sampleDisk(hitInfo.state) * radius;

            sampledLightPos = light.position.xyz + T * d.x + B * d.y;
        }

        float maxT;
        vec3 Li;

        vec3 L = getLightDirAndRadianceAtPosition(light, hitInfo.hitPos, sampledLightPos, maxT, Li);

        if (max(max(Li.r, Li.g), Li.b) <= 0.0) continue;

        float NdotL = max(dot(hitInfo.N_shade, L), 0.0);
        if (NdotL <= 0.0) continue;

        float offsetSign = (dot(hitInfo.N_geom, L) >= 0.0) ? 1.0 : -1.0;

        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N_geom * (offsetSign * SHADOW_BIAS);

        float visibility = traceShadowVisibility( shadowOrigin, L, maxT);

        if (visibility <= 0.0) continue;

        float lightSelectionPdf = 1.0 / float(lightCount);

        vec3 kd =
            (vec3(1.0) - hitInfo.F) *
            (1.0 - hitInfo.metallic) *
            (1.0 - hitInfo.transmission) *
            hitInfo.alpha *
            hitInfo.albedo;

        vec3 diffuseBRDF = kd / PI;


        /////////
        vec3 H = safeNormalize(L + hitInfo.V);
        float NdotH = max(dot(hitInfo.N_shade, H), 0.0);
        float VdotH = max(dot(hitInfo.V, H), 0.0);

        vec3 F1 = fresnelSchlick(VdotH, hitInfo.F0);
        float shininess = mix(128.0, 4.0, hitInfo.roughness);
        float specFactor = pow(NdotH, shininess) * hitInfo.specular;

        vec3 specularBRDF;
        if (hitInfo.metallic > 0.5) specularBRDF = hitInfo.albedo * specFactor;
        else specularBRDF = F1 * specFactor;

        direct += (diffuseBRDF + specularBRDF) * Li * NdotL * visibility / lightSelectionPdf;

        ///////

        //direct += diffuseBRDF * Li * NdotL * visibility / lightSelectionPdf;
    }

    return direct / float(configUBO.NEESampleCount);
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

//whitted style在结束的时候一定会发射二次光线，MPT在遇到glass/jade等材质的时候发射二次光线
void IndirectLight(in HitInfoStruct hitInfo){
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
        vec3 R = normalize(reflect(hitInfo.I, hitInfo.N_shade));

        primaryPayload.nextRay[0].origin = hitInfo.hitPos + hitInfo.N_shade * EPSILON;
        primaryPayload.nextRay[0].dir = normalize(mix(R, RandomDirectionInHemisphere(hitInfo.N_shade, hitInfo.state), hitInfo.roughness * hitInfo.roughness));
        //primaryPayload.nextRay[0].dir = R; //金属会变成完美镜面金属；hitInfo.roughness 在这个分支中不再生效。
        primaryPayload.nextRay[0].throughputMul = mix(vec3(0.04), hitInfo.albedo, hitInfo.metallic);

        primaryPayload.spawnRayCount = 1u;
        primaryPayload.done = 0u;
    }else if(hitInfo.material_type == MATERIAL_GLASS || hitInfo.material_type == MATERIAL_JADE){

        //test
        // primaryPayload.radiance = vec3(0.0);
        // primaryPayload.spawnRayCount = 0u;
        // primaryPayload.done = 1u;
        // return;


    //if(hasTransmission){
        //float eta = frontFace ? (1.0 / ior) : ior;

        float n1 = primaryPayload.insideMedium == 1u ? hitInfo.ior : 1.0;
        float n2 = primaryPayload.insideMedium == 1u ? 1.0    : hitInfo.ior;
        float eta = n1 / n2;

        vec3 normal = hitInfo.N_geom; //test：按道理这里应该用N_geom的，但是Dragon Test里面N_geom效果很奇怪，需要debug，先暂时用N_shade
        normal = hitInfo.N_shade; //先暂时用N_shade //

        vec3 R = safeNormalize(reflect(hitInfo.I, normal));
        vec3 T = refract(hitInfo.I, normal, eta);

        bool tir = dot(T, T) < 1e-8;

        primaryPayload.nextRay[0].currentIOR = primaryPayload.currentIOR;
        primaryPayload.nextRay[0].insideMedium = primaryPayload.insideMedium;
        primaryPayload.nextRay[0].mediumEntryPos = primaryPayload.mediumEntryPos;
        primaryPayload.nextRay[1].currentIOR = primaryPayload.currentIOR;
        primaryPayload.nextRay[1].insideMedium = primaryPayload.insideMedium;
        primaryPayload.nextRay[1].mediumEntryPos = primaryPayload.mediumEntryPos;
        if(tir){ // 全反射
            //primaryPayload.nextRayOrigin0 = hitPos + R * EPSILON;
            vec3 Roff = dot(R, normal) > 0.0 ? normal : -normal;
            primaryPayload.nextRay[0].origin = hitInfo.hitPos + Roff * EPSILON;
            primaryPayload.nextRay[0].dir = R;
            primaryPayload.nextRay[0].throughputMul = vec3(1.0);
            primaryPayload.spawnRayCount = 1u;
        }
        else{ //有反射和折射
            T = safeNormalize(T);
            vec3 Toff = dot(T, normal) > 0.0 ? normal : -normal;
            vec3 Roff = dot(R, normal) > 0.0 ? normal : -normal;
            float F2 = fresnelScalar;//pow(fresnelScalar,0.5);

            uint rayIndex = 0; //第一条射线，查询折射
            if(primaryPayload.depth < configUBO.maxRefractionDepth ){ //&& Rand(hitInfo.state) < hitInfo.transmission
                primaryPayload.spawnRayCount = 1u;
                //折射
                primaryPayload.nextRay[rayIndex].origin = hitInfo.hitPos + Toff * EPSILON;
                primaryPayload.nextRay[rayIndex].dir = T;
                primaryPayload.nextRay[rayIndex].throughputMul = hitInfo.transmissionColor * (1.0 - F2);

                //primaryPayload.nextRay[rayIndex].throughputMul = vec3(0.05, 0.35, 0.12) * (1.0 - F2); //test

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

vec3 directLight(in HitInfoStruct hitInfo){ //只在whitted style里面使用
    //kd 可以理解为：分配给漫反射（diffuse / diffuse-like scattering）通道的有效反射率颜色。
    vec3 kd = (1.0 - hitInfo.metallic) * hitInfo.albedo; //排除金属的漫反射
    kd *= (1.0 - hitInfo.transmission); //排除投射的分量，给透射/折射通道腾出能量
    kd *= hitInfo.alpha; //按覆盖率或混合透明度缩小表面贡献
    vec3 diffuseBRDF = kd / PI; //通过kd可以计算Lambert 漫反射 BRDF 的颜色权重

    //直接光线(漫反射，高光，阴影)
    uint lightNum = min(configUBO.lightCount, uint(RTLIGHT_SIZE));
    vec3 directDiffuse = vec3(0.0);
    vec3 directSpecular = vec3(0.0);
    for(uint i = 0u; i < lightNum; ++i){
        RtLightStruct light = rtLightUBO.lights[i];

        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitInfo.hitPos, maxT, lightRadiance);

        float NdotL = max(dot(hitInfo.N_shade, L), 0.0);
        if(NdotL <= 0.0) continue;

        //给每一个light发射一根shadowray
        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N_geom * SHADOW_BIAS;
        float visibility = 1.0f; //default is disable shadow
        if(configUBO.softShadowEnable == 0){ //|| customUBO.cameraInMotion == 1
            visibility = traceShadowVisibility(shadowOrigin, L, maxT);
        }else{
            visibility = traceSoftShadowVisibility(
                shadowOrigin, hitInfo.hitPos, hitInfo.N_geom,
                vec3(rtLightUBO.lights[i].position),
                rtLightUBO.lights[i].params.y,
                //rtLightUBO.lights[i].radius,
                configUBO.softShadowSampleNumber,
                hitInfo.state
            );
        }
        if(visibility <= 0.0) continue;

        vec3 H = safeNormalize(L + hitInfo.V);
        float NdotH = max(dot(hitInfo.N_shade, H), 0.0);
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
            //directDiffuse = vec3(0.0);
            directSpecular += hitInfo.albedo * specFactor * lightRadiance * NdotL * visibility;
        }else {
            directSpecular += F1 * specFactor * lightRadiance * NdotL * visibility;
        }
    }//end of light loop

    vec3 localRadiance = hitInfo.emission;//recover
    if(hitInfo.transmission < 0.01) localRadiance += directDiffuse + directSpecular;
    else localRadiance += directSpecular;
    
    // Whitted mode only:
    // PT mode must not add this fake ambient term here.
    //if (!enableNEE) {
        float ambientIntensity = 0.2;
        vec3 ambient = diffuseBRDF * SampleSky(hitInfo.N_shade) * PI * ambientIntensity; //增加天空漫反射
        localRadiance += ambient;
    //}

    return localRadiance;
}

void WhittedStyleRayTracing(in HitInfoStruct hitInfo){//没有随机分支，稳定
    primaryPayload.radiance = directLight(hitInfo);
    primaryPayload.spawnRayCount = 0u;
    primaryPayload.done = 1u;

    IndirectLight(hitInfo); //二次光线(折射，反射)
}

struct ScatterResult{
    vec3 direction;
    vec3 throughputMul;
    uint valid;
};

ScatterResult ScatterMetal(in HitInfoStruct hitInfo){// 只处理金属
    //理想镜面测试
    //在当前阶段，保留“理想镜面金属 + 彩色 Fresnel”的最小修复完全是合理的工程选择：它稳定、容易调试，并且对于粗糙度很低的金属（例如你的 roughness = 0.1）视觉上往往已经足够可信
    ScatterResult result;

    vec3 N = faceforward(hitInfo.N_shade, hitInfo.I, hitInfo.N_geom);
    vec3 wi = normalize(reflect(hitInfo.I, N));

    float NoV = clamp(dot(N, -hitInfo.I), 0.0, 1.0);

    // 金属：baseColor 就是 F0，不要使用 dielectric 的 0.04。
    vec3 F0 = clamp(hitInfo.albedo, vec3(0.0), vec3(0.999));
    vec3 F  = F0 + (vec3(1.0) - F0) * pow(1.0 - NoV, 5.0);

    result.valid = dot(wi, hitInfo.N_geom) > 0.0 ? 1u : 0u;
    result.direction = wi;

    // 对 delta/specular reflection，若采样与 BRDF 配对，
    // 反射路径 throughput 可直接使用 Fresnel F。
    result.throughputMul = F;

    return result;
}



ScatterResult ScatterDiffuse(in HitInfoStruct hitInfo){
    ScatterResult result;

    vec3 N = hitInfo.N_shade;

    // 若 normal 可能未 face-forward，可先保证它面对入射 ray 的反侧。
    // 假设 hitInfo.I 是 ray 的前进方向（camera / previous hit -> current hit）
    if (dot(hitInfo.I, N) > 0.0) {
        N = -N;
    }

    vec3 wi = SampleCosineHemisphere(N, hitInfo.state);
    float NoL = max(dot(N, wi), 0.0);

    result.direction = wi;

    // Lambert:
    // f = albedo / PI
    // pdf = NoL / PI
    // throughputMul = f * NoL / pdf = albedo
    result.throughputMul = hitInfo.albedo;

    // Debug: 强制白色，不受布料绿色 albedo 影响
    //result.throughputMul = vec3(0.8);

    result.valid = (NoL > 0.0) ? 1u : 0u;
    return result;
}

void MDSPathTracing(in HitInfoStruct hitInfo){ //Mixed-deterministic/stochastic PT
    if(hitInfo.material_type != MATERIAL_GLASS && hitInfo.material_type != MATERIAL_JADE){ //传统PathTracing部分，随机采样的 Monte Carlo Path Tracing //&& hitInfo.material_type != MATERIAL_JADE
        primaryPayload.spawnRayCount = 0u;

        //NEE = Next Event Estimation
        vec3 localRadiance = hitInfo.emission; // 当前命中点的局部 radiance
        if(configUBO.enableNEE != 0u) localRadiance += EstimateDirectLightingNEE(hitInfo);

        // vec3 localRadiance;
        // if(configUBO.enableNEE != 0u) localRadiance = directLight(hitInfo); //测试直接用whitted的方法模拟NEE

        ScatterResult scatter; //散射逻辑：TODO 解决Warp Divergence问题
    
        if(hitInfo.material_type == MATERIAL_GOLD ) //|| hitInfo.material_type == MATERIAL_JADE
            scatter = ScatterMetal(hitInfo);
        else if(hitInfo.material_type == MATERIAL_PLASTIC || hitInfo.material_type == MATERIAL_CERAMIC || hitInfo.material_type == MATERIAL_LIGHT) 
            scatter = ScatterDiffuse(hitInfo);// 处理电介质材质（混合漫反射和镜面反射），里面有随机数发生
        vec3 offsetDir = dot(scatter.direction, hitInfo.N_geom) > 0.0 ? hitInfo.N_geom: -hitInfo.N_geom;
        primaryPayload.spawnRayCount = scatter.valid;
        //primaryPayload.radiance = hitInfo.emission; //跟whitted的最大区别是，前者有rtlight设定，但PT里面没有rtlight，而是靠自发光物体
        primaryPayload.radiance = localRadiance; //NEE
        vec3 origin=hitInfo.hitPos+offsetDir*0.001;
        primaryPayload.nextRay[0].origin = origin; //hitPos + N_geom * 0.001;
        primaryPayload.nextRay[0].dir = scatter.direction;
        primaryPayload.nextRay[0].throughputMul = scatter.throughputMul;
        primaryPayload.done = scatter.valid == 0u ? 1u : 0u;
    }else{ //whitted-style部分
        IndirectLight(hitInfo); //对glass/jade，发射二次光线(折射，反射)
    }
}

//如果你的 base-color texture 已经以 Vulkan 的 sRGB image format 创建，例如：
//VK_FORMAT_R8G8B8A8_SRGB
//那么采样时硬件会自动做 sRGB → linear 转换。此时 shader 里不能再调用 SRGBToLinear，否则是 double-decode，颜色会变暗且饱和度/观感异常。
// vec3 SRGBToLinear(vec3 c){
//     bvec3 cutoff = lessThanEqual(c, vec3(0.04045));

//     vec3 low  = c / 12.92;
//     vec3 high = pow((c + 0.055) / 1.055, vec3(2.4));

//     return mix(high, low, vec3(cutoff));
// }

void updatePayload(in MaterialStruct mat, vec3 Ng, vec3 Ns, InstanceStruct ins, vec2 uv){
    //命中信息重建
    HitInfoStruct hitInfo;
    hitInfo.material_type = mat.type;

    hitInfo.emission = mat.emissionColor * mat.emissionStrength;
    hitInfo.metallic = clamp(mat.metallic, 0.0, 1.0);
    hitInfo.roughness = clamp(mat.roughness, 0.02, 1.0);
    hitInfo.transmission = clamp(mat.transmission, 0.0, 1.0);
    hitInfo.specular = clamp(mat.specular, 0.0, 1.0);
    hitInfo.ior = max(mat.ior, 1.01);
    
    hitInfo.transmissionColor = mat.transmissionColor;

    hitInfo.albedo = mat.albedo;
    hitInfo.alpha = mat.alpha; 
#ifndef DISABLE_TEXTURE

    //if (mat.baseColorTextureIndex != INVALID_TEXTURE_INDEX) {//add texture
        //vec4 baseColor = SampleTexture(mat.baseColorTextureIndex, uv);
    uint textureIndex_baseColor = ins.textureIndex_baseColor;
    uint textureIndex_metallicRoughness = ins.textureIndex_metallicRoughness;
    
    vec4 baseColor = SampleTexture(textureIndex_baseColor, uv);
    hitInfo.albedo *= baseColor.rgb;
    hitInfo.alpha *= baseColor.a;
    //}

    //处理glb里面的alphaMode/alphaCut
    uint alphaMode = ins.alphaMode;
    if (alphaMode == ALPHA_MODE_OPAQUE) {
        hitInfo.alpha = 1.0;// glTF 规定：OPAQUE 忽略 baseColor alpha
    }else if (alphaMode == ALPHA_MODE_MASK) {
        // 注意：这在 closest-hit 中只能“黑掉当前交点”，
        // 不能让 ray 穿过该三角形，因此不是最终正确实现。
        // if (hitInfo.alpha < ins.alphaCutoff) {//测试用。如果实现了rahit就不需要这个了
        //     primaryPayload.radiance = vec3(0.0);
        //     primaryPayload.spawnRayCount = 0u;
        //     primaryPayload.done = 1u;
        //     return;
        // }

        hitInfo.alpha = 1.0;
    }
    else if (alphaMode == ALPHA_MODE_BLEND) {
        hitInfo.alpha = clamp(hitInfo.alpha, 0.0, 1.0);
    }

    

    //test: draw normal texture。可以用来证明切线空间贴图采样正确
    // vec3 n = SampleTexture(textureIndex_normal, uv).xyz * 2.0 - 1.0; 
    // vec3 debugColor = n * 0.5 + 0.5;
    // primaryPayload.radiance = debugColor;

    // primaryPayload.radiance = Ns;
    //不完全正确。它说明你现在确实在显示经过某种变换后的法线，但这张图存在很强的全局性异常：地面几乎纯绿、右墙近乎纯蓝、左墙大量黑/红，这不像正常、连续的 world-space shading normal 可视化。

    //vec3 dN = Ns - Ng;// 放大 8 倍，并把零差映射为中灰
    //primaryPayload.radiance = clamp(dN * 8.0 + 0.5, 0.0, 1.0);

    //primaryPayload.radiance = 0.5 * Ns + 0.5; //用了texture normal的话，边界比较smooth
    //primaryPayload.radiance = 0.5 * Ng + 0.5;//完全用geometry算的normal，在非平面部分转换比较生硬

    //在 glTF/GLB 2.0 标准的 metallicRoughnessTexture 中，真正有定义的只有
    //x / R	未使用	对 metallic-roughness 材质计算应忽略
    //y / G	roughness（粗糙度）	0 = 很光滑、清晰反射；1 = 很粗糙、反射发散
    //z / B	metallic（金属度）	0 = 非金属/绝缘体；1 = 金属
    //primaryPayload.radiance = vec3(metallicRoughness.y);
    //primaryPayload.radiance = vec3(metallicRoughness.w);
    
    //return;

    vec4 mr = SampleTexture(textureIndex_metallicRoughness, uv);
    // 注意：mr texture 应按线性数据读取；不能 sRGB decode。
    // hitInfo.roughness = clamp(mat.roughness * mr.g, 0.04, 1.0);
    // hitInfo.metallic = clamp(mat.metallic * mr.b,0.0, 1.);
    if(mr.b > 0.75) {
        hitInfo.material_type = 2; //设为金属
        // glTF-compatible metal/rough F0
        //hitInfo.F0 = mix(vec3(0.04), hitInfo.albedo, hitInfo.metallic);
        //hitInfo.ior = 1.5;       // 仅在 transmission / dielectric refraction 时有意义
        hitInfo.F0 = hitInfo.albedo;
        hitInfo.specular = 1.0;  // 若你的实现仍需要该字段，保持默认即可
        //hitInfo.reflectance？
    }
    hitInfo.roughness = clamp(ins.roughnessFactor * mr.g,  0.04, 1.0);
    hitInfo.metallic  = clamp(ins.metallicFactor * mr.b, 0.0, 1.0);

#endif

    //射线命中的是正面还是反面的判定：
    //dot(I, Ng) < 0：射线从 Ng 所指的半球射向表面，命中的是几何正面；
    //dot(I, Ng) > 0：射线从另一侧射来，命中的是背面。

    hitInfo.hitPos = getWorldHitPos();
    hitInfo.I = safeNormalize(gl_WorldRayDirectionEXT); // 入射方向：射线前进方向
    hitInfo.V = -hitInfo.I; //视向向量，观察方向

    bool frontFace = dot(hitInfo.I, Ng) < 0.0; //入射光线落在表面的哪一侧（正面还是背面），<0就是正面
    Ng = frontFace ? Ng : -Ng; // N 始终朝向入射光
    Ns = frontFace ? Ns : -Ns; // N 始终朝向入射光
    if (dot(Ns, Ng) < 0.0) Ns = -Ns; // 使 shading normal 留在 face-forward geometric normal 所在半球。
    

    hitInfo.N_shade = Ns;// Ns 用于光照、BRDF、Fresnel 和 normal map 外观。
    hitInfo.N_geom = Ng;// 保留 mesh winding 的 geometric normal。

    //todo: ai说这里frontFace写反了。但是反过来后dragon看起来怪怪的，需要验证
    hitInfo.airToMedium = (primaryPayload.insideMedium == 0u) && frontFace;
    hitInfo.mediumToAir = (primaryPayload.insideMedium == 1u) && !frontFace;
    //bool invalid1 = (primaryPayload.insideMedium == 1u && !frontFace);
    //bool invalid2 = (primaryPayload.insideMedium == 0u && frontFace);

    // 计算基础反射率 F0
    float f0Scalar = pow((1.0 - mat.ior) / (1.0 + mat.ior), 2.0);
    vec3 dielectricF0 = vec3(f0Scalar);
    hitInfo.F0 = mix(dielectricF0, mat.albedo, mat.metallic);
    // 计算菲涅尔项
    //float cosTheta = abs(dot(N_geom, -I));
    //float cosTheta = abs(dot(N, -I));
    hitInfo.cosTheta = clamp(dot(hitInfo.N_shade,-hitInfo.I),0.0,1.0);
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


