#version 460
//#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommonStruct.glsl"

layout(location = 0) rayPayloadInEXT PrimaryPayloadStruct primaryPayload;

vec3 SampleSky(vec3 dir){
    vec3 sky = vec3(0.2, 0.3, 0.4);
    //return sky;

    dir = normalize(dir);
    float t = 0.5 * (dir.y + 1.0);
    return mix(vec3(1.0),sky,t);
}

void main(){
    primaryPayload.radiance = SampleSky(gl_WorldRayDirectionEXT);
    primaryPayload.done = 1u;
}