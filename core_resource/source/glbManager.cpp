
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
    const tinygltf::Accessor* tangentAccessor = nullptr;

    // MATERIAL
    // primitive.material < 0 表示该 primitive 没有显式材质，
    // 应使用 glTF 默认材质。
    int materialId = primitive.material;
    int textureIndex_baseColor = myGlbMaterials[materialId].baseColorTextureIndex;
    int coordIndex_baseColor = myGlbMaterials[materialId].baseColorTexCoord;
    int textureIndex_normal = myGlbMaterials[materialId].normalTextureIndex;
    int coordIndex_normal = myGlbMaterials[materialId].normalTexCoord;
    int textureIndex_metalicRoughness = myGlbMaterials[materialId].metallicRoughnessTextureIndex;
    int coordIndex_metalicRoughness = myGlbMaterials[materialId].metallicRoughnessTexCoord;
    // std::cout << "meshIndex = " << meshIndex << ", materialId = " << materialId 
    //     << ", textureIndex_BaseColor = " << textureIndex_baseColor << ", coordIndex = " << coordIndex_baseColor
    //     << ", textureIndex_normal = " << textureIndex_normal << ", coordIndex = " << coordIndex_normal
    //     << ", textureIndex_metalicRoughness = " << textureIndex_metalicRoughness << ", coordIndex = " << coordIndex_metalicRoughness
    //     << "\n";
    //textureIds.push_back(std::vector<int>{textureIndex_baseColor,textureIndex_normal,textureIndex_metalicRoughness});
    glbMaterialIds.push_back(primitive.material);

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

    // TANGENT
    it = primitive.attributes.find("TANGENT");
    if (it != primitive.attributes.end()) tangentAccessor = &gltfModel.accessors[it->second];
    

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
    // TANGENT
    //-----------------------------
    const float* tangents = nullptr;
    size_t tangentStride = sizeof(float) * 4;

    if (tangentAccessor){
        if (tangentAccessor->componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || tangentAccessor->type != TINYGLTF_TYPE_VEC4)
            throw std::runtime_error("TANGENT accessor must be FLOAT VEC4.");
        
        if (tangentAccessor->count != posAccessor->count) throw std::runtime_error("TANGENT count does not match POSITION count.");
        
        const tinygltf::BufferView& tangentView = gltfModel.bufferViews[tangentAccessor->bufferView];
        const tinygltf::Buffer& tangentBuffer = gltfModel.buffers[tangentView.buffer];

        tangents = reinterpret_cast<const float*>(tangentBuffer.data.data() + tangentView.byteOffset + tangentAccessor->byteOffset);

        // byteStride == 0 means tightly packed.
        tangentStride = tangentView.byteStride != 0 ? tangentView.byteStride : sizeof(float) * 4;
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


         if (tangents){
            const uint8_t* tangentElement = reinterpret_cast<const uint8_t*>(tangents) + i * tangentStride;
            const float* tangent = reinterpret_cast<const float*>(tangentElement);

            //std::cout<<"vertex "<<i<<" tangent = "<<tangent[0]<<", "<<tangent[1]<<", "<<tangent[2]<<", "<<tangent[3]<<std::endl;

            vertex.tangent = glm::vec4(tangent[0], tangent[1], tangent[2], tangent[3]);

            // 可选：防止导出数据因精度问题出现非单位长度
            vertex.tangent = glm::vec4(glm::normalize(glm::vec3(vertex.tangent)), vertex.tangent.w < 0.0f ? -1.0f : 1.0f);
        }
        else{
            // 若模型未提供 tangent，不能安全地默认给任意固定方向。
            // 后续应该根据 POSITION + NORMAL + TEXCOORD_0 生成。
            vertex.tangent = glm::vec4(0.0f);
        }

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

        const uint32_t imageUsage = imageUsages[i];
        const bool isSRGB = (imageUsage & TextureUsage_BaseColor) != 0 || (imageUsage & TextureUsage_Emissive) != 0;
        const VkFormat format = isSRGB? VK_FORMAT_R8G8B8A8_SRGB: VK_FORMAT_R8G8B8A8_UNORM;

        int texture_index = textureManager->PushNewTextureImage(commandPool);//todo: use raytracing(compute) queue family? 
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureManager->SetupTextureImage(texture_index, img.width, img.height, usage, 1, format, 8);
        textureManager->GenerateTextureImageFromTexel(texture_index, 0, false, (void *)img.image.data(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); //put sampler_id to 0 here, can change later

        //const VkDeviceSize sourceSize = img.image.size();
        //std::cout<<"sourceSize = "<<sourceSize<<std::endl;
        //const VkDeviceSize expectedSize = VkDeviceSize(img.width) * img.height * img.component * (img.bits / 8);
        //std::cout<<"expectedSize = "<<expectedSize<<std::endl;
        //return;

        //print a image for debug
        /*
        if (i == 2) {
            const size_t count = std::min<size_t>(100, img.image.size());

            std::cout << "Image 2 metadata:\n";
            std::cout << "  width     = " << img.width << '\n';
            std::cout << "  height    = " << img.height << '\n';
            std::cout << "  component = " << img.component << '\n';
            std::cout << "  bits      = " << img.bits << '\n';
            std::cout << "  bytes     = " << img.image.size() << '\n';

            std::cout << "First " << count << " bytes (hex):\n";

            for (size_t j = 0; j < count; ++j) {
                std::cout
                    << std::hex
                    << std::setw(2)
                    << std::setfill('0')
                    << static_cast<unsigned int>(img.image[j])
                    << ' ';

                // 每行显示 16 个 byte
                if ((j + 1) % 16 == 0) std::cout << '\n';
            }
            std::cout << std::dec << '\n';
        }*/
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

    imageUsages.resize(gltfModel.images.size());
    std::fill(imageUsages.begin(), imageUsages.end(), TextureUsage_None);
        

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
        //myGlbMaterials[materialIndex].alphaMode = material.alphaMode;
        if (material.alphaMode == "MASK") 
            myGlbMaterials[materialIndex].alphaMode = static_cast<int>(AlphaMode::Mask);
        else if (material.alphaMode == "BLEND") 
            myGlbMaterials[materialIndex].alphaMode = static_cast<int>(AlphaMode::Blend);
        else 
            myGlbMaterials[materialIndex].alphaMode = static_cast<int>(AlphaMode::Opaque); // "OPAQUE" 或缺省/未知值
        
        myGlbMaterials[materialIndex].alphaCutoff = static_cast<float>(material.alphaCutoff);
        myGlbMaterials[materialIndex].doubleSided = material.doubleSided;

        //------------------------------------------------------
        // Fill image usages
        //------------------------------------------------------
        auto MarkImageUsage = [&](int textureIndex, uint32_t usage){
            if (textureIndex < 0 || textureIndex >= static_cast<int>(gltfModel.textures.size())) return;
            const int imageIndex = gltfModel.textures[textureIndex].source;
            if (imageIndex < 0 || imageIndex >= static_cast<int>(imageUsages.size())) return;
            imageUsages[imageIndex] |= usage;
        };
        MarkImageUsage(pbr.baseColorTexture.index, TextureUsage_BaseColor);
        MarkImageUsage(pbr.metallicRoughnessTexture.index, TextureUsage_MetallicRoughness);
        MarkImageUsage(material.normalTexture.index, TextureUsage_Normal);
        MarkImageUsage(material.occlusionTexture.index, TextureUsage_Occlusion);
        MarkImageUsage(material.emissiveTexture.index, TextureUsage_Emissive);

        //------------------------------------------------------
        // Check for transmission
        //------------------------------------------------------
        auto it = material.extensions.find("KHR_materials_transmission");
        if (it != material.extensions.end()) {
            const tinygltf::Value& transmissionExt = it->second;

            if (transmissionExt.Has("transmissionFactor")) {
                double transmissionFactor = transmissionExt.Get("transmissionFactor").GetNumberAsDouble();
                std::cout << "Material [" << materialIndex << "] " << "transmissionFactor = " << transmissionFactor << std::endl;
            }

            if (transmissionExt.Has("transmissionTexture")) {
                const tinygltf::Value& texture = transmissionExt.Get("transmissionTexture");

                if (texture.Has("index")) {
                    int textureIndex = texture.Get("index").GetNumberAsInt();
                    std::cout << "  transmissionTexture = " << textureIndex << std::endl;
                }
            }
        }
        else {
            //std::cout << "Material [" << materialIndex << "] has NO transmission extension" << std::endl;
        }   

        //------------------------------------------------------
        // Check for alpha and metal/roughness factor
        //------------------------------------------------------
        // std::cout
        //     << "Material [" << materialIndex << "] "
        //     << "alphaMode = " << material.alphaMode
        //     << ", alphaCutoff = " << material.alphaCutoff
        //     << ", doubleSided = " << material.doubleSided
        //     << ", metallicFactor = " << pbr.metallicFactor
        //     << ", roughnessFactor = " << pbr.roughnessFactor
        //     << std::endl;

        //------------------------------------------------------
        // Summary for this material
        //------------------------------------------------------
        // std::cout
        //     << "Load Material [" << materialIndex << "] "
        //     << "name = " << myGlbMaterials[materialIndex].name
        //     << ", baseColorTex = " << myGlbMaterials[materialIndex].baseColorTextureIndex
        //     << ", metallicRoughnessTex = " << myGlbMaterials[materialIndex].metallicRoughnessTextureIndex
        //     << ", normalTex = " << myGlbMaterials[materialIndex].normalTextureIndex
        //     << ", emissiveTextureIndex = " << myGlbMaterials[materialIndex].emissiveTextureIndex
        //     << ", occlusionTextureIndex = " << myGlbMaterials[materialIndex].occlusionTextureIndex
        //     << std::endl;
    }
    std::cout << "Loaded material count = " << myGlbMaterials.size() << std::endl;

    //print for debug
    // for (size_t imageIndex = 0; imageIndex < imageUsages.size(); ++imageIndex){
    //     const uint32_t usage = imageUsages[imageIndex];
    //     std::cout<< "imageUsages[" << imageIndex << "]"<< " = " << usage<< " : ";
    //     bool hasUsage = false;
    //     if ((usage & TextureUsage_BaseColor) != 0){
    //         std::cout << "BaseColor ";
    //         hasUsage = true;
    //     }
    //     if ((usage & TextureUsage_MetallicRoughness) != 0){
    //         std::cout << "MetallicRoughness ";
    //         hasUsage = true;
    //     }
    //     if ((usage & TextureUsage_Normal) != 0){
    //         std::cout << "Normal ";
    //         hasUsage = true;
    //     }
    //     if ((usage & TextureUsage_Occlusion) != 0){
    //         std::cout << "Occlusion ";
    //         hasUsage = true;
    //     }
    //     if ((usage & TextureUsage_Emissive) != 0){
    //         std::cout << "Emissive ";
    //         hasUsage = true;
    //     }
    //     if (!hasUsage) std::cout << "None";
    //     std::cout << '\n';
    // }
}

GLBMaterial& CGLBManager::getGLBMaterial(int materialId){
    return myGlbMaterials[materialId];
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


