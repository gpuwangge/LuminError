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
    //vec3 tangent;
    //float _padding2;
    vec4 tangent; // xyz = tangent direction, w = handedness (+1 / -1)
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

struct SurfaceFrame{
    vec3 P;
    vec3 N_geometric; // triangle geometric normal, world space
    vec3 N_interpolate; // interpolated vertex normal, world space
    vec3 T;
    vec3 B;
    vec2 uv;
};
SurfaceFrame getTriangleSurfaceFrame(uint geometryIndex){ //根据vertex attribute，计算每个三角形的uv, normal, TBN。不翻转。
    //第一步：读取数据，计算重心
    GeometryInfo geo = sboGeometryInfos.infos[geometryIndex];

    uint baseIndex = gl_PrimitiveID * 3u;

    uint i0 = geo.indexBuf.indices[baseIndex + 0u];
    uint i1 = geo.indexBuf.indices[baseIndex + 1u];
    uint i2 = geo.indexBuf.indices[baseIndex + 2u];

    TriangleVertexInfo v0 = geo.vertexBuf.vertices[i0];
    TriangleVertexInfo v1 = geo.vertexBuf.vertices[i1];
    TriangleVertexInfo v2 = geo.vertexBuf.vertices[i2];

    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    SurfaceFrame frame;

    //第二步：计算uv
    frame.uv = v0.uv * bc.x + v1.uv * bc.y + v2.uv * bc.z;

    //第三步：object position插值
    // Object space interpolated position / UV
    vec3 Pobj = v0.position * bc.x + v1.position * bc.y + v2.position * bc.z;
    frame.P = vec3(gl_ObjectToWorldEXT * vec4(Pobj, 1.0)); // position: point transform

    //第四步：shade normal插值
    // 不要过早 normalize；先插值，之后再 normalize。
    vec3 Nobj = v0.normal * bc.x + v1.normal * bc.y + v2.normal * bc.z;
    mat3 normalMatrix = transpose(mat3(gl_WorldToObjectEXT));
    frame.N_interpolate = safeNormalize(normalMatrix * Nobj);

    //Notes:
    // normal: inverse-transpose(model)。
    // 对 Vulkan ray tracing builtin，Nobj * mat3(gl_WorldToObjectEXT)
    // 是 row-vector 写法，与 transpose(mat3(gl_WorldToObjectEXT)) * Nobj 等价。
    
    //第五步：geometric normal计算，每个三角形上的geometric normal都一样，跟hit position无关
    vec3 e1Obj = v1.position - v0.position;
    vec3 e2Obj = v2.position - v0.position;
    vec3 NgObj = safeNormalize(cross(e1Obj, e2Obj));
    frame.N_geometric = normalize(normalMatrix * NgObj);
    

    // 防止 authored vertex normals 落在真实 face normal 的反半球。
    // if (dot(frame.N_interpolate, frame.N_geometric) < 0.0) {
    //     frame.N_interpolate = -frame.N_interpolate;
    // }

    // 用几何法线决定 hit side。
    // vec3 V = safeNormalize(-gl_WorldRayDirectionEXT);
    // if (dot(frame.N_geometric, V) < 0.0) {
    //     frame.N_geometric = -frame.N_geometric;
    //     frame.N_interpolate = -frame.N_interpolate;
    // }

    //第六步：计算tangent vector
    // tangent: direction 走 object-to-world 的线性部分。
    // 不能把 tangent 当作 normal 一样直接走 normalMatrix。
    vec3 Tobj = v0.tangent.xyz * bc.x + v1.tangent.xyz * bc.y + v2.tangent.xyz * bc.z;
    mat3 objectToWorld3x3 = mat3(gl_ObjectToWorldEXT);
    frame.T = objectToWorld3x3 * Tobj;

    // 非均匀缩放、插值误差都会破坏 N 和 T 的正交性。
    // Gram-Schmidt 修正后再 normalize。
    frame.T = safeNormalize(frame.T - frame.N_interpolate * dot(frame.N_interpolate, frame.T));


    //第七步：计算Bi tangent
    // .w 只是 +/-1，但对 interpolation 做符号选择更稳妥。
    float handedness = v0.tangent.w * bc.x + v1.tangent.w * bc.y + v2.tangent.w * bc.z;
    handedness = handedness < 0.0 ? -1.0 : 1.0;
    // glTF tangent.w 保留 UV mirror 的 handedness。
    //frame.B = safeNormalize(cross(frame.N_interpolate, frame.T)) * handedness;
    frame.B = cross(frame.N_interpolate, frame.T) * handedness;

    // 保持你当前的 two-sided hit normal 行为。
    // vec3 V = normalize(-gl_WorldRayDirectionEXT);
    // if (dot(frame.N_geometric, V) < 0.0){
    //     frame.N_geometric = -frame.N_geometric;

    //     // 关键：如果 N 翻转，B 也必须翻转。
    //     // T 不变；否则 TBN 的方向关系会被破坏。
    //     frame.B = -frame.B;
    // }

    return frame;
}

vec3 decodeNormalMap(vec3 encodedNormal){
    return encodedNormal * 2.0 - 1.0;
}

/*
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
}*/

// #ifndef DISABLE_TEXTURE
// vec4 SampleTexture2(uint texId, vec2 uv){
//     texId = min(texId, MAX_GLOBAL_TEXTURES - 1u);
//     return texture(texarray[texId], uv);
// }
// #endif

void main(){
    InstanceStruct ins = instanceUBO.instances[uint(gl_InstanceCustomIndexEXT)];

    //uint materialIndex = instanceUBO.instances[instanceIndex].materialIndex;
    //uint geometryIndex = instanceUBO.instances[instanceIndex].geometryIndex; //thats the model
    

    // uint textureIndex_baseColor = instanceUBO.instances[instanceIndex].textureIndex_baseColor;
    //uint textureIndex_normal = instanceUBO.instances[instanceIndex].textureIndex_normal;
    // uint textureIndex_metallicRoughness = instanceUBO.instances[instanceIndex].textureIndex_metallicRoughness;

    // float metallicFactor = instanceUBO.instances[instanceIndex].metallicFactor;
    // float roughnessFactor = instanceUBO.instances[instanceIndex].roughnessFactor;
    // uint alphaMode = instanceUBO.instances[instanceIndex].alphaMode;
    // float alphaCutoff = instanceUBO.instances[instanceIndex].alphaCutoff;
    // float doubleSided = instanceUBO.instances[instanceIndex].doubleSided;


    MaterialStruct mat = materialUBO.materials[ins.materialIndex];
    
    //Core
    vec3 Ntri = getTriangleWorldNormal(ins.geometryIndex); //legacy
    //vec2 uv = getTriangleUV(geometryIndex); //legacy
    SurfaceFrame frame = getTriangleSurfaceFrame(ins.geometryIndex);

    //翻转
    vec3 I = safeNormalize(gl_WorldRayDirectionEXT);
    bool frontFace = dot(I, frame.N_geometric) < 0.0;
    // 让最终使用的 normal 指向 ray origin / view side。
    if (!frontFace) {//背面命中
        frame.N_geometric   = -frame.N_geometric;
        frame.N_interpolate = -frame.N_interpolate;
        frame.T = -frame.T;
    }

    //TODO：这段代码看起来跟triangle无关，需放在updatePayload?
    vec3 Ns = frame.N_interpolate;
    vec3 Ng = frame.N_geometric;
    //从 normal map 取出一个切线空间法线，
    //再通过 TBN 矩阵把它转换为世界空间法线，
    //最后作为光照、反射、BRDF 计算使用的表面法线。
    //normal map 的 RGB 通常保存 tangent-space 的 XYZ 方向，而不是颜色。
    //这段代码最终得到的是与后续光照计算处于同一坐标系的 shading normal (N)
#ifndef DISABLE_TEXTURE
    if (ins.textureIndex_normal >= 0){
        // 替换成你自己的 bindless texture sampling 函数。
        //用当前 hit point 的 UV 去采样 normal texture。
        vec3 encodedNormal = SampleTexture(ins.textureIndex_normal, frame.uv).xyz;
        
        //这一步把 texture 的 [0, 1] 范围还原到 normal vector 的 [-1, 1] 范围：
        vec3 Nts = decodeNormalMap(encodedNormal);

        // Test: 临时夸张强度，正常通常为 1
        // Nts.xy *= 8.0;
        // Nts = normalize(Nts);

        //Test: 固定一个Nts
        //Nts = vec3(0.0, 0.0, 1.0);
        //vec3 Ns = normalize(mat3(frame.T, frame.B, frame.N) * Nts);

        // 若纹理含 normalTexture.scale，应缩放 xy，而不是 xyz。
        // Nts.xy *= mat.normalScale;

        //构造 TBN 基底矩阵
        mat3 TBN = mat3(frame.T, frame.B, frame.N_interpolate);

        // tangent-space -> world-space
        //把 tangent-space normal 变成 world-space normal
        Ns = normalize(TBN * Nts);

        // 保证 shading normal 面向入射光线/观察方向。
        //实现一个 two-sided shading normal：
        //无论射线从三角形正面还是反面命中，
        //都让最终 shading normal 朝向射线来处。
        // vec3 V = normalize(-gl_WorldRayDirectionEXT);
        // if (dot(Ng, V) < 0.0) Ng = -Ng;
        // if (dot(Ns, V) < 0.0) Ns = -Ns;

    }
#endif

    //Test 
    //updatePayload(mat, Ntri, Ntri, textureIndex_baseColor, textureIndex_normal, textureIndex_metallicRoughness, frame.uv);
    //updatePayload(mat, Ns, Ns, textureIndex_baseColor, textureIndex_normal, textureIndex_metallicRoughness, frame.uv);
    //updatePayload(mat, Ng, Ng, textureIndex_baseColor, textureIndex_normal, textureIndex_metallicRoughness, frame.uv);

    

    updatePayload(mat, Ng, Ns, ins, frame.uv);

    //Notes
    //例如一个 low-poly sphere：
    //N_geometric 会让每个三角形呈现平面块状光照。
    //Ntri / N_interpolate 通过共享顶点 normal 伪造平滑球面光照。
}

