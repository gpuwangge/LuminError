#version 460
//#extension GL_EXT_ray_tracing : require
#include "../CommonShaders/rayPipelineCommonStruct.glsl"

layout(location = 1) rayPayloadInEXT ShadowPayloadStruct shadowPayload;

void main(){
    //shadowPayload.visibility = 0u; //use rahit instead, so leave this empty
}
