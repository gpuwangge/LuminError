#ifndef H_GRAPHICSDESCRIPTOR
#define H_GRAPHICSDESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "../include/texture.h"

class CGraphicsDescriptorManager{
public:
    CGraphicsDescriptorManager(){};
    ~CGraphicsDescriptorManager(){};

    /************
     * Pool
     ************/
    static int graphicsUniformTypes;
    static VkDescriptorPool graphicsDescriptorPool;
    static std::vector<VkDescriptorPoolSize> graphicsDescriptorPoolSizes;
    static void createDescriptorPool(unsigned int object_textbox_count = 0);
    
    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> graphicsBindings;
    static VkDescriptorSetLayout descriptorSetLayout_general;
    static VkDescriptorSetLayout descriptorSetLayout_textureImageSampler;
    static void createDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    static void createDescriptorSetLayout_TextureImageSampler();

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets_general; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    //void createDescriptorSets_General(VkImageView depthImageView, VkImageView lightDepthImageView0, VkImageView lightDepthImageView1);
    void createDescriptorSets_General(VkImageView depthImageView, std::vector<CWxjImageBuffer> &lightDepthBuffers);

    /************
     * 1 GRAPHCIS_UNIFORMBUFFER_MVP
     ************/
    static std::vector<CWxjBuffer> mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
	static std::vector<void*> mvpUniformBuffersMapped;
    static void addMVPUniformBuffer();
    static MVPUniformBufferObject mvpUBO;

    /************
     * 2 GRAPHCIS_UNIFORMBUFFER_TEXT_MVP
     ************/
    static std::vector<CWxjBuffer> textMVPUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
	static std::vector<void*> textMVPUniformBuffersMapped;
    static void addTextMVPUniformBuffer();
    static TextMVPUniformBufferObject textMVPUBO;

    /************
     * 3 GRAPHCIS_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    static void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, T customUniformBufferObject){
        //std::cout<<"sizeof(customUniformBufferObject)="<<sizeof(customUniformBufferObject)<<std::endl;
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }

    /************
     * 4 GRAPHCIS_UNIFORMBUFFER_LIGHTING
     ************/
    static LightingUniformBufferObject m_lightingUBO;
    static std::vector<CWxjBuffer> m_lightingUniformBuffers; 
	static std::vector<void*> m_lightingUniformBuffersMapped;
    static VkDeviceSize m_lightingUniformBufferSize;
    static void addLightingUniformBuffer();

    /************
     * 5 GRAPHCIS_UNIFORMBUFFER_VP
     ************/
    static std::vector<CWxjBuffer> vpUniformBuffers; 
	static std::vector<void*> vpUniformBuffersMapped;
    static void addVPUniformBuffer();
    static VPUniformBufferObject vpUBO;
    //static bool CheckMVP(); //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset

    /************
     * 6 GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE
     ************/
    static unsigned int textureImageSamplerSize;
    static std::vector<VkSampler> textureImageSamplers;
    static void addTextureImageSamplerUniformBuffer(std::vector<int> mipLevels, std::vector<std::vector<bool>> UVWRepeats);

    /************
     * 7 GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE
     ************/
    static VkSampler depthImageSampler;
    static void addDepthImageSamplerUniformBuffer();
    
    /************
     * 8 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE
     ************/
    static VkSampler lightDepthImageSampler;
    static void addLightDepthImageSamplerUniformBuffer();

    /************
     * 9 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS
     ************/
    static VkSampler lightDepthImageSampler_hardwareDepthBias;
    static void addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();


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

#endif