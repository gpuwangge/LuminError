#version 460
//#define ENABLE_TEXTURE
#include "../CommonShaders/rayPipelineCommonStruct.glsl"
#include "../CommonShaders/rayPipelineCommonHit.glsl"
hitAttributeEXT vec2 attribs;

/**************
Sphere related functions
**************/
vec3 getSphereWorldNormal(){
    vec3 objN = safeNormalize(getObjectHitPos());
    mat3 normalMat = transpose(inverse(mat3(gl_ObjectToWorldEXT)));
    return normalize(normalMat * objN);
}

vec2 getSphereUV(){
    // 对单位球：object-space position 本身就是从球心指向表面的方向。
    vec3 p = safeNormalize(getObjectHitPos());

    // U: 环绕 Y 轴，范围 [0, 1)
    float u = atan(p.z, p.x) / (2.0 * PI) + 0.5;

    // V: 从北极到南极，范围 [0, 1]
    float v = acos(clamp(p.y, -1.0, 1.0)) / PI;

    return vec2(u, v);
}

void main(){
    uint instanceIndex = uint(gl_InstanceCustomIndexEXT);
    uint materialIndex = instanceUBO.instances[instanceIndex].materialIndex;
    MaterialStruct mat = materialUBO.materials[materialIndex];

    //bool bExit = earlyExit(mat);
    //if(bExit) return;

    //Core
    vec3 Ngeom = getSphereWorldNormal(); //Normal for Geometry
    vec2 uv = getSphereUV();
    updatePayload(mat, Ngeom, uv);
}