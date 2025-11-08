#pragma once
#include "utility.h"
#include <vulkan/vulkan.h>

//Uniform Naming Rule: PipelineType_DescriptorType_Purpose
enum UniformTypes {
    GRAPHCIS_UNIFORMBUFFER_GLOBAL =                 0x00000001,
    GRAPHCIS_UNIFORMBUFFER_OBJECT_DYNAMIC =         0x00000002, //assume app uses one: MVP or VP
    GRAPHCIS_UNIFORMBUFFER_TEXT_DYNAMIC =           0x00000004,
    GRAPHCIS_UNIFORMBUFFER_LIGHTING =               0x00000008,
    GRAPHCIS_UNIFORMBUFFER_CUSTOM =                 0x00000010,
    //GRAPHCIS_UNIFORMBUFFER_VP =                     0x00000020,
    GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE =    0x00000040,
    GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE =      0x00000080,  //for main camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE = 0x00000100,  //for light camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS = 0x00000200,  //for light camera(Hardware depth bias, use two renderpass, dynamic depth bias)
    //GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2 = 0x00000100,

    COMPUTE_UNIFORMBUFFER_CUSTOM =   0x00000400,
    COMPUTE_STORAGEBUFFER_DOUBLE =   0x00000800,
    COMPUTE_STORAGEIMAGE_TEXTURE =   0x00001000,
    COMPUTE_STORAGEIMAGE_SWAPCHAIN = 0x00002000
};

struct GlobalUniformBufferObject {
    alignas(16) glm::mat4 mainCameraProj; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraView; //16*4=64 bytes
    alignas(4) float tanHalfFovY;          // 4 bytes
    alignas(4) float aspect;               // 4 bytes
    alignas(4) float padding[30];          // 4 × 30 = 120 bytes
    // Sum: 64 + 64 + 4 + 4 + 120 = 256 bytes

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
};

//!In graphicsDescriptor.cpp and rendererCore.cpp, assume MVPData is 256 bytes. If change size here, need adjust those sourcefiles
struct ObjectTextData{
    alignas(16) glm::mat4 model; //16*4=64 bytes
    alignas(4) bool identityCameraProj; // 4 byte
    alignas(4) bool identityCameraView; // 4 byte
    alignas(4) bool padding_bool[2];       // 8 bytes
    alignas(16) glm::vec4 padding[11];     // 16 × 11 = 176 bytes
    // sum: 64 + 16 + 176 = 256 bytes 
};

#define OBJECT_TEXT_NUM 256
struct StructObjectUniformBuffer {
	//MVPData *mvpData; //dynamic doesn't work

    //for now, support two groups of mvpData. Each draw only use one mvpData matrices. Use offset to access.
    //Each mvpData is to be aligned to be 256 bytes
    //Support up to 256 (MVP) objects. buffer size is 256*256(TODO: update to 320) = 65536 bytes; Buffer range is 256 bytes(for each object)
    //65536 bytes = 64 kilo bytes
    ObjectTextData mvpData[OBJECT_TEXT_NUM];
    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }

public:
    StructObjectUniformBuffer(){}
    //void init(int mvpCount){}
    ~StructObjectUniformBuffer(){}
};

struct TextMVPUniformBufferObject {
    ObjectTextData mvpData[OBJECT_TEXT_NUM];
    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
public:
    TextMVPUniformBufferObject(){}
    ~TextMVPUniformBufferObject(){}
};

// struct VPUniformBufferObject {
// 	//alignas(16) glm::mat4 model;
// 	alignas(16) glm::mat4 view;
// 	alignas(16) glm::mat4 proj;

//     static VkDescriptorSetLayoutBinding GetBinding(){
//         VkDescriptorSetLayoutBinding binding;
//         binding.binding = 0;
// 		binding.descriptorCount = 1;
// 		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
// 		binding.pImmutableSamplers = nullptr;
// 		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//         return binding;
//     }
// };

struct ModelPushConstants{
    glm::mat4 model;
};

struct IntPushConstants{
    int value;
};

