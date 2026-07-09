#version 460
#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;

void main()
{
    vec3 sky = vec3(0.6, 0.7, 0.9); // 先用常量天空色
    //primaryPayload.radiance = primaryPayload.throughput * sky;
    primaryPayload.radiance = sky;
    primaryPayload.done = 1u;
}