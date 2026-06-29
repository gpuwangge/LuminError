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

#endif