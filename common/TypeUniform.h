#pragma once
#include "utility.h"
#include <vulkan/vulkan.h>

//Uniform Naming Rule: PipelineType_DescriptorType_Purpose
enum UniformTypes {
    GRAPHCIS_UNIFORMBUFFER_CUSTOM =                 0x00000001, 
    GRAPHCIS_UNIFORMBUFFER_LIGHTING =               0x00000002,
    GRAPHCIS_UNIFORMBUFFER_MVP =                    0x00000004, //assume app uses one: MVP or VP
    GRAPHCIS_UNIFORMBUFFER_TEXT_MVP =               0x00000008,
    GRAPHCIS_UNIFORMBUFFER_VP =                     0x00000010,
    GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE =    0x00000020,
    GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE =      0x00000040,  //for main camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE = 0x00000080,  //for light camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS = 0x00000100,  //for light camera(Hardware depth bias, use two renderpass, dynamic depth bias)
    //GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2 = 0x00000100,

    COMPUTE_UNIFORMBUFFER_CUSTOM =   0x00000200,
    COMPUTE_STORAGEBUFFER_DOUBLE =   0x00000400,
    COMPUTE_STORAGEIMAGE_TEXTURE =   0x00000800,
    COMPUTE_STORAGEIMAGE_SWAPCHAIN = 0x00001000
};


struct MVPData{
    alignas(16) glm::mat4 model; //16*4=64 bytes
	alignas(16) glm::mat4 mainCameraProj; //16*4=64 bytes
    //alignas(16) glm::mat4 lightCameraProj; //16*4=64 bytes, TODO: alignment?
    alignas(16) glm::mat4 mainCameraView; //16*4=64 bytes
    //alignas(16) glm::mat4 padding; //: MVP size is 192 bytes, but require a multiple of device limit minUniformBufferOffsetAlignment 256.
    //alignas(16) glm::mat4 lightCameraView;
    //Each element of pDynamicOffsets which corresponds to a descriptor binding with type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment
    alignas(16) glm::mat4 padding0; 
    alignas(16) glm::mat4 padding1;
    alignas(16) glm::mat4 padding2;
    alignas(16) glm::mat4 padding3;
    alignas(16) glm::mat4 padding4;
};  

#define MVP_NUM 256
struct MVPUniformBufferObject {
	//MVPData *mvpData; //dynamic doesn't work

    //for now, support two groups of mvpData. Each draw only use one mvpData matrices. Use offset to access.
    //Each mvpData is to be aligned to be 256 bytes
    //Support up to 256 (MVP) objects. buffer size is 256*256(TODO: update to 320) = 65536 bytes; Buffer range is 256 bytes(for each object)
    //65536 bytes = 64 kilo bytes
    MVPData mvpData[MVP_NUM];

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
    MVPUniformBufferObject(){}

    void init(int mvpCount){
       // mvpData = new MVPData[mvpCount];
        //std::cout<<"Created mvpData, mvpCount = "<<mvpCount<<std::endl;
    }

    ~MVPUniformBufferObject(){
        //if(mvpData) delete mvpData;
    }
};

struct TextMVPUniformBufferObject {
    MVPData mvpData[MVP_NUM];
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

struct VPUniformBufferObject {
	//alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return binding;
    }
};

struct ModelPushConstants{
    glm::mat4 model;
};

struct IntPushConstants{
    int value;
};

