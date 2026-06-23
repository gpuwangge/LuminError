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