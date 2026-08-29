#version 460
//#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommonStruct.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayloadStruct primaryPayload;

vec3 SampleSky(vec3 dir){
    //vec3 sky = vec3(0.2, 0.3, 0.4);
    //return sky;
    vec3 sky = vec3(0.075, 0.1, 0.2);

    dir = normalize(dir);
    float t = 0.5 * (dir.y + 1.0);
    return mix(vec3(1.0),sky,t);
}

vec3 SampleSky0(vec3 dir){
    dir = normalize(dir);

    // y >= 0：蓝色天空
    // y < 0：黄色地面/下半球
    return (dir.y >= 0.0)
        ? vec3(0.0, 0.0, 1.0)
        : vec3(1.0, 1.0, 0.0);
}

vec3 SampleSky1(vec3 dir)
{
    dir = normalize(dir);

    float t = 0.5 * (dir.y + 1.0);

    // 仅显示 t，黑 = 0，白 = 1
    return vec3(t);
}

vec3 SampleSky2(vec3 dir)
{
    dir = normalize(dir);

    // 顶部：略亮的雾蓝灰
    vec3 top = vec3(0.38, 0.43, 0.52);

    // 地平线：第一张主要的蓝灰背景色
    vec3 horizon = vec3(0.30, 0.35, 0.44);

    // 下方：略深，但不是原先那种深海军蓝
    vec3 bottom = vec3(0.24, 0.28, 0.36);

    float t = clamp(0.5 * (dir.y + 1.0), 0.0, 1.0);

    if (t < 0.5)
    {
        return mix(bottom, horizon, smoothstep(0.0, 0.5, t));
    }
    else
    {
        return mix(horizon, top, smoothstep(0.5, 1.0, t));
    }
}

void main(){
    primaryPayload.radiance = SampleSky2(gl_WorldRayDirectionEXT);
    //primaryPayload.radiance = SampleSky1(gl_WorldRayDirectionEXT);

    //primaryPayload.radiance = vec3(0.0);

    //primaryPayload.radiance = vec3(1.0, 1.0, 1.0);

    //vec3 dir = normalize(gl_WorldRayDirectionEXT);
    //primaryPayload.radiance = dir * 0.5 + 0.5;// 将 [-1, 1] 映射到 [0, 1]

    primaryPayload.done = 1u;
}