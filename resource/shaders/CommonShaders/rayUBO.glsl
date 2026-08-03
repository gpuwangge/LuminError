#ifndef RAYUBO_GLSL
#define RAYUBO_GLSL

struct MaterialStruct {
    vec3 albedo;
    //float _padding0;
    vec3 emissionColor;
    //float _padding1;
    vec3 transmissionColor;
    //float _padding2;
    uint type;
    float metallic;
    float roughness;
    float alpha;
    float emissionStrength;
    float reflectance;
    float specular;
    float ior;
    float transmission;
    //float padding[44];
};

const int MATERIAL_SIZE = 64;//assume max 64 materials for now
layout(set = 0, binding = 3) readonly uniform MaterialUniformBufferInfo {
   MaterialStruct materials[MATERIAL_SIZE];
} sboMaterial;

struct TriangleVertexInfo{
    vec3 position;
    //float _padding0;
    vec3 normal;
    //float _padding1;
    vec3 tangent;
    //float _padding2;
    vec3 color;
    //float _padding3;
    vec2 uv;
    //float _padding4;
    //float _padding5;
    int material_id;
    float padding[15]; //60 bytes
    //total size: 16*4+4+60=128 bytes
};

layout(set = 0, binding = 4, std430) readonly buffer SBOTriangleVertex {
   TriangleVertexInfo vertices[];
} sboTriangleVertex;

layout(set = 0, binding = 5, std430) readonly buffer SBOTriangleIndex {
   uint indices[];
} sboTriangleIndex;

layout(set = 0, binding = 6, std430) readonly buffer SBOTriangleReorderIndex {
   uint indices[];
} sboTriangleReorderIndex;

struct BVHNode {
    vec3 bbox_min; //16 bytes
    //float _padding0;
    vec3 bbox_max; //16 bytes
    //float _padding1;
    int left; //4 bytes
    int right; //4 bytes
    int tri_start; //4 bytes
    int tri_count; //4 bytes
    float padding[4]; //16 bytes
    //total size: 16+16+4*4+16=64 bytes
};

layout(set = 0, binding = 7, std430) readonly buffer SBOBVHNode {
   BVHNode nodes[];
} sboBVHNode;


struct Sphere{
   vec3 position;
   float radius;
   int material_id;
   bool visibility;
   float padding;
};
const int NUM_SPHERES = 5;
//Sphere spheres[NUM_SPHERES];

layout(set = 0, binding = 8, std430) readonly buffer SBOSphere {
   Sphere spheres[];
} sboSphere;

layout(set = 0, binding = 9) uniform CustomBufferObject {
    int frameCount;
    bool cameraInMotion;
    uint triangleCount;
    uint materialCount;
} customUBO;

//to be added (Vulkan support up to ? SB bindings, for 1080TI it is unlimited)
//(SB Size limit is separate, usually 128MB or more, for 1080TI it is 4GB)

#endif