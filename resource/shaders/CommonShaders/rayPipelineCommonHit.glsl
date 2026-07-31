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
    uint cameraInMotion;
    uint lightCount;
    uint materialCount;

    uint renderMode;      // 0 = Whitted, 1 = Path Tracing, 2 = ReSTIR(未实现), 3 = Bidirectional(未实现)
    uint spp;             // Samples Per Pixel
    uint maxBounce;       // 最大反弹次数
    uint accumulate;      // 0 = 不积累, 1 = 帧间积累
    uint randomSeed;      // 可选，每次运行不同

    float rrProbability;   //RR（俄罗斯轮盘）
    uint enableNEE;
    uint useSky;
    float maxRadiance;
    uint debugMode;

    uint softShadowEnable; //for whitted style only
    uint softShadowSampleNumber; //for whitted style only

    uint maxReflectionDepth;
    uint maxRefractionDepth;
} customUBO;

const int RTLIGHT_SIZE = 64;//assume max 64 rt lights for now
layout(set = 0, binding = 6, std430) readonly buffer SBORtLightBuffer {
    RtLightInfo lights[RTLIGHT_SIZE];
} sboRtLightBuffer;

layout(set = 0, binding = 7, std430) readonly buffer SBOInstance {
   InstanceInfo instances[];
} sboInstance;

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

vec3 SampleDiskLight(RtLightInfo light, vec3 shadingPos, inout uint state){ //for path tracing NEE
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
    uint lightCount = min(customUBO.lightCount, uint(RTLIGHT_SIZE));

    if(customUBO.enableNEE == 0u || lightCount == 0u) return vec3(0.0);

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

    RtLightInfo light = sboRtLightBuffer.lights[lightIndex];

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

void WhittedStyleRayTracing(in HitInfoStruct hitInfo){//没有随机分支，稳定
    vec3 directDiffuse = vec3(0.0);
    vec3 directSpecular = vec3(0.0);

    uint lightNum = min(customUBO.lightCount, uint(RTLIGHT_SIZE));

    // vec3 kd = (1.0 - metallic) * albedo;
    // vec3 diffuseBRDF = kd / PI;
    // // diffuse 抑制：为了玻璃材质
    // kd *= (1.0 - transmission);
    // kd *= mat.alpha;
    vec3 kd = (1.0 - hitInfo.metallic) * hitInfo.albedo;
    kd *= (1.0 - hitInfo.transmission);
    kd *= hitInfo.alpha;
    vec3 diffuseBRDF = kd / PI;

    //1. 硬阴影
    for(uint i = 0u; i < lightNum; ++i){
        RtLightInfo light = sboRtLightBuffer.lights[i];

        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitInfo.hitPos, maxT, lightRadiance);

        float NdotL = max(dot(hitInfo.N, L), 0.0);
        if(NdotL <= 0.0) continue;


        //给每一个light发射一根shadowray
        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N * SHADOW_BIAS;
        float visibility = 1.0f; //default is disable shadow
        if(customUBO.softShadowEnable == 0){
            visibility = traceShadowVisibility(shadowOrigin, L, maxT);
        }else{
            visibility = traceSoftShadowVisibility(
                shadowOrigin, hitInfo.hitPos, hitInfo.N,
                vec3(sboRtLightBuffer.lights[i].position),
                sboRtLightBuffer.lights[i].radius,
                customUBO.softShadowSampleNumber,
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
    vec3 ambient = //增加天空漫反射
        diffuseBRDF *
        SampleSky(hitInfo.N) *
        PI *
        ambientIntensity;
    localRadiance += ambient;

    //2. 二次光线
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

    primaryPayload.radiance = localRadiance;
    primaryPayload.spawnRayCount = 0u;
    primaryPayload.done = 1u;

    if(hasReflection && hitInfo.transmission < 0.01){ //金属
        vec3 R = normalize(reflect(hitInfo.I, hitInfo.N));

        primaryPayload.nextRay[0].origin = hitInfo.hitPos + hitInfo.N * EPSILON;
        primaryPayload.nextRay[0].dir = normalize(mix(R, RandomDirectionInHemisphere(hitInfo.N, hitInfo.state), hitInfo.roughness * hitInfo.roughness));
        primaryPayload.nextRay[0].throughputMul = mix(vec3(0.04), hitInfo.albedo, hitInfo.metallic);

        primaryPayload.spawnRayCount = 1u;
        primaryPayload.done = 0u;
    }

    if(hasTransmission){
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
            if(primaryPayload.depth < customUBO.maxRefractionDepth ){ //&& Rand(hitInfo.state) < hitInfo.transmission
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
            if(primaryPayload.depth < customUBO.maxReflectionDepth ){
                primaryPayload.spawnRayCount = 2u;
                //反射
                primaryPayload.nextRay[rayIndex].origin = hitInfo.hitPos + Roff * EPSILON;
                primaryPayload.nextRay[rayIndex].dir = R;
                primaryPayload.nextRay[rayIndex].throughputMul = vec3(F2);
            }
            
            //fakeSSS效果一般，暂时保留commentted code
            //else{
                //vec3 fakeSSS=SampleSky(hitInfo.N)*hitInfo.transmissionColor*0.35; //sub surface scatter

                // RtLightInfo light = sboRtLightBuffer.lights[0];
                // vec3 L = normalize(light.position.xyz - hitInfo.hitPos);
                // float backLight = max(dot(-hitInfo.N, L), 0.0);
                // vec3 fakeSSS =
                //     backLight *
                //     light.color.rgb *
                //     light.intensity *
                //     hitInfo.transmissionColor *
                //     0.03;

                //primaryPayload.radiance += fakeSSS;
            //}

            //旧的反射代码
            // primaryPayload.nextRayOrigin1 = hitInfo.hitPos + Roff * EPSILON;
            // primaryPayload.nextRayDir1 = R;
            // //primaryPayload.nextRayThroughputMul1 = vec3(fresnelScalar);
            // //float F = pow(fresnelScalar,0.8);
            // primaryPayload.nextRayThroughputMul1 = vec3(F2);
            // //float reflectionBoost = 2.0;
            // //primaryPayload.nextRayThroughputMul1 = vec3(min(fresnelScalar * reflectionBoost,1.0));

            // primaryPayload.spawnRayCount = 2u;
        }

        primaryPayload.done = 0u;
    }//end of transmission
}

struct ScatterResult{
    vec3 direction;
    vec3 throughputMul;
    uint valid;
};
ScatterResult ScatterGlass(in HitInfoStruct hitInfo){// 只处理玻璃
    ScatterResult result;
    
    // float refractionRatio = hitInfo.ior;
    // bool entering = dot(Ngeom0, -hitInfo.I) > 0.0;

    // if (!entering) {
    //     Ngeom0 = -Ngeom0;
    //     refractionRatio = 1.0 / hitInfo.ior;
    // }
    
    // vec3 refractedDir = refract(hitInfo.I, Ngeom0, refractionRatio);

    // 判断当前是进入介质还是离开介质
    bool entering = dot(hitInfo.I, hitInfo.Ngeom) < 0.0;

    // GLSL refract() 要求 eta = n1 / n2
    float eta;
    vec3 Ngeom0 = hitInfo.Ngeom;

    if (entering){ // Air -> Glass
        eta = 1.0 / hitInfo.ior;
    }
    else{ // Glass -> Air
        Ngeom0 = -Ngeom0;
        eta = hitInfo.ior;
    }

    vec3 refractedDir = refract(hitInfo.I, Ngeom0, eta);
    
    // 增加基于粗糙度的法线扰动
    vec3 perturbedNormal = Ngeom0;
    if (hitInfo.roughness > 0.0) {
        vec3 randomJitter = RandomDirectionInHemisphere(Ngeom0, hitInfo.state) - Ngeom0;
        perturbedNormal = normalize(Ngeom0 + randomJitter * hitInfo.roughness * 0.3);
    }
    
    // 使用扰动后的法线重新计算
    refractedDir = refract(hitInfo.I, perturbedNormal, eta);
    //float cosTheta = abs(dot(perturbedNormal, -hitInfo.I));
    //vec3 F = hitInfo.F0 + (1.0 - hitInfo.F0) * pow(1.0 - cosTheta, 5.0);
    
    //float reflectionProbability = (F.r + F.g + F.b) / 3.0;
    float reflectionProbability = clamp(dot(hitInfo.F, vec3(0.333333)), 0.05, 0.95);
    
    if (length(refractedDir) < 0.001 || Rand(hitInfo.state) < reflectionProbability * 0.8) {
        result.direction = reflect(hitInfo.I, perturbedNormal);
        if (hitInfo.roughness > 0.0) {
            result.direction = normalize(mix(hitInfo.I, RandomDirectionInHemisphere(perturbedNormal, hitInfo.state), hitInfo.roughness));
        }
        result.throughputMul = hitInfo.F / max(reflectionProbability, 0.1);
    } else {
        result.direction = refractedDir;
        vec3 glassColor = hitInfo.transmissionColor * hitInfo.albedo;
        result.throughputMul = glassColor * (1.0 - hitInfo.F) / max(1.0 - reflectionProbability, 0.1);
    }

    result.valid = 1u;
    return result;
}

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
float JadeLuminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

ScatterResult ScatterJade(in HitInfoStruct hitInfo)
{
    ScatterResult result;
    result.direction = vec3(0.0);
    result.throughputMul = vec3(0.0);
    result.valid = 0u;

    /*
     * Jade 近似为三个散射分量：
     *
     * 1. 镜面反射：
     *      F
     *
     * 2. 弱透射：
     *      (1-F) * transmission * transmissionColor
     *
     * 3. 漫反射：
     *      (1-F) * (1-transmission) * albedo
     *
     * 三个分量的采样概率根据各自能量计算。
     * 因此不会因为 transProb 较小而把 throughput 放大十几倍。
     */

    vec3 F = clamp(hitInfo.F, vec3(0.0), vec3(1.0));

    float transmission =
        clamp(hitInfo.transmission, 0.0, 0.95);

    vec3 albedo =
        clamp(hitInfo.albedo, vec3(0.0), vec3(1.0));

    vec3 transmissionColor =
        clamp(hitInfo.transmissionColor, vec3(0.0), vec3(1.0));

    // 每个 lobe 真正携带的颜色/能量
    vec3 specWeight = F;

    vec3 transWeight =
        (vec3(1.0) - F) *
        transmission *
        transmissionColor;

    vec3 diffuseWeight =
        (vec3(1.0) - F) *
        (1.0 - transmission) *
        albedo;

    /*
     * 使用亮度构造标量采样概率。
     * EPS 避免所有分量恰好为零。
     */
    float specEnergy = max(JadeLuminance(specWeight), 0.0);
    float transEnergy = max(JadeLuminance(transWeight), 0.0);
    float diffuseEnergy = max(JadeLuminance(diffuseWeight), 0.0);

    float totalEnergy =
        specEnergy +
        transEnergy +
        diffuseEnergy;

    if(totalEnergy < 1e-6)
    {
        return result;
    }

    float specProb = specEnergy / totalEnergy;
    float transProb = transEnergy / totalEnergy;
    float diffuseProb = diffuseEnergy / totalEnergy;

    float randomValue = Rand(hitInfo.state);

    /*
     * hitInfo.N 是朝向入射射线一侧的法线。
     * 用它处理反射和漫反射，避免背面方向错误。
     */
    vec3 shadingNormal = hitInfo.N;

    /**********************************************************
     * 1. Glossy reflection
     **********************************************************/
    if(randomValue < specProb)
    {
        vec3 reflectedDir =
            safeNormalize(reflect(hitInfo.I, shadingNormal));

        /*
         * 当前不是严格的 GGX 采样，只是简单的粗糙反射近似。
         * 使用 roughness² 可以避免 roughness=0.35 时扰动过强。
         */
        float roughnessAmount =
            hitInfo.roughness * hitInfo.roughness;

        if(roughnessAmount > 0.001)
        {
            vec3 randomDir =
                RandomDirectionInHemisphere(
                    shadingNormal,
                    hitInfo.state
                );

            reflectedDir = safeNormalize(
                mix(
                    reflectedDir,
                    randomDir,
                    roughnessAmount
                )
            );
        }

        // 防止粗糙扰动把反射方向推入物体内部
        if(dot(reflectedDir, shadingNormal) <= 0.0)
        {
            reflectedDir =
                safeNormalize(reflect(hitInfo.I, shadingNormal));
        }

        result.direction = reflectedDir;

        /*
         * Monte Carlo 权重：
         *
         * contribution / samplingProbability
         */
        result.throughputMul =
            specWeight /
            max(specProb, 1e-6);

        result.valid = 1u;
        return result;
    }

    /**********************************************************
     * 2. Weak transmission
     **********************************************************/
    if(randomValue < specProb + transProb)
    {
        bool entering =
            dot(hitInfo.I, hitInfo.Ngeom) < 0.0;

        /*
         * refract() 要求法线朝向入射射线所在介质。
         */
        vec3 refractNormal =
            entering
                ? hitInfo.Ngeom
                : -hitInfo.Ngeom;

        float eta =
            entering
                ? 1.0 / hitInfo.ior
                : hitInfo.ior;

        /*
         * 对折射法线施加很小的粗糙扰动。
         * Jade 不应该像毛玻璃一样强烈随机折射。
         */
        float transmissionRoughness =
            hitInfo.roughness *
            hitInfo.roughness *
            0.20;

        if(transmissionRoughness > 0.001)
        {
            vec3 randomNormal =
                RandomDirectionInHemisphere(
                    refractNormal,
                    hitInfo.state
                );

            refractNormal = safeNormalize(
                mix(
                    refractNormal,
                    randomNormal,
                    transmissionRoughness
                )
            );

            // 扰动后仍须朝向入射射线
            if(dot(hitInfo.I, refractNormal) > 0.0)
            {
                refractNormal = -refractNormal;
            }
        }

        vec3 transmittedDir =
            refract(
                hitInfo.I,
                refractNormal,
                eta
            );

        /*
         * 全反射：
         * 本次虽然选择了 transmission lobe，
         * 但物理结果只能反射。
         */
        if(dot(transmittedDir, transmittedDir) < 1e-8)
        {
            result.direction =
                safeNormalize(
                    reflect(
                        hitInfo.I,
                        refractNormal
                    )
                );

            /*
             * TIR 时当前透射分量转为反射。
             * 使用 transWeight / transProb 保持本次样本能量稳定。
             */
            result.throughputMul =
                transWeight /
                max(transProb, 1e-6);

            result.valid = 1u;
            return result;
        }

        result.direction =
            safeNormalize(transmittedDir);

        /*
         * 关键修复：
         *
         * transWeight 中已经包含：
         *
         * transmissionColor
         * * transmission
         * * (1-F)
         *
         * 不会再发生：
         *
         * transmissionColor*(1-F)/0.05 ≈ 15
         */
        result.throughputMul =
            transWeight /
            max(transProb, 1e-6);

        result.valid = 1u;
        return result;
    }

    /**********************************************************
     * 3. Diffuse
     **********************************************************/
    result.direction =
        RandomDirectionInHemisphere(
            shadingNormal,
            hitInfo.state
        );

    /*
     * RandomDirectionInHemisphere 是 cosine-weighted。
     *
     * Lambert：
     *     f = diffuseWeight / PI
     *
     * PDF：
     *     pdf = cosTheta / PI
     *
     * f*cosTheta/pdf 抵消后为 diffuseWeight。
     */
    result.throughputMul =
        diffuseWeight /
        max(diffuseProb, 1e-6);

    result.valid = 1u;
    return result;
}

bool isGlass(float transmission, float alpha){ return transmission > 0.0 && alpha < 0.5; }
bool isJade(float transmission, float alpha){  return transmission > 0.05 && alpha > 0.9; }
bool isMetal(float metallic){ return metallic > 0.8; }
//bool isLambert(){}

void PathTracing(in HitInfoStruct hitInfo){ //随机采样的 Monte Carlo Path Tracing
    // 俄罗斯轮盘赌不应该放在这里，因为没有完整的throughput，而是要放在rgen里
    primaryPayload.spawnRayCount = 0u;

    //NEE = Next Event Estimation
    vec3 localRadiance = hitInfo.emission; // 当前命中点的局部 radiance
    if(customUBO.enableNEE != 0u) localRadiance += EstimateDirectLightingNEE(hitInfo, hitInfo.state);

    ScatterResult scatter; //散射逻辑：TODO 解决Warp Divergence问题
    if(isGlass(hitInfo.transmission, hitInfo.alpha)) scatter = ScatterGlass(hitInfo);
    else if(isMetal(hitInfo.metallic)) scatter = ScatterMetal(hitInfo);
    //else if(isJade(hitInfo.transmission, hitInfo.alpha)) scatter = ScatterJade(hitInfo);
    else scatter = ScatterDiffuse(hitInfo);// 处理电介质材质（混合漫反射和镜面反射）

    vec3 offsetDir = dot(scatter.direction, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom: -hitInfo.Ngeom;
    primaryPayload.spawnRayCount = scatter.valid;
    //primaryPayload.radiance = hitInfo.emission; //跟whitted的最大区别是，前者有rtlight设定，但PT里面没有rtlight，而是靠自发光物体
    primaryPayload.radiance = localRadiance; //NEE
    vec3 origin=hitInfo.hitPos+offsetDir*0.001;
    primaryPayload.nextRay[0].origin = origin; //hitPos + Ngeom * 0.001;
    primaryPayload.nextRay[0].dir = scatter.direction;
    primaryPayload.nextRay[0].throughputMul = scatter.throughputMul;
    primaryPayload.done = scatter.valid == 0u ? 1u : 0u;
}

void updatePayload(in Material mat, vec3 Ngeom){
    //命中信息重建
    HitInfoStruct hitInfo;
    hitInfo.albedo = mat.albedo;
    hitInfo.emission = mat.emissionColor * mat.emissionStrength;
    hitInfo.metallic = clamp(mat.metallic, 0.0, 1.0);
    hitInfo.roughness = clamp(mat.roughness, 0.02, 1.0);
    hitInfo.transmission = clamp(mat.transmission, 0.0, 1.0);
    hitInfo.specular = clamp(mat.specular, 0.0, 1.0);
    hitInfo.ior = max(mat.ior, 1.01);
    hitInfo.alpha = mat.alpha;
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
    else if(customUBO.renderMode == 1) PathTracing(hitInfo);

}


#endif


