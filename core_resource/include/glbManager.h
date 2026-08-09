#pragma once
#include <tiny_gltf.h>
#include "TypeVertex.h"

namespace LEResource{

class CTextureManager;

class CGLBManager final{
public:
    CGLBManager() {}
    ~CGLBManager() {}

    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkQueue m_raytracingQueue;
    CTextureManager *textureManager;

    tinygltf::TinyGLTF loader;
    tinygltf::Model gltfModel;

    std::string warn;
    std::string err;

    void LoadGLB(const std::string& filename);
    void LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    VkSamplerAddressMode gltfWrapToVk(int gltfWrap);
    void createVkImageFromMemory(VkDevice device,
        VkPhysicalDevice physicalDevice,
        uint32_t width,
        uint32_t height,
        int component,
        const unsigned char* pixels,
        size_t pixelByteSize,
        VkFormat format,
        VkImage& outImage,
        VkImageView& outImageView,
        VkDeviceMemory& outImageMemory);
    void LoadTexture();

    int GetMeshSize(IN int glbIndex);
};

}//namespace