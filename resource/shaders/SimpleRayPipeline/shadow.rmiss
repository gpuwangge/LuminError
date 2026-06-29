#version 460
#extension GL_EXT_ray_tracing : require
//#include "../CommonShaders/rayPipelineCommon.glsl"
layout(location = 1) rayPayloadInEXT bool shadowed;
void main(){
    shadowed = false;
}