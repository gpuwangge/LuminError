#version 460
#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;
hitAttributeEXT vec2 attribs;

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


//////////////Untility functions/////////////////////
const float PI  = 3.14159265359;
const float EPS = 1e-4;

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 v)
{
    return clamp(v, vec3(0.0), vec3(1.0));
}

uint pcg_hash(uint v)
{
    v = v * 747796405u + 2891336453u;
    v = ((v >> ((v >> 28u) + 4u)) ^ v) * 277803737u;
    v = (v >> 22u) ^ v;
    return v;
}

float rand(inout uint state)
{
    state = pcg_hash(state);
    return float(state) / 4294967296.0;
}

vec3 randomUnitVector(inout uint rng)
{
    float z = rand(rng) * 2.0 - 1.0;
    float a = rand(rng) * 2.0 * PI;
    float r = sqrt(max(0.0, 1.0 - z * z));
    return vec3(r * cos(a), r * sin(a), z);
}

vec3 cosineHemisphere(vec3 N, inout uint rng)
{
    vec3 u = normalize(abs(N.x) > 0.1 ? cross(vec3(0.0, 1.0, 0.0), N)
                                      : cross(vec3(1.0, 0.0, 0.0), N));
    vec3 v = cross(N, u);

    float r1 = rand(rng);
    float r2 = rand(rng);

    float phi = 2.0 * PI * r1;
    float r   = sqrt(r2);
    float x   = r * cos(phi);
    float y   = r * sin(phi);
    float z   = sqrt(max(0.0, 1.0 - r2));

    return normalize(u * x + v * y + N * z);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    float f = pow(1.0 - saturate(cosTheta), 5.0);
    return F0 + (1.0 - F0) * f;
}

// 假设 attribs.xy 是 sphere 的 uv
vec3 sphereNormalFromAttribs(vec2 uv)
{
    float phi   = uv.x * 2.0 * PI;
    float theta = uv.y * PI;

    float sinTheta = sin(theta);
    return normalize(vec3(
        sinTheta * cos(phi),
        cos(theta),
        sinTheta * sin(phi)
    ));
}
/////////////////////////

void main(){
    uint material_Id = uint(gl_InstanceCustomIndexEXT);
    primaryPayload.throughput = vec3(1.0);
    primaryPayload.radiance   = sboMaterial.materials[material_Id].albedo * primaryPayload.throughput;
    primaryPayload.done       = 1u;
    return;

    uint materialId = uint(gl_InstanceCustomIndexEXT);
    Material mat = sboMaterial.materials[materialId];

    vec3 baseColor         = saturate(mat.albedo);
    vec3 emission          = mat.emissionColor * mat.emissionStrength;
    vec3 transmissionColor = saturate(mat.transmissionColor);

    float metallic     = saturate(mat.metallic);
    float roughness    = 0;//clamp(mat.roughness, 0.0, 1.0);//！
    float alpha        = saturate(mat.alpha);
    float reflectance  = saturate(mat.reflectance);
    float specular     = saturate(mat.specular);
    float transmission = 0;//saturate(mat.transmission);//！
    float ior          = max(mat.ior, 1.0001);

    vec3 Nobj = sphereNormalFromAttribs(attribs);
    vec3 N    = normalize((gl_ObjectToWorldEXT * vec4(Nobj, 0.0)).xyz);

    vec3 rayOrigin = gl_WorldRayOriginEXT;
    vec3 rayDir    = normalize(gl_WorldRayDirectionEXT);
    float t        = gl_HitTEXT;
    vec3 P         = rayOrigin + t * rayDir;

    vec3 V = normalize(-rayDir);

    bool frontFace = dot(rayDir, N) < 0.0;
    vec3 Ns = frontFace ? N : -N;

    uint rng = uint(gl_LaunchIDEXT.x) * 1973u +
               uint(gl_LaunchIDEXT.y) * 9277u +
               uint(gl_PrimitiveID)   * 2663u +
               materialId             * 1619u +
               floatBitsToUint(t);

    vec3 dielectricF0 = vec3(0.16 * reflectance * reflectance);
    dielectricF0 = mix(dielectricF0, vec3(specular), 0.5);
    vec3 F0 = mix(dielectricF0, baseColor, metallic);

    float NdotV = saturate(dot(Ns, V));
    vec3  F     = fresnelSchlick(NdotV, F0);

    primaryPayload.radiance = emission * primaryPayload.throughput;

    // alpha 很低时，当作吸收/终止；若你以后做真正透明裁剪，应放到 any-hit。
    if (alpha <= 0.001)
    {
        primaryPayload.done       = 1u;
        primaryPayload.nextOrigin = P;
        primaryPayload.nextDir    = vec3(0.0);
        return;
    }

    // 1) transmissive branch
    if (transmission > 0.001)
    {
        float etaI = frontFace ? 1.0 : ior;
        float etaT = frontFace ? ior : 1.0;
        float eta  = etaI / etaT;

        vec3 reflDir = reflect(rayDir, Ns);
        vec3 refrDir = refract(rayDir, Ns, eta);

        bool tir = length(refrDir) < 1e-6;

        float Fr = (F.x + F.y + F.z) * (1.0 / 3.0);
        float choose = rand(rng);

        if (tir || choose < Fr)
        {
            //vec3 glossyRefl = normalize(mix(reflDir, randomUnitVector(rng), roughness * roughness));
            vec3 hemi = cosineHemisphere(Ns, rng);
            vec3 glossyRefl = normalize(mix(reflDir, normalize(reflDir + hemi), roughness * roughness));
            if (dot(glossyRefl, Ns) <= 0.0) glossyRefl = reflDir;

            primaryPayload.nextOrigin = P + Ns * EPS;
            primaryPayload.nextDir    = glossyRefl;
            primaryPayload.throughput *= mix(vec3(1.0), baseColor, metallic) * max(Fr, 0.05);
            primaryPayload.done       = 0u;
            return;
        }
        else
        {
            vec3 glossyRefr = normalize(mix(refrDir, randomUnitVector(rng), roughness * roughness * 0.25));
            primaryPayload.nextOrigin = P + glossyRefr * EPS;
            primaryPayload.nextDir    = glossyRefr;
            primaryPayload.throughput *= transmissionColor * transmission * max(1.0 - Fr, 0.05);
            primaryPayload.done       = 0u;
            return;
        }
    }

    // 2) opaque branch: metallic specular vs diffuse
    float specProb = clamp(max(max(F.r, F.g), F.b), 0.05, 0.95);
    if (rand(rng) < mix(specProb, 1.0, metallic))
    {
        vec3 reflDir   = reflect(rayDir, Ns);
        vec3 glossyDir = normalize(mix(reflDir, randomUnitVector(rng), roughness * roughness));
        if (dot(glossyDir, Ns) < 0.0)
            glossyDir = normalize(reflect(glossyDir, Ns));

        primaryPayload.nextOrigin = P + Ns * EPS;
        primaryPayload.nextDir    = glossyDir;
        primaryPayload.throughput *= F;
        primaryPayload.done       = 0u;
        return;
    }
    else
    {
        vec3 diffuseDir = cosineHemisphere(Ns, rng);
        primaryPayload.nextOrigin = P + Ns * EPS;
        primaryPayload.nextDir    = diffuseDir;
        primaryPayload.throughput *= baseColor * (1.0 - metallic);
        primaryPayload.done       = 0u;
        return;
    }
}