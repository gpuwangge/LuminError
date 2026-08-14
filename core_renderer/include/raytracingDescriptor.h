#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeUniform.h"
#include "TypeDataBuffer.h"
#include "swapchain.h"

namespace LEGameEngine{
    class IGameEngine;
}

namespace LERenderer{

class CRaytracingDescriptorManager{
public:
    CRaytracingDescriptorManager(){}
    ~CRaytracingDescriptorManager(){}

    CSwapchain* p_swapchain;

    /************
     * Pool
     ************/
    static int raytracingUniformTypes;
    static VkDescriptorPool raytracingDescriptorPool;
    static std::vector<VkDescriptorPoolSize> raytracingDescriptorPoolSizes;
    void createDescriptorPool();

    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> raytracingBindings;
    static VkDescriptorSetLayout descriptorSetLayout;
    void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    void createDescriptorSets(VkImageView textureImageView, VkAccelerationStructureKHR tlas, const std::vector<VkImageView>& glbTextureImageViews);

    /************
     * 0 image
     ************/

     /************
     * 1 acceleration structure
     ************/

    /************
     * 2 geometry info
     ************/
    static std::vector<CWxjBuffer> storageBuffers_geometryInfo;
    static std::vector<void*> storageBuffersMapped_geometryInfo;
    void addStorageBuffer_geometryInfo();
    void uploadStorageBuffer_geometryInfo(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 3 UNIFORMBUFFER_MATERIAL
     * Read only
     ************/
    static std::vector<CWxjBuffer> uniformBuffers_material;
    static std::vector<void*> uniformBuffersMapped_material;
    void addUniformBuffer_material();
    void uploadUniformBuffer_material(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 4 UNIFORMBUFFER_GLOBAL
     ************/
    static std::vector<CWxjBuffer> m_globalUniformBuffers; 
	static std::vector<void*> m_globalUniformBuffersMapped;
    //static VkDeviceSize m_globalUniformBufferSize;    
    void addGlobalUniformBuffer_raytracing();
    void uploadGlobalUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 5 UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer_raytracing(VkDeviceSize customUniformBufferSize);
    void uploadCustomUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 6 UNIFORMBUFFER_RTLIGHT
     ************/
    static std::vector<CWxjBuffer> m_uniformBuffers_rtLight; 
	static std::vector<void*> m_uniformBuffersMapped_rtLight;
    void addUniformBuffer_rtLight();
    void uploadUniformBuffer_rtLight(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 7 UNIFORMBUFFER_INSTANCEINFO
     ************/
    static std::vector<CWxjBuffer> m_uniformBuffers_instance; 
	static std::vector<void*> m_uniformBuffersMapped_instance;
    void addUniformBuffer_instance();
    void uploadUniformBuffer_instance(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 8 UNIFORMBUFFER_CONFIGINFO
     ************/
    static std::vector<CWxjBuffer> m_uniformBuffers_config; 
	static std::vector<void*> m_uniformBuffersMapped_config;
    void addUniformBuffer_config();
    void uploadUniformBuffer_config(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 9 accumulated IMAGE
     ************/

    /************
     * 10 Sampler for GLTF Textures
     ************/
    std::vector<VkSampler> glbSamplers;

    /************
     *  COMPUTE_STORAGEIMAGE_TEXTURE
     *  COMPUTE_STORAGEIMAGE_SWAPCHAIN
     ************/
    void addStorageImage(VkBufferUsageFlags usage);
   

     /************
     * Helper Functions
     ************/
    bool bVerbose = false;
    static int getPoolSize(int glbSamplerSize);
    static int getLayoutSize(int glbSamplerSize);
    static int getSetSize(int glbSamplerSize);
    void DestroyAndFree();
};

} //namespace