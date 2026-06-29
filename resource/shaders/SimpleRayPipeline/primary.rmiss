#version 460
#extension GL_EXT_ray_tracing : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT Payload payload;
void main() {
    vec3 dir = normalize(gl_WorldRayDirectionEXT);

    float t = 0.5 * (dir.y + 1.0);
    t = pow(t, 1.6);

    vec3 skyTop    = vec3(1.00, 0.87, 0.69);
    vec3 skyMid    = vec3(0.80, 0.65, 0.45);
    vec3 skyBottom = vec3(0.10, 0.01, 0.01);    

    vec3 c1 = mix(skyBottom, skyMid, t);
    vec3 c2 = mix(skyMid, skyTop, t);
    vec3 skyColor = mix(c1, c2, t);

    payload.radiance += payload.throughput * skyColor;
    payload.done = 1u;
}