#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeUniform.h"
#include "TypeLight.h"
#include <array>

class CWxjBuffer;

namespace LEGameEngine{
    class IGameEngine;
}

namespace LERenderer{

class CGraphicsDescriptorManager{
public:
    CGraphicsDescriptorManager(){};
    ~CGraphicsDescriptorManager(){};

    LEGameEngine::IGameEngine* game;

    /************
     * Pool
     ************/
    static int graphicsUniformTypes;
    static VkDescriptorPool graphicsDescriptorPool;
    static std::vector<VkDescriptorPoolSize> graphicsDescriptorPoolSizes;
    void createDescriptorPool(unsigned int object_textbox_count = 0);
    
    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> graphicsBindings;
    static VkDescriptorSetLayout descriptorSetLayout_general;
    static VkDescriptorSetLayout descriptorSetLayout_textureImageSampler;
    void createDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    void createDescriptorSetLayout_TextureImageSampler();

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets_general; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    //void createDescriptorSets_General(VkImageView depthImageView, VkImageView lightDepthImageView0, VkImageView lightDepthImageView1);
    void createDescriptorSets_General(VkImageView depthImageView, std::vector<VkImageView> &depthlight_imageviews);

    /************
     * 1 GRAPHCIS_UNIFORMBUFFER_GLOBAL
     ************/
    static std::vector<CWxjBuffer> m_globalUniformBuffers; 
	static std::vector<void*> m_globalUniformBuffersMapped;
    static VkDeviceSize m_globalUniformBufferSize;
    void addGlobalUniformBuffer();
    void uploadGlobalUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 2 GRAPHCIS_UNIFORMBUFFER_MVP (for object)
     ************/
    static std::vector<CWxjBuffer> mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
	//static std::vector<void*> mvpUniformBuffersMapped;
    void addMVPUniformBuffer(std::vector<void*>& mvpUniformBuffersMapped);
    //static MVPUniformBufferObject mvpUBO;

    /************
     * 3 GRAPHCIS_UNIFORMBUFFER_TEXT_MVP (for textbox)
     ************/
    static std::vector<CWxjBuffer> textMVPUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
    void addTextMVPUniformBuffer(std::vector<void*>& textMVPUniformBuffersMapped);

    /************
     * 4 GRAPHCIS_UNIFORMBUFFER_LIGHTING (for light)
     ************/
    //static LightingUniformBufferObject m_lightingUBO;
    static std::vector<CWxjBuffer> m_lightingUniformBuffers; 
	//static std::vector<void*> m_lightingUniformBuffersMapped;
    static VkDeviceSize m_lightingUniformBufferSize;
    void addLightingUniformBuffer(std::vector<void*>& lightingUniformBuffersMapped);

    /************
     * 5 GRAPHCIS_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    void uploadCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 5.5 GRAPHCIS_UNIFORMBUFFER_VP (temp)
     ************/
    //static std::vector<CWxjBuffer> vpUniformBuffers; 
	//static std::vector<void*> vpUniformBuffersMapped;
    //void addVPUniformBuffer(std::vector<void*>& vpUniformBuffersMapped);
    //static VPUniformBufferObject vpUBO;
    //static bool CheckMVP(); //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset

    /************
     * 6 GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE
     ************/
    static unsigned int textureImageSamplerSize;
    static std::vector<VkSampler> textureImageSamplers;
    void addTextureImageSamplerUniformBuffer(std::vector<int> &mipLevels, std::vector<std::array<bool,3>> &UVWRepeats);

    /************
     * 7 GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE
     ************/
    static VkSampler depthImageSampler;
    void addDepthImageSamplerUniformBuffer();
    
    /************
     * 8 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE
     ************/
    static VkSampler lightDepthImageSampler;
    void addLightDepthImageSamplerUniformBuffer();

    /************
     * 9 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS
     ************/
    static VkSampler lightDepthImageSampler_hardwareDepthBias;
    void addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();


    /************
     * 10 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2
     * Test: WIP
     ************/
    //static VkSampler lightDepthImageSampler_hardwareDepthBias2;
    //static void addLightDepthImageSamplerUniformBuffer_hardwareDepthBias2();

    /************
     * Helper Functions
     ************/
    static int getPoolSize();
    static int getLayoutSize_General();
    static int getSetSize_General();
    void DestroyAndFree();
};

}//namespace