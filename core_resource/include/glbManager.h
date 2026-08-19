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

    void LoadGLBFromFile(const std::string& filename);

    //std::vector<int> textureIds_baseColor;
    std::vector<std::vector<int>> textureIds; //baseColor, normal, metallicRoughness
    void LoadGLBMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    VkSamplerAddressMode gltfWrapToVk(int gltfWrap);
    void LoadGLBTexture(VkCommandPool &commandPool, std::vector<VkSampler> &glbSamplers);

    struct MyGLBMaterial{
        glm::vec4 baseColorFactor{1.0f};

        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;

        glm::vec3 emissiveFactor{0.0f};

        int baseColorTextureIndex = -1;
        int metallicRoughnessTextureIndex = -1;
        int normalTextureIndex = -1;
        int occlusionTextureIndex = -1;
        int emissiveTextureIndex = -1;

        int baseColorTexCoord = 0;
        int metallicRoughnessTexCoord = 0;
        int normalTexCoord = 0;
        int occlusionTexCoord = 0;
        int emissiveTexCoord = 0;

        float normalScale = 1.0f;
        float occlusionStrength = 1.0f;

        float alphaCutoff = 0.5f;
        bool doubleSided = false;

        // tinygltf 中通常是 "OPAQUE"、"MASK"、"BLEND"
        std::string alphaMode = "OPAQUE";

        std::string name;
    };
    std::vector<MyGLBMaterial> myGlbMaterials;
    void LoadGLBMaterial();

    int GetGLBMeshSize(IN int glbIndex);
};

}//namespace