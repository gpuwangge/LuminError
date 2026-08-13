
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

void CGLBManager::LoadGLB(const std::string& filename){
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

void CGLBManager::LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D){
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

int CGLBManager::GetMeshSize(IN int glbIndex){
    return gltfModel.meshes.size();//todo: add glbIndex to suppport multiple glb files
}

void CGLBManager::LoadTexture(VkCommandPool &commandPool){
    // -----------------------------------------------------------
    // 1) Load glTF
    // -----------------------------------------------------------
    // tinygltf::Model gltfModel;
    // tinygltf::TinyGLTF loader;
    // std::string err, warn;

    // bool ok = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, "scene.glb");
    // if (!ok) { /* 处理错误 */ }

    // -----------------------------------------------------------
    // 2) 创建所有 VkImage（对应 model.images）
    // -----------------------------------------------------------
    // std::vector<VkImage> vkImages(gltfModel.images.size());
    // std::vector<VkImageView> vkImageViews(gltfModel.images.size());
    // std::vector<VkDeviceMemory> vkImageMems(gltfModel.images.size());

    // textureManager->textureImages[0].m_textureImageBuffer.image;
    // textureManager->textureImages[0].m_textureImageBuffer.view;
    // textureManager->textureImages[0].m_textureImageBuffer.deviceMemory;

    for (size_t i = 0; i < gltfModel.images.size(); ++i) {
        const tinygltf::Image &img = gltfModel.images[i];

        int texture_index = textureManager->PushNewTextureImage(commandPool);//todo: use raytracing(compute) queue family? 
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        textureManager->SetupTextureImage(texture_index, img.width, img.height, usage, 1, VK_FORMAT_R8G8B8A8_SRGB, 8);
        textureManager->GenerateTextureImageFromTexel(texture_index, 0, false, (void *)img.image.data()); //put sampler_id to 0 here, can change later

        //const VkDeviceSize sourceSize = img.image.size();
        //std::cout<<"sourceSize = "<<sourceSize<<std::endl;
        //const VkDeviceSize expectedSize = VkDeviceSize(img.width) * img.height * img.component * (img.bits / 8);
        //std::cout<<"expectedSize = "<<expectedSize<<std::endl;
        //return;
    }

    std::cout<<"textureManager->textureImages.size() = "<<textureManager->textureImages.size()<<std::endl;
    return;
    

    // -----------------------------------------------------------
    // 3) 创建 VkSampler（对应 model.textures 中的 sampler）
    // -----------------------------------------------------------
    std::vector<VkSampler> vkSamplers(gltfModel.textures.size());

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

        vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &vkSamplers[t]);
    }

    // --------------------------------------------------------------------
    // 4) 把 “image + sampler” 绑定到 descriptor set（binding 2）
    // --------------------------------------------------------------------
    std::vector<VkDescriptorImageInfo> texDescInfos(gltfModel.textures.size());

    for (size_t t = 0; t < gltfModel.textures.size(); ++t) {
    const tinygltf::Texture &tex = gltfModel.textures[t];
    uint32_t imgIdx = static_cast<uint32_t>(tex.source); // source → images[idx]

    VkDescriptorImageInfo info{};
    info.imageView = textureManager->textureImages[imgIdx].m_textureImageBuffer.view;// vkImageViews[imgIdx];
    info.sampler = vkSamplers[t];
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    texDescInfos[t] = info;
    }

    // 假设已有 descriptorSet 已经预先分配好了
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //write.dstSet = raytraceDescSet; //todo: 这里需要传入 descriptorSet
    write.dstBinding = 2; // 与 shader 中 binding 对齐
    write.dstArrayElement = 0;
    write.descriptorCount = static_cast<uint32_t>(texDescInfos.size());
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = texDescInfos.data();

    vkUpdateDescriptorSets(m_logicalDevice, 1, &write, 0, nullptr);
}

/*
void CGLBManager::createVkImageFromMemory(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t width,
    uint32_t height,
    int component,
    const unsigned char* pixels,
    size_t pixelByteSize,
    VkFormat format,
    VkImage& outImage,
    VkImageView& outImageView,
    VkDeviceMemory& outImageMemory){
    if (width == 0 || height == 0 || pixels == nullptr || pixelByteSize == 0) 
        throw std::runtime_error("Invalid texture image data");
    
    constexpr uint32_t mipLevels = 1; // 此实现只有一个 mip level。

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

    
    try {
        // 1. 创建并填充 host-visible staging buffer。
        CreateBuffer(device,physicalDevice,static_cast<VkDeviceSize>(pixelByteSize),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,stagingMemory);

        void* mapped = nullptr;
        if (vkMapMemory(device,stagingMemory,0,static_cast<VkDeviceSize>(pixelByteSize),0,&mapped) != VK_SUCCESS)
            throw std::runtime_error("Failed to map staging-buffer memory");
        
        std::memcpy(mapped, pixels, pixelByteSize);
        vkUnmapMemory(device, stagingMemory);

        // 2. 创建 GPU-local image。
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage =
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &outImage) != VK_SUCCESS) 
            throw std::runtime_error("Failed to create texture VkImage");

        VkMemoryRequirements imageMemoryRequirements{};
        vkGetImageMemoryRequirements(device,outImage,&imageMemoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = imageMemoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(
            physicalDevice,
            imageMemoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &outImageMemory) != VK_SUCCESS) 
            throw std::runtime_error("Failed to allocate texture image memory");

        if (vkBindImageMemory(device, outImage, outImageMemory, 0) != VK_SUCCESS) 
            throw std::runtime_error("Failed to bind texture image memory");

        // 3. 录制上传命令。
        VkCommandBuffer cmd = BeginSingleTimeCommands(device,m_commandPool);
        TransitionImageLayout(cmd,outImage,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,mipLevels);
        CopyBufferToImage(cmd,stagingBuffer,outImage,width,height);
        TransitionImageLayout(cmd,outImage,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,mipLevels);
        EndSingleTimeCommands(device,m_commandPool,m_graphicsQueue,cmd);

        // 4. 创建 image view。
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = outImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &viewInfo, nullptr, &outImageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture image view");
        }

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
    }
    catch (...) {
        if (stagingBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, stagingBuffer, nullptr);
        }
        if (stagingMemory != VK_NULL_HANDLE) {
            vkFreeMemory(device, stagingMemory, nullptr);
        }
        if (outImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(device, outImageView, nullptr);
            outImageView = VK_NULL_HANDLE;
        }
        if (outImage != VK_NULL_HANDLE) {
            vkDestroyImage(device, outImage, nullptr);
            outImage = VK_NULL_HANDLE;
        }
        if (outImageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(device, outImageMemory, nullptr);
            outImageMemory = VK_NULL_HANDLE;
        }
        throw;
    }
}*/

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


