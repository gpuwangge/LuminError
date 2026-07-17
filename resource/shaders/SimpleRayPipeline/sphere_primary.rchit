#version 460
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

void main(){
    uint instanceIndex = uint(gl_InstanceCustomIndexEXT);
    uint materialIndex = sboInstance.instances[instanceIndex].materialIndex;
    Material mat = sboMaterial.materials[materialIndex];

    bool bExit = earlyExit(mat);
    if(bExit) return;

    //Core
    vec3 Ngeom = getSphereWorldNormal(); //Normal for Geometry
    updatePayload(mat, Ngeom);
}