#version 460
#extension GL_EXT_ray_tracing : require
#include "../CommonShaders/rayPipelineCommon.glsl"
layout(location = 1) rayPayloadInEXT ShadowPayload shadowPayload;
void main(){
    shadowPayload.visibility = 1u;
}