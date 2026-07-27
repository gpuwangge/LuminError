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

    //Cosine-weighted Hemisphere Sampling（Malley's Method）
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

float traceSoftShadowVisibility(vec3 origin, vec3 hitpos, vec3 N, vec3 lightCenter, float radius, int sampleCount, uint baseSeed) {
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

void WhittedStyleRayTracing(in HitInfoStruct hitInfo){
    /////////////////////
    //Method1： 确定性的 Whitted-style 光追 + 直接光照
    //没有随机分支，稳定
    //OUTPUT: primaryPayload.throughput, primaryPayload.radiance
    //1.直接光 + 阴影

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

    for(uint i = 0u; i < lightNum; ++i){
        RtLightInfo light = sboRtLightBuffer.lights[i];

        float maxT;
        vec3 lightRadiance;
        vec3 L = getLightDirAndRadiance(light, hitInfo.hitPos, maxT, lightRadiance);

        float NdotL = max(dot(hitInfo.N, L), 0.0);
        if(NdotL <= 0.0) continue;


        vec3 shadowOrigin = hitInfo.hitPos + hitInfo.N * SHADOW_BIAS;
        //float visibility = traceShadowVisibility(shadowOrigin, L, maxT);
        const int SHADOW_SAMPLES = 4; // 先从 4 / 8 / 16 试
        float visibility = traceSoftShadowVisibility(
            shadowOrigin, hitInfo.hitPos, hitInfo.N,
            vec3(sboRtLightBuffer.lights[i].position),
            sboRtLightBuffer.lights[i].radius,
            SHADOW_SAMPLES,
            hitInfo.state
        );
        if(visibility <= 0.0) continue;
        //float visibility = 1.0;//test, disable shadow

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
    bool hasTransmission = hitInfo.transmission > 0.01; //recover

    primaryPayload.radiance = localRadiance;
    primaryPayload.spawnRayCount = 0u;
    primaryPayload.done = 1u;

    if(hasReflection && hitInfo.transmission < 0.01){ //金属
        vec3 R = normalize(reflect(hitInfo.I, hitInfo.N));

        primaryPayload.nextRayOrigin0 = hitInfo.hitPos + hitInfo.N * EPSILON;
        primaryPayload.nextRayDir0 = normalize(mix(R, RandomDirectionInHemisphere(hitInfo.N, hitInfo.state), hitInfo.roughness * hitInfo.roughness));
        primaryPayload.nextRayThroughputMul0 = mix(vec3(0.04), hitInfo.albedo, hitInfo.metallic);

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

        primaryPayload.nextCurrentIOR0 = primaryPayload.currentIOR;
        primaryPayload.nextInsideMedium0 = primaryPayload.insideMedium;
        primaryPayload.nextMediumEntryPos0 = primaryPayload.mediumEntryPos;
        primaryPayload.nextCurrentIOR1 = primaryPayload.currentIOR;
        primaryPayload.nextInsideMedium1 = primaryPayload.insideMedium;
        primaryPayload.nextMediumEntryPos1 = primaryPayload.mediumEntryPos;
        if(tir){ // 全反射
            //primaryPayload.nextRayOrigin0 = hitPos + R * EPSILON;
            vec3 Roff = dot(R, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;
            primaryPayload.nextRayOrigin0 = hitInfo.hitPos + Roff * EPSILON;
            primaryPayload.nextRayDir0 = R;
            primaryPayload.nextRayThroughputMul0 = vec3(1.0);
            primaryPayload.spawnRayCount = 1u;
        }
        else{ //有反射和折射
            T = safeNormalize(T);

            vec3 Toff = dot(T, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;
            vec3 Roff = dot(R, hitInfo.Ngeom) > 0.0 ? hitInfo.Ngeom : -hitInfo.Ngeom;

            float F2 = pow(fresnelScalar,0.5);

            //折射
            primaryPayload.nextRayOrigin0 = hitInfo.hitPos + Toff * EPSILON;
            primaryPayload.nextRayDir0 = T;
            primaryPayload.nextRayThroughputMul0 = hitInfo.transmissionColor * (1.0 - F2);

            if(hitInfo.airToMedium){
                primaryPayload.nextCurrentIOR0 = hitInfo.ior;
                primaryPayload.nextInsideMedium0 = 1u;
                primaryPayload.nextMediumEntryPos0 = hitInfo.hitPos;
            }else if(hitInfo.mediumToAir){
                float distanceInGlass = length(hitInfo.hitPos - primaryPayload.mediumEntryPos);
                
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
            primaryPayload.nextRayOrigin1 = hitInfo.hitPos + Roff * EPSILON;
            primaryPayload.nextRayDir1 = R;
            //primaryPayload.nextRayThroughputMul1 = vec3(fresnelScalar);
            //float F = pow(fresnelScalar,0.8);
            primaryPayload.nextRayThroughputMul1 = vec3(F2);
            //float reflectionBoost = 2.0;
            //primaryPayload.nextRayThroughputMul1 = vec3(min(fresnelScalar * reflectionBoost,1.0));

            primaryPayload.spawnRayCount = 2u;
        }

        primaryPayload.done = 0u;
    }//end of transmission
}

void PathTracing(in HitInfoStruct hitInfo){ //随机采样的 Monte Carlo Path Tracing
    //OUTPUT: primaryPayload.throughput

    //Material mat = sboMaterial.materials[hitInfo.material_id];
    
    // 添加自发光贡献
    //vec3 emittedLight = mat.emissionColor * mat.emissionStrength;
    //result_brightness_score += emittedLight * payload.throughput;

    // 俄罗斯轮盘赌（从第3次反弹开始）
    //if(i > 2) {
    //    float p = max(primaryPayload.throughput.r, max(primaryPayload.throughput.g, primaryPayload.throughput.b));
    //    if(p < 0.001) return;
    //    if(Rand(hitInfo.state) > p) return;
        //throughput /= p;
    //}
    
    primaryPayload.spawnRayCount = 0u;
    vec3 throughputMul = vec3(1.0);

    // 处理折射/透射材质（玻璃、水等）
    vec3 I0 = hitInfo.I;
    vec3 Ngeom0 = hitInfo.Ngeom;
    if (hitInfo.transmission > 0.0 && hitInfo.alpha < 0.5) {
        // float refractionRatio = hitInfo.ior;
        // bool entering = dot(Ngeom0, -hitInfo.I) > 0.0;
        
        
        // if (!entering) {
        //     Ngeom0 = -Ngeom0;
        //     refractionRatio = 1.0 / hitInfo.ior;
        // }
        
        // vec3 refractedDir = refract(hitInfo.I, Ngeom0, refractionRatio);

        // 判断当前是进入介质还是离开介质
        bool entering = dot(hitInfo.I, Ngeom0) < 0.0;

        // GLSL refract() 要求 eta = n1 / n2
        float eta;

        if (entering)
        {
            // Air -> Glass
            eta = 1.0 / hitInfo.ior;
        }
        else
        {
            // Glass -> Air
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
        float cosTheta = abs(dot(perturbedNormal, -hitInfo.I));
        vec3 F = hitInfo.F0 + (1.0 - hitInfo.F0) * pow(1.0 - cosTheta, 5.0);
        
        //float reflectionProbability = (F.r + F.g + F.b) / 3.0;
        float reflectionProbability = clamp(dot(F, vec3(0.333333)), 0.05, 0.95);
        
        if (length(refractedDir) < 0.001 || Rand(hitInfo.state) < reflectionProbability * 0.8) {
            I0 = reflect(hitInfo.I, perturbedNormal);
            if (hitInfo.roughness > 0.0) {
                I0 = normalize(mix(I0, RandomDirectionInHemisphere(perturbedNormal, hitInfo.state), hitInfo.roughness));
            }
            throughputMul *= F / max(reflectionProbability, 0.1);
        } else {
            I0 = refractedDir;
            vec3 glassColor = hitInfo.transmissionColor * hitInfo.albedo;
            throughputMul *= glassColor * (1.0 - F) / max(1.0 - reflectionProbability, 0.1);
        }
        primaryPayload.spawnRayCount = 1u;
    }
    
    // 处理金属材质（高反射）
    else if (hitInfo.metallic > 0.8) {
        // 金属材质主要进行镜面反射
        vec3 reflectedDir = reflect(hitInfo.I, Ngeom0);
        
        // 根据粗糙度添加随机性
        if (hitInfo.roughness > 0.0) {
            reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(Ngeom0, hitInfo.state), hitInfo.roughness));
        }
        
        I0 = reflectedDir;
        throughputMul *= hitInfo.F * hitInfo.albedo;
        primaryPayload.spawnRayCount = 1u;
    }

    
    // 处理电介质材质（混合漫反射和镜面反射）
    else {
        // 根据菲涅尔项决定反射和漫反射的比例
        float reflectionProbability = (hitInfo.F.r + hitInfo.F.g + hitInfo.F.b) / 3.0;
        
        if (Rand(hitInfo.state) < reflectionProbability) {
            // 镜面反射
            vec3 reflectedDir = reflect(I0, Ngeom0);
            
            // 根据粗糙度添加随机性
            if (hitInfo.roughness > 0.0) {
                reflectedDir = normalize(mix(reflectedDir, RandomDirectionInHemisphere(Ngeom0, hitInfo.state), hitInfo.roughness));
            }
            
            I0 = reflectedDir;
            throughputMul *= hitInfo.F / reflectionProbability;
            primaryPayload.spawnRayCount = 1u;
        } else {
            // 漫反射 
            I0 = RandomDirectionInHemisphere(Ngeom0, hitInfo.state);
            
            // 能量守恒：漫反射部分 = (1 - F) * 漫反射颜色
            vec3 kD = (1.0 - hitInfo.F) * (1.0 - hitInfo.metallic);
            throughputMul *= kD * hitInfo.albedo / (1.0 - reflectionProbability);

            primaryPayload.spawnRayCount = 1u;
        }
    }

    primaryPayload.radiance = hitInfo.emission; //跟whitted的最大区别是，前者有rtlight设定，但PT里面没有rtlight，而是靠自发光物体
    vec3 offsetDir = dot(I0,Ngeom0)>0?Ngeom0:-Ngeom0;
    vec3 origin=hitInfo.hitPos+offsetDir*0.001;
    primaryPayload.nextRayOrigin0 = origin; //hitPos + Ngeom * 0.001;
    primaryPayload.nextRayDir0 = I0;
    primaryPayload.nextRayThroughputMul0 = throughputMul;

    primaryPayload.done = 0u;
}



void updatePayload(in Material mat, vec3 Ngeom){
    /**************
    命中信息重建
    **************/
    HitInfoStruct hitInfo;

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

    hitInfo.albedo = mat.albedo;
    hitInfo.emission = mat.emissionColor * mat.emissionStrength;
    hitInfo.metallic = clamp(mat.metallic, 0.0, 1.0);
    hitInfo.roughness = clamp(mat.roughness, 0.02, 1.0);
    hitInfo.transmission = clamp(mat.transmission, 0.0, 1.0);
    hitInfo.specular = clamp(mat.specular, 0.0, 1.0);
    hitInfo.ior = max(mat.ior, 1.01);
    hitInfo.alpha = mat.alpha;
    hitInfo.transmissionColor = mat.transmissionColor;

    //float f0Scalar = pow((1.0 - ior) / (1.0 + ior), 2.0);
    //vec3 dielectricF0 = vec3(f0Scalar);
    //vec3 F0 = mix(dielectricF0, albedo, metallic);

    // 计算基础反射率 F0
    //vec3 F0 = mix(vec3(mat.reflectance), albedo, metallic);
    float f0Scalar = pow((1.0 - mat.ior) / (1.0 + mat.ior), 2.0);
    vec3 dielectricF0 = vec3(f0Scalar);
    hitInfo.F0 = mix(dielectricF0, mat.albedo, mat.metallic);
    // 计算菲涅尔项
    //float cosTheta = abs(dot(Ngeom, -I));
    //float cosTheta = abs(dot(N, -I));
    hitInfo.cosTheta = clamp(dot(hitInfo.N,-hitInfo.I),0.0,1.0);
    hitInfo.F = hitInfo.F0 + (1.0 - hitInfo.F0) * pow(1.0 - hitInfo.cosTheta, 5.0);


    /*
     uint state = gl_LaunchIDEXT.x;
     state = state * 747796405u + gl_LaunchIDEXT.y;
     state = state * 747796405u + customUBO.frameCount;
     state = state * 747796405u + 2891336453u;
    //hitInfo.state = state;
    //TODO:state完全没有：
    // bounce depth
    // ray direction
    // hit position
    // primitive id
    // instance id

    state = state * 747796405u + uint(primaryPayload.seed);
    //state ^= uint(primaryPayload.seed);
    state = state * 747796405u + primaryPayload.depth;
    // state = state * 747796405u + uint(hitInfo.hitPos.x*12+hitInfo.hitPos.y*345);
    // state = state * 747796405u + uint(hitInfo.I.x);
    hitInfo.state = state;
    */


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

    if(customUBO.renderMode == 0){
        WhittedStyleRayTracing(hitInfo);
    }else if(customUBO.renderMode == 1){
        PathTracing(hitInfo);
    }
}


#endif


