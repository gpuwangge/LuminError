#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "../CommonShaders/rayPipelineCommon.glsl"

layout(location = 0) rayPayloadInEXT Payload payload;
hitAttributeEXT vec2 bary;

struct TriangleVertexInfo{
    vec3 position;
    float _padding0;
    vec3 normal;
    float _padding1;
    vec3 tangent;
    float _padding2;
    vec3 color;
    float _padding3;
    vec2 uv;
    float _padding4;
    float _padding5;
    int material_id;
    float padding[15]; //60 bytes
    //total size: 16*4+4+60=128 bytes
};

//layout(set = 0, binding = 2, std430) readonly buffer SBOTriangleVertex { //host setup, but not used
//   TriangleVertexInfo vertices[];
//} sboTriangleVertex;

//layout(set = 0, binding = 3, std430) readonly buffer SBOTriangleIndex { //host setup, but not used
//   uint indices[];
//} sboTriangleIndex;

//buffer_reference：启用“缓冲引用”类型，即允许定义一个包含未定长数组或指针的 block，着色器会把它当作引用类型处理（像 pointer-to-buffer）
//scalar：表明使用“标量对齐”规则（而不是 std140/std430 的向量/结构对齐规则），这会影响数组和结构成员在内存中的偏移计算。
//buffer_reference_align = N：这是告诉编译器“这个引用所指向的数据在内存中至少按 N 字节对齐”，编译器据此生成更高效或正确的内存加载指令；你也必须保证运行时传入的设备地址满足这个对齐要求，否则行为可能未定义或性能受损。
//readonly 表示着色器通过这个引用只能读取数据，不能写回（便于优化和避免竞态）。
//buffer 关键字后面的名字（VertexBufferRef / IndexBufferRef）是类型名，定义好后可以在 shader 的其他地方用作变量类型。
layout(buffer_reference, scalar, buffer_reference_align = 16) readonly buffer VertexBufferRef { TriangleVertexInfo vertices[]; };
layout(buffer_reference, scalar, buffer_reference_align = 4) readonly buffer IndexBufferRef { uint indices[]; };
struct GeometryInfo {
    VertexBufferRef vertexBuf;
    IndexBufferRef  indexBuf;
};

layout(set = 0, binding = 4, scalar) readonly buffer SBOGeometryInfoBuffer {
    GeometryInfo infos[];
} sboGeometryInfos;

void main(){
    uint modelId = gl_InstanceCustomIndexEXT; //gl_InstanceCustomIndexEXT是一个可以自定义的量，我用它来代表用第几个模型。
    GeometryInfo geo = sboGeometryInfos.infos[modelId];

    uint prim = gl_PrimitiveID * 3u; //gl_PrimitiveID 是当前命中 primitive 的本地 ID，不是全场景统一 primitive ID。

    //这是原来使用sbo来传递数据的code。之前的做法是把所有object的model数据都挤到这两个sbo里面去，后面索引出现了问题。因为rt管线的prim不再是全局索引了。
    //新的方法把GeometryInfo也就是model本身的数据作为sbo传进来，也就是object和model分开。在host端通过设置InstanceCustomIndex来告诉shader这个object(instance)使用哪个model。
    //uint i0 = sboTriangleIndex.indices[prim + 0];
    //uint i1 = sboTriangleIndex.indices[prim + 1];
    //uint i2 = sboTriangleIndex.indices[prim + 2];
    //TriangleVertexInfo v0 = sboTriangleVertex.vertices[i0];
    //TriangleVertexInfo v1 = sboTriangleVertex.vertices[i1];
    //TriangleVertexInfo v2 = sboTriangleVertex.vertices[i2];

    uint i0 = geo.indexBuf.indices[prim + 0];
    uint i1 = geo.indexBuf.indices[prim + 1];
    uint i2 = geo.indexBuf.indices[prim + 2];
    TriangleVertexInfo v0 = geo.vertexBuf.vertices[i0];
    TriangleVertexInfo v1 = geo.vertexBuf.vertices[i1];
    TriangleVertexInfo v2 = geo.vertexBuf.vertices[i2];

    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    vec3 Pobj = v0.position * bc.x + v1.position * bc.y + v2.position * bc.z;
    vec3 Nobj = normalize(v0.normal * bc.x + v1.normal * bc.y + v2.normal * bc.z);

    vec3 P = vec3(gl_ObjectToWorldEXT * vec4(Pobj, 1.0));
    //vec3 N = normalize(Nobj * mat3(gl_WorldToObjectEXT));
    vec3 N = normalize(transpose(mat3(gl_WorldToObjectEXT)) * Nobj);
    N = -N;
    vec3 Nshading = N;
    if (dot(gl_WorldRayDirectionEXT, Nshading) < 0.0) Nshading = -Nshading; //do this so both sides of a triangle will have light color
    
    const int LIGHT_COUNT = 4; //TODO: Lights should be input, not hard-code

    vec3 lightPos[LIGHT_COUNT] = vec3[](
        vec3(-0.5, -0.0, -2.5), //red
        vec3(0.0, -0.5, -2.5),  //green
        vec3(-2.0, -2.0, -2.5),  //blue
        vec3(0.0, 0.0, 0.0)
    );

    vec3 lightColor[LIGHT_COUNT] = vec3[](
        vec3(0.0, 0.0, 1.0), //red
        vec3(0.0, 1.0, 0.0),  //green
        vec3(1.0, 0.0, 0.0),  //blue
        vec3(1.0, 0.0, 0.0)
    );

    float lightIntensity[LIGHT_COUNT] = float[](10.0, 10.0, 10.0, 0.0);

    vec3 baseColor = vec3(0.8, 0.7, 0.6);
    vec3 localLighting = baseColor * 0.25;   // ambient

    for (int i = 0; i < LIGHT_COUNT; ++i){
        vec3 toLight = lightPos[i] - P;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 1e-4);

        float diff = max(dot(Nshading, -L), 0.0);

        float attenuation = 1.0 / max(dist * dist, 1e-4);

        localLighting += baseColor * lightColor[i] * lightIntensity[i] * diff * attenuation;
    }

    vec3 hitPos = gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;
    //vec3 reflDir = reflect(payload.nextDir, N); // 实际上建议单独存当前入射方向
    vec3 reflDir = reflect(gl_WorldRayDirectionEXT, N);
    payload.nextOrigin = hitPos + N * 0.001;
    payload.nextDir    = normalize(reflDir);

    //payload = localLighting;
    //payload.radiance = localLighting;
    payload.radiance += payload.throughput * localLighting;
    float reflectance = 0.75f; //TODO: change this later
    payload.throughput *= reflectance;

    //payload.done = 1u;
    if (reflectance < 0.01) payload.done = 1u;
    else payload.done = 0u;

}
