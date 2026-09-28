#pragma once
#include <tiny_gltf.h>
#include "TypeVertex.h"
#include "ILogCore.h"
#include "TypeGLB.h"

namespace LEResource{

class CTextureManager;

struct GLBStruct{
    tinygltf::Model gltfModel;

    std::vector<int> mesh_primitive_size;

    std::vector<GLBMaterial> materials; //这个glb file有哪些材质
    std::vector<uint32_t> glbMaterialIds; //每个mesh对应的材质id

    //to compute resource offsets for each glb
    //int accumulatedMaterialSize;
    int accumulatedMeshSize;
    int accumulatedTextureSize;

    
};

class CGLBManager final{
public:
    CGLBManager() {}
    ~CGLBManager() {}

    LELog::ILogCore *logger = NULL;
    void SetLogger(LELog::ILogCore *logger_){logger = logger_;}

    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkQueue m_raytracingQueue;
    CTextureManager *textureManager;

    tinygltf::TinyGLTF loader;
    
    //glb里面image的数量<=texture的数量，texture.source指向image
    //在生成views和sampler(==texture数量)的时候，需要用这个来正确建立views
    std::vector<int> texture_sources;
    std::vector<int>& GetGLBTextureSources(){
        return texture_sources;
    }

    //每个mesh可能有不止一个primitive，这个是用来给mesh.primitive循环用
    int GetGLBMeshPrimitiveSize(int glbId, int meshId){
        return glbObjects[glbId].mesh_primitive_size[meshId];
    }

    std::string warn;
    std::string err;

    void LoadGLBFromFile(const std::string& filename);

    //std::vector<int> textureIds_baseColor;
    //std::vector<std::vector<int>> textureIds; //baseColor, normal, metallicRoughness
    //std::vector<uint32_t> glbMaterialIds; //决定哪个glb mesh用哪个glb material
    bool LoadGLBMesh(IN int glbId, IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    VkSamplerAddressMode gltfWrapToVk(int gltfWrap);
    void LoadGLBTexture(int glbId, VkCommandPool &commandPool, std::vector<VkSampler> &glbSamplers);

    //std::vector<GLBMaterial> myGlbMaterials;
    std::vector<GLBStruct> glbObjects;
    void LoadGLBMaterial(int glbId);
    GLBMaterial& getGLBMaterial(int glbId, int materialId);

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

    int GetGLBMeshSize(IN int glbId);
    int GetGLBTextureSize(int glbId);
};

}//namespace