#pragma once
#include <tiny_gltf.h>
#include "TypeVertex.h"
#include "TypeRaytracing.h"

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
    //std::vector<std::vector<int>> textureIds; //baseColor, normal, metallicRoughness
    std::vector<uint32_t> glbMaterialIds; //决定哪个glb mesh用哪个glb material
    void LoadGLBMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    VkSamplerAddressMode gltfWrapToVk(int gltfWrap);
    void LoadGLBTexture(VkCommandPool &commandPool, std::vector<VkSampler> &glbSamplers);

    std::vector<GLBMaterial> myGlbMaterials;
    void LoadGLBMaterial();
    GLBMaterial& getGLBMaterial(int materialId);

    //通过读GLB Material，需要获得每种image的usage
    enum TextureUsage : uint32_t{
        TextureUsage_None              = 0,
        TextureUsage_BaseColor         = 1u << 0,
        TextureUsage_MetallicRoughness = 1u << 1,
        TextureUsage_Normal            = 1u << 2,
        TextureUsage_Emissive          = 1u << 3,
        TextureUsage_Occlusion         = 1u << 4,
    };
    std::vector<uint32_t> imageUsages;

    int GetGLBMeshSize(IN int glbIndex);
};

}//namespace