#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 payload;
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

layout(set = 0, binding = 2, std430) readonly buffer SBOTriangleVertex {
   TriangleVertexInfo vertices[];
} sboTriangleVertex;

layout(set = 0, binding = 3, std430) readonly buffer SBOTriangleIndex {
   uint indices[];
} sboTriangleIndex;


void main()
{
    uint prim = gl_PrimitiveID * 3u;

    uint i0 = sboTriangleIndex.indices[prim + 0];
    uint i1 = sboTriangleIndex.indices[prim + 1];
    uint i2 = sboTriangleIndex.indices[prim + 2];

    TriangleVertexInfo v0 = sboTriangleVertex.vertices[i0];
    TriangleVertexInfo v1 = sboTriangleVertex.vertices[i1];
    TriangleVertexInfo v2 = sboTriangleVertex.vertices[i2];

    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    vec3 P = v0.position * bc.x + v1.position * bc.y + v2.position * bc.z;
    vec3 N = normalize(v0.normal * bc.x + v1.normal * bc.y + v2.normal * bc.z);

    //最小安全版：
    //vec3 L = normalize(vec3(0.3, 0.6, 0.7));
    //float diff = max(dot(N, L), 0.0);
    //payload = baseColor * (0.2 + 0.8 * diff);

    vec3 L = normalize(vec3(-0.5, 0.8, 0.6));
    float diff = max(dot(N, L), 0.0);
    vec3 baseColor = vec3(0.8, 0.7, 0.6);
    payload = baseColor * (0.15 + 0.85 * diff);
}

/*
void main() {
    vec3 bc = vec3(1.0 - bary.x - bary.y, bary.x, bary.y);

    vec3 baseColor = vec3(bc.x, bc.y, bc.z);

    vec3 N = vec3(0.0, 0.0, 1.0);                // 三角形法线，先写死
    vec3 L = normalize(vec3(-0.5, 0.8, -0.6));   // 一个最小灯光方向
    float diff = max(dot(N, L), 0.0);

    payload = baseColor * (0.2 + 0.8 * diff);  
}
*/