#version 460
#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;
hitAttributeEXT vec2 attribs;

void main()
{
    primaryPayload.throughput = vec3(1.0);
    primaryPayload.radiance   = vec3(0.0, 1.0, 0.0);
    primaryPayload.done       = 1u;
}