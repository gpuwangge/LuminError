
#define TINYGLTF_IMPLEMENTATION 
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
// 已经有 Texture.cpp 实现 stb_image
#define TINYGLTF_NO_STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE_IMPLEMENTATION
#include "glbManager.h"
#include "Foundation.h"
#include <iostream>

#include "texture.h"

namespace LEResource{

void CGLBManager::LoadGLBFromFile(const std::string& filename){
    bool ok = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, GLB_PATH + filename);

    if (!warn.empty()) std::cout << warn << std::endl;
    if (!err.empty()) std::cout << err << std::endl;
    if (!ok) std::cout << "Load failed\n";

    std::cout<<"GLB Mesh Size = "<< gltfModel.meshes.size()<< std::endl;
    //for (size_t i = 0; i < model.meshes.size(); ++i) 
    //    std::cout << "Mesh " << i << ": " << model.meshes[i].name << ", primitive count: " << model.meshes[i].primitives.size() << std::endl;
    std::cout<<"GLB Material Size = "<< gltfModel.materials.size()<< std::endl;
    std::cout<<"GLB Image Size = "<< gltfModel.images.size()<< std::endl;
    std::cout<<"GLB Texture Size = "<< gltfModel.textures.size()<< std::endl;
    std::cout<<"GLB Sampler Size = "<<gltfModel.samplers.size()<<std::endl;
}

void CGLBManager::LoadGLBMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D){
    //unsigned int meshIndex = 100;
    //unsigned int primitiveIndex = 0;

    auto& mesh = gltfModel.meshes[meshIndex];
    // auto& material = model.materials[0];
    // auto& image = model.images[0];
    // auto& texture = model. textures  [0];

    if (mesh.primitives.empty()) {
        throw std::runtime_error("Mesh contains no primitives.");
    }

    // 这里只读取一个 primitive
    const tinygltf::Primitive& primitive = mesh.primitives[primitiveIndex];

    const tinygltf::Accessor* posAccessor = nullptr;
    const tinygltf::Accessor* normalAccessor = nullptr;
    const tinygltf::Accessor* texAccessor = nullptr;

    // MATERIAL
    // primitive.material < 0 表示该 primitive 没有显式材质，
    // 应使用 glTF 默认材质。
    int materialId = primitive.material;
    int textureIndex_BaseColor = myGlbMaterials[materialId].baseColorTextureIndex;
    std::cout << "meshIndex = " << meshIndex << ", materialId = " << materialId << ", textureIndex_BaseColor = " << textureIndex_BaseColor<< "\n";
    textureIds_baseColor.push_back(textureIndex_BaseColor);

    //std::cout << "primitiveIndex = " << primitiveIndex << "\n";
    //std::cout << "materialId     = " << materialId << "\n";
    // if (materialId >= 0){
    //     const tinygltf::Material& material = gltfModel.materials[materialId];
    //     std::cout << "material name  = " << material.name << "\n";
    // }
    // else std::cout << "material       = default glTF material\n";
    

    // POSITION
    auto it = primitive.attributes.find("POSITION");
    if (it != primitive.attributes.end()) posAccessor = &gltfModel.accessors[it->second];

    if (!posAccessor) throw std::runtime_error("Mesh has no POSITION.");

    // NORMAL
    it = primitive.attributes.find("NORMAL");
    if (it != primitive.attributes.end()) normalAccessor = &gltfModel.accessors[it->second];

    // TEXCOORD_0
    it = primitive.attributes.find("TEXCOORD_0");
    if (it != primitive.attributes.end()) texAccessor = &gltfModel.accessors[it->second];

    //-----------------------------
    // POSITION
    //-----------------------------
    const tinygltf::BufferView& posView = gltfModel.bufferViews[posAccessor->bufferView];
    const tinygltf::Buffer& posBuffer = gltfModel.buffers[posView.buffer];
    const float* positions = reinterpret_cast<const float*>(posBuffer.data.data() + posView.byteOffset + posAccessor->byteOffset);

    //-----------------------------
    // NORMAL
    //-----------------------------
    const float* normals = nullptr;

    if (normalAccessor){
        const tinygltf::BufferView& normalView = gltfModel.bufferViews[normalAccessor->bufferView];
        const tinygltf::Buffer& normalBuffer = gltfModel.buffers[normalView.buffer];
        normals = reinterpret_cast<const float*>(normalBuffer.data.data() + normalView.byteOffset + normalAccessor->byteOffset);
    }

    //-----------------------------
    // TEXCOORD
    //-----------------------------
    const float* texcoords = nullptr;
    if (texAccessor){
        const tinygltf::BufferView& texView = gltfModel.bufferViews[texAccessor->bufferView];
        const tinygltf::Buffer& texBuffer = gltfModel.buffers[texView.buffer];
        texcoords = reinterpret_cast<const float*>(texBuffer.data.data() + texView.byteOffset + texAccessor->byteOffset);
    }

    //-----------------------------
    // Vertex
    //-----------------------------
    vertices3D.clear();
    size_t vertexCount = posAccessor->count;
    for (size_t i = 0; i < vertexCount; ++i){
        Vertex3D vertex{};
        vertex.pos = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
        vertex.color = glm::vec3(1.0f);

        if (normals) vertex.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
        else vertex.normal = glm::vec3(0,1,0);
        
        if (texcoords) vertex.texCoord = glm::vec2(texcoords[i * 2 + 0], 1.0f - texcoords[i * 2 + 1]);
        else vertex.texCoord = glm::vec2(0);

        vertices3D.push_back(vertex);
    }

    //-----------------------------
    // Index
    //-----------------------------
    indices3D.clear();
    if (primitive.indices >= 0){
        const tinygltf::Accessor& indexAccessor = gltfModel.accessors[primitive.indices];
        const tinygltf::BufferView& indexView = gltfModel.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexView.buffer];
        const unsigned char* data = indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset;

        switch (indexAccessor.componentType){
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
            const uint16_t* src = reinterpret_cast<const uint16_t*>(data);
            for (size_t i = 0; i < indexAccessor.count; ++i) indices3D.push_back(src[i]);
            break;
        }

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        {
            const uint32_t* src = reinterpret_cast<const uint32_t*>(data);
            indices3D.assign(src, src + indexAccessor.count);
            break;
        }

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        {
            const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
            for (size_t i = 0; i < indexAccessor.count; ++i) indices3D.push_back(src[i]);
            break;
        }

        default:
            throw std::runtime_error("Unsupported index type.");
        }
    }
}

int CGLBManager::GetGLBMeshSize(IN int glbIndex){
    return gltfModel.meshes.size();//todo: add glbIndex to suppport multiple glb files
}

void CGLBManager::LoadGLBTexture(VkCommandPool &commandPool, std::vector<VkSampler> &glbSamplers){
    for (size_t i = 0; i < gltfModel.images.size(); ++i) {
        const tinygltf::Image &img = gltfModel.images[i];

        int texture_index = textureManager->PushNewTextureImage(commandPool);//todo: use raytracing(compute) queue family? 
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureManager->SetupTextureImage(texture_index, img.width, img.height, usage, 1, VK_FORMAT_R8G8B8A8_SRGB, 8);
        textureManager->GenerateTextureImageFromTexel(texture_index, 0, false, (void *)img.image.data(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); //put sampler_id to 0 here, can change later

        //const VkDeviceSize sourceSize = img.image.size();
        //std::cout<<"sourceSize = "<<sourceSize<<std::endl;
        //const VkDeviceSize expectedSize = VkDeviceSize(img.width) * img.height * img.component * (img.bits / 8);
        //std::cout<<"expectedSize = "<<expectedSize<<std::endl;
        //return;
    }

    std::cout<<"textureManager->textureImages.size() = "<<textureManager->textureImages.size()<<std::endl;
    
    // -----------------------------------------------------------
    // 创建 VkSampler（对应 model.textures 中的 sampler）
    // -----------------------------------------------------------
    //std::vector<VkSampler> vkSamplers(gltfModel.textures.size());
    glbSamplers.resize(gltfModel.textures.size());

    for (size_t t = 0; t < gltfModel.textures.size(); ++t) {
        const tinygltf::Texture &tex = gltfModel.textures[t];
        const tinygltf::Sampler *smp = nullptr;

        if (tex.sampler >= 0) {
        smp = &gltfModel.samplers[tex.sampler];
        } else {
        //smp = &defaultSampler; // 如果 glTF 没写 sampler，用默认值
        }

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        // ---- 填写过滤方式 ----------------------------------------
        samplerInfo.magFilter =
        (smp->magFilter == TINYGLTF_TEXTURE_FILTER_NEAREST)
        ? VK_FILTER_NEAREST
        : VK_FILTER_LINEAR;

        // minFilter 需要考虑 mipmap，下面是简化的映射
        samplerInfo.minFilter =
        (smp->minFilter == TINYGLTF_TEXTURE_FILTER_NEAREST ||
        smp->minFilter == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST ||
        smp->minFilter == TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR)
        ? VK_FILTER_NEAREST
        : VK_FILTER_LINEAR;

        // ---- 填写包裹方式 ----------------------------------------
        samplerInfo.addressModeU = gltfWrapToVk(smp->wrapS);
        samplerInfo.addressModeV = gltfWrapToVk(smp->wrapT);
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        // ---- Mipmap 选项 ----------------------------------------
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        constexpr uint32_t imgMipLevels = 1; //assume no mipmap for now
        samplerInfo.maxLod = static_cast<float>(imgMipLevels - 1);

        vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &glbSamplers[t]); //need destroy with vkDestroySampler
    }

    std::cout<<"Read from gltfModel.textures: glbSamplers.size() = "<<glbSamplers.size()<<std::endl;
}

void CGLBManager::LoadGLBMaterial(){
    bool bGLBMaterialVerbose = false;

    myGlbMaterials.clear();
    myGlbMaterials.resize(gltfModel.materials.size());

    for (int materialIndex = 0; materialIndex < static_cast<int>(gltfModel.materials.size()); ++materialIndex){
        if (materialIndex < 0 || materialIndex >= static_cast<int>(gltfModel.materials.size())) throw std::runtime_error("Invalid material index.");

        const tinygltf::Material& material = gltfModel.materials[materialIndex];
        const tinygltf::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

        //------------------------------------------------------
        // Basic info
        //------------------------------------------------------
        myGlbMaterials[materialIndex].name = material.name;

        //------------------------------------------------------
        // PBR metallic-roughness
        //------------------------------------------------------
        if (pbr.baseColorFactor.size() == 4){
            myGlbMaterials[materialIndex].baseColorFactor = glm::vec4(
                static_cast<float>(pbr.baseColorFactor[0]),
                static_cast<float>(pbr.baseColorFactor[1]),
                static_cast<float>(pbr.baseColorFactor[2]),
                static_cast<float>(pbr.baseColorFactor[3])
            );
        }

        myGlbMaterials[materialIndex].metallicFactor = static_cast<float>(pbr.metallicFactor);
        myGlbMaterials[materialIndex].roughnessFactor = static_cast<float>(pbr.roughnessFactor);
        myGlbMaterials[materialIndex].baseColorTextureIndex = pbr.baseColorTexture.index;
        myGlbMaterials[materialIndex].baseColorTexCoord = pbr.baseColorTexture.texCoord;
        myGlbMaterials[materialIndex].metallicRoughnessTextureIndex = pbr.metallicRoughnessTexture.index;
        myGlbMaterials[materialIndex].metallicRoughnessTexCoord = pbr.metallicRoughnessTexture.texCoord;

        //------------------------------------------------------
        // Normal
        //------------------------------------------------------
        myGlbMaterials[materialIndex].normalTextureIndex = material.normalTexture.index;
        myGlbMaterials[materialIndex].normalTexCoord = material.normalTexture.texCoord;
        myGlbMaterials[materialIndex].normalScale = static_cast<float>(material.normalTexture.scale);

        //------------------------------------------------------
        // Occlusion
        //------------------------------------------------------
        myGlbMaterials[materialIndex].occlusionTextureIndex = material.occlusionTexture.index;
        myGlbMaterials[materialIndex].occlusionTexCoord = material.occlusionTexture.texCoord;
        myGlbMaterials[materialIndex].occlusionStrength = static_cast<float>(material.occlusionTexture.strength);

        //------------------------------------------------------
        // Emissive
        //------------------------------------------------------
        if (material.emissiveFactor.size() == 3){
            myGlbMaterials[materialIndex].emissiveFactor = glm::vec3(
                static_cast<float>(material.emissiveFactor[0]),
                static_cast<float>(material.emissiveFactor[1]),
                static_cast<float>(material.emissiveFactor[2])
            );
        }
        myGlbMaterials[materialIndex].emissiveTextureIndex = material.emissiveTexture.index;
        myGlbMaterials[materialIndex].emissiveTexCoord = material.emissiveTexture.texCoord;

        //------------------------------------------------------
        // Alpha / raster state
        //------------------------------------------------------
        myGlbMaterials[materialIndex].alphaMode = material.alphaMode;
        myGlbMaterials[materialIndex].alphaCutoff = static_cast<float>(material.alphaCutoff);
        myGlbMaterials[materialIndex].doubleSided = material.doubleSided;

        std::cout
            << "Load Material [" << materialIndex << "] "
            << "name = " << myGlbMaterials[materialIndex].name
            << ", baseColorTex = "
            << myGlbMaterials[materialIndex].baseColorTextureIndex
            << ", metallicRoughnessTex = "
            << myGlbMaterials[materialIndex].metallicRoughnessTextureIndex
            << ", normalTex = "
            << myGlbMaterials[materialIndex].normalTextureIndex
            << std::endl;
    }
    std::cout << "Loaded material count = " << myGlbMaterials.size() << std::endl;

        /*
        const tinygltf::Material& material = gltfModel.materials[materialIndex];
        const tinygltf::PbrMetallicRoughness& pbr = material.pbrMetallicRoughness;

        auto PrintTextureInfo = [](const char* name, const tinygltf::TextureInfo& textureInfo){
            std::cout << "  " << name << ":\n";

            if (textureInfo.index >= 0){
                std::cout << "    texture index = " << textureInfo.index << "\n";
                std::cout << "    texCoord set = " << textureInfo.texCoord << "\n";
            }
            else
                std::cout << "    none\n";
        };

        if(bGLBMaterialVerbose){
            std::cout << "========================================\n";
            std::cout << "Material Index = " << materialIndex << "\n";
            std::cout << "Material Name  = " << material.name << "\n";
        }

        //----------------------------------------------------------
        // PBR Metallic-Roughness
        //----------------------------------------------------------
        if(bGLBMaterialVerbose) std::cout << "[PBR Metallic-Roughness]\n";

        if(bGLBMaterialVerbose){
            const std::vector<double>& baseColor = pbr.baseColorFactor;
            std::cout << "  baseColorFactor = ("
                    << baseColor[0] << ", "
                    << baseColor[1] << ", "
                    << baseColor[2] << ", "
                    << baseColor[3] << ")\n";

            std::cout << "  metallicFactor  = " << pbr.metallicFactor << "\n";
            std::cout << "  roughnessFactor = " << pbr.roughnessFactor << "\n";

            PrintTextureInfo("baseColorTexture", pbr.baseColorTexture);
            PrintTextureInfo("metallicRoughnessTexture",
                            pbr.metallicRoughnessTexture);
        }

        //----------------------------------------------------------
        // Normal texture
        //----------------------------------------------------------
        if(bGLBMaterialVerbose){
            std::cout << "[Normal]\n";
            if (material.normalTexture.index >= 0){
                std::cout << "  normalTexture index      = " << material.normalTexture.index << "\n";
                std::cout << "  normalTexture texCoord   = "  << material.normalTexture.texCoord << "\n";
                std::cout << "  normalTexture scale      = "  << material.normalTexture.scale << "\n";
            }
            else
                std::cout << "  normalTexture = none\n";
        }
            

        //----------------------------------------------------------
        // Occlusion texture
        //----------------------------------------------------------
        if(bGLBMaterialVerbose){
            std::cout << "[Occlusion]\n";
            if (material.occlusionTexture.index >= 0){
                std::cout << "  occlusionTexture index   = " << material.occlusionTexture.index << "\n";
                std::cout << "  occlusionTexture texCoord= "  << material.occlusionTexture.texCoord << "\n";
                std::cout << "  occlusionTexture strength= "  << material.occlusionTexture.strength << "\n";
            }
            else
                std::cout << "  occlusionTexture = none\n";
        }

        //----------------------------------------------------------
        // Emissive
        //----------------------------------------------------------
        if(bGLBMaterialVerbose){
            std::cout << "[Emissive]\n";
            std::cout << "  emissiveFactor = ("
                    << material.emissiveFactor[0] << ", "
                    << material.emissiveFactor[1] << ", "
                    << material.emissiveFactor[2] << ")\n";

            PrintTextureInfo("emissiveTexture", material.emissiveTexture);
        }

        //----------------------------------------------------------
        // Rasterization / alpha mode
        //----------------------------------------------------------
        if(bGLBMaterialVerbose){
            std::cout << "[Render State]\n";
            std::cout << "  alphaMode  = " << material.alphaMode << "\n";
            std::cout << "  alphaCutoff= " << material.alphaCutoff << "\n";
            std::cout << "  doubleSided= "
                    << (material.doubleSided ? "true" : "false") << "\n";

            std::cout << "========================================\n";
        }
    }*/
}

VkSamplerAddressMode CGLBManager::gltfWrapToVk(int gltfWrap){
    switch (gltfWrap) {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

    case TINYGLTF_TEXTURE_WRAP_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;

    default:
        // glTF 2.0 默认 wrapS / wrapT 是 REPEAT
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

}//namespace


