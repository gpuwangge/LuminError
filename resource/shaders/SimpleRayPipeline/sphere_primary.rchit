#version 460

#include "../CommonShaders/rayPipelineCommonStruct.glsl"
#include "../CommonShaders/rayPipelineCommonHit.glsl"

//layout(location = 0) rayPayloadInEXT PrimaryPayload primaryPayload;
//layout(location = 1) rayPayloadEXT ShadowPayload shadowPayload;
hitAttributeEXT vec2 attribs;
//hitAttributeEXT vec3 hitNormal;

layout(set = 0, binding = 5, std430) readonly buffer SBOMaterial {
   Material materials[];
} sboMaterial;


/**************
Sphere related functions
**************/
vec3 getSphereWorldNormal(){
    // // 假设 sphere 在 object space 下球心为 (0,0,0)
    // vec3 objHitPos = getObjectHitPos();
    // vec3 objN = safeNormalize(objHitPos);

    // // 如果只有均匀缩放/旋转/平移，这样基本可用
    // vec3 worldN = normalize((gl_ObjectToWorldEXT * vec4(objN, 0.0)).xyz);
    // return worldN;

    vec3 objN = safeNormalize(getObjectHitPos());
    mat3 normalMat = transpose(inverse(mat3(gl_ObjectToWorldEXT)));
    return normalize(normalMat * objN);
}

void main(){
    uint materialIndex = uint(gl_InstanceCustomIndexEXT);
    Material mat = sboMaterial.materials[materialIndex];

    bool bExit = earlyExit(mat);
    if(bExit) return;
    
    //Core
    vec3 Ngeom = getSphereWorldNormal(); //Normal for Geometry
    updatePayload(mat, Ngeom);
}