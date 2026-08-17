#include "../CommonShaders/rayPipelineCommonStruct.glsl"
#include "../CommonShaders/rayPipelineCommonHit.glsl"

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

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
    //uint materialIndex;
    //uint _pad0;
};

layout(set = 0, binding = 2, scalar) readonly buffer SBOGeometryInfoBuffer {
    GeometryInfo infos[];
} sboGeometryInfos;

/**************
Traingle related functions
**************/
vec3 getTriangleWorldNormal(uint geometryIndex){
    GeometryInfo geo = sboGeometryInfos.infos[geometryIndex];
    uint primID = gl_PrimitiveID * 3u; //gl_PrimitiveID 是当前命中 primitive 的本地 ID，不是全场景统一 primitive ID。

    //这是原来使用sbo来传递数据的code。之前的做法是把所有object的model数据都挤到这两个sbo里面去，后面索引出现了问题。因为rt管线的prim不再是全局索引了。
    //新的方法把GeometryInfo也就是model本身的数据作为sbo传进来，也就是object和model分开。在host端通过设置InstanceCustomIndex来告诉shader这个object(instance)使用哪个model。
    //uint i0 = sboTriangleIndex.indices[primID + 0];
    //uint i1 = sboTriangleIndex.indices[primID + 1];
    //uint i2 = sboTriangleIndex.indices[primID + 2];
    //TriangleVertexInfo v0 = sboTriangleVertex.vertices[i0];
    //TriangleVertexInfo v1 = sboTriangleVertex.vertices[i1];
    //TriangleVertexInfo v2 = sboTriangleVertex.vertices[i2];

    uint i0 = geo.indexBuf.indices[primID + 0];
    uint i1 = geo.indexBuf.indices[primID + 1];
    uint i2 = geo.indexBuf.indices[primID + 2];
    TriangleVertexInfo v0 = geo.vertexBuf.vertices[i0];
    TriangleVertexInfo v1 = geo.vertexBuf.vertices[i1];
    TriangleVertexInfo v2 = geo.vertexBuf.vertices[i2];

    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    vec3 Pobj = v0.position * bc.x + v1.position * bc.y + v2.position * bc.z;
    vec3 Nobj = normalize(v0.normal * bc.x + v1.normal * bc.y + v2.normal * bc.z);

    vec3 P = vec3(gl_ObjectToWorldEXT * vec4(Pobj, 1.0));
    vec3 N = normalize(Nobj * mat3(gl_WorldToObjectEXT)); //vec3 N = normalize(transpose(mat3(gl_WorldToObjectEXT)) * Nobj);
    vec3 V = normalize(-gl_WorldRayDirectionEXT);
    if (dot(N, V) < 0.0) N = -N;

    return N;
}

vec2 getTriangleUV(uint geometryIndex){
    GeometryInfo geo = sboGeometryInfos.infos[geometryIndex];

    uint baseIndex = gl_PrimitiveID * 3u;

    uint i0 = geo.indexBuf.indices[baseIndex + 0u];
    uint i1 = geo.indexBuf.indices[baseIndex + 1u];
    uint i2 = geo.indexBuf.indices[baseIndex + 2u];

    vec2 uv0 = geo.vertexBuf.vertices[i0].uv;
    vec2 uv1 = geo.vertexBuf.vertices[i1].uv;
    vec2 uv2 = geo.vertexBuf.vertices[i2].uv;

    float w0 = 1.0 - bary.x - bary.y;
    return uv0 * w0 + uv1 * bary.x + uv2 * bary.y;

    // TriangleVertexInfo v0 = geo.vertexBuf.vertices[i0];
    // TriangleVertexInfo v1 = geo.vertexBuf.vertices[i1];
    // TriangleVertexInfo v2 = geo.vertexBuf.vertices[i2];

    // vec3 bc = vec3(1.0 - bary.x - bary.y,bary.x,bary.y);
    // return v0.uv * bc.x +v1.uv * bc.y +v2.uv * bc.z;
}

void main(){
    uint instanceIndex = uint(gl_InstanceCustomIndexEXT);
    uint materialIndex = instanceUBO.instances[instanceIndex].materialIndex;
    uint geometryIndex = instanceUBO.instances[instanceIndex].geometryIndex; //thats the model
    uint textureIndex_baseColor = instanceUBO.instances[instanceIndex].textureIndex_baseColor;

    MaterialStruct mat = materialUBO.materials[materialIndex];
    
    //Core
    vec3 Ntri = getTriangleWorldNormal(geometryIndex);
    vec2 uv = getTriangleUV(geometryIndex);
    updatePayload(mat, Ntri, textureIndex_baseColor, uv);
}
