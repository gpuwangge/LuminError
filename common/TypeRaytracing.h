#pragma once
#include "TypeDataBuffer.h"

struct RtMesh{
    int modelId;

    CWxjBuffer vertexBuffer;//use this to get buffer address
    CWxjBuffer indexBuffer;//use this to get buffer address
    VkDeviceAddress vertexAddress = 0;
    VkDeviceAddress indexAddress  = 0;

    uint32_t vertexCount = 0;
    uint32_t indexCount  = 0;
    uint32_t vertexStride = 0;

    VkAccelerationStructureKHR blas = VK_NULL_HANDLE;
    CWxjBuffer blasBuffer;
    VkDeviceAddress blasAddress = 0;//blasDeviceAddress
};

struct RtSphere{
    glm::vec3 center;      // 球心（object space 或 world space，二选一统一）
    float radius;          // 半径
    uint32_t materialIndex;// 材质索引
    //uint32_t pad0;
    //uint32_t pad1;
    //uint32_t pad2;

    // VkAccelerationStructureKHR blas = VK_NULL_HANDLE;
    // CWxjBuffer blasBuffer;
    // VkDeviceAddress blasAddress = 0;//blasDeviceAddress
};

struct GeometryInfoGPU{//remove alignas(16), because shader use scalar(which is 24byte stride)
    VkDeviceAddress vertexBuf;
    VkDeviceAddress indexBuf;
    //  uint32_t materialIndex;//new
    //  uint32_t _pad0;//new
};

static constexpr size_t GEOMETRYINFO_SIZE = 256; //this is the maximum vertex index count for all triangles
struct StructStorageBuffer_GeometryInfo{
    GeometryInfoGPU geometryInfos[GEOMETRYINFO_SIZE]; //each triangle has 3 indices
};