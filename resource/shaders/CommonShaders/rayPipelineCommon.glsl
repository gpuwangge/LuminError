#ifndef RAY_COMMON_GLSL
#define RAY_COMMON_GLSL

struct PrimaryPayload {
    vec3 radiance;    // 当前这次 trace 返回的光照贡献
    vec3 throughput;  // 路径权重，给 rgen 累积用
    vec3 nextOrigin;  // 下一跳光线起点
    vec3 nextDir;     // 下一跳光线方向
    uint done;        // 1 = 终止，0 = 继续
};

struct ShadowPayload{
    uint visibility;
};


float hash11(float p)
{
    p = fract(p * 0.1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

float hash12(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec2 rnd2(vec2 pix, int sampleIndex, int frameIndex)
{
    float seed = hash12(pix + vec2(float(sampleIndex), float(frameIndex)));
    return vec2(
        hash11(seed + 1.0),
        hash11(seed + 2.0)
    );
}



/**************
Untility functions
**************/
const float PI = 3.14159265359;
const float EPSILON = 0.001;
const float SHADOW_BIAS = 0.01;

float saturate(float x){
    return clamp(x, 0.0, 1.0);
}

vec3 safeNormalize(vec3 v){
    float len2 = dot(v, v);
    if(len2 < 1e-12) return vec3(0.0, 0.0, 1.0);
    return v * inversesqrt(len2);
}

float luminance(vec3 c){
    return dot(c, vec3(0.2126, 0.7152, 0.0722));
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float fresnelSchlickScalar(float cosTheta, float ior){
    float f0 = (1.0 - ior) / (1.0 + ior);
    f0 = f0 * f0;
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}




/**************
以下是为了做软阴影的utility函数(lagacy)
**************/
uint hash_u32(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

//随机数
float rand01(inout uint state) {
    state = hash_u32(state);
    return float(state) / 4294967296.0;
}
//在灯周围构造局部基
//对圆盘灯，需要给灯的法线方向构造切线空间。你当前是点光源，没有方向，所以最省事的近似是“圆盘始终面向 shading point”，这相当于 billboarded disk，先做效果很方便。
void buildOrthonormalBasis(vec3 n, out vec3 t, out vec3 b) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    t = normalize(cross(up, n));
    b = cross(n, t);
}
//圆盘均匀采样
vec2 sampleDisk(inout uint rng) {
    float u1 = rand01(rng);
    float u2 = rand01(rng);

    float r = sqrt(u1);
    float phi = 6.28318530718 * u2;

    return vec2(r * cos(phi), r * sin(phi));
}

#endif