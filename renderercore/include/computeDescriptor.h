#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeUniform.h"
#include "TypeDataBuffer.h"

namespace LEApplication{
    class IApplication;
}

namespace LERenderer{

class CComputeDescriptorManager{
public:
    CComputeDescriptorManager(){}
    ~CComputeDescriptorManager(){}

    LEApplication::IApplication* game;

    /************
     * Pool
     ************/
    static int computeUniformTypes;
    static VkDescriptorPool computeDescriptorPool;
    static std::vector<VkDescriptorPoolSize> computeDescriptorPoolSizes;
    void createDescriptorPool();

    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> computeBindings;
    static VkDescriptorSetLayout descriptorSetLayout;
    void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    void createDescriptorSets(VkImageView textureImageView = NULL, std::vector<VkImageView> *swapchainImageViews = NULL);


    /************
     * 1 COMPUTE_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    void uploadCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize);


    /************
     * 2 COMPUTE_STORAGEBUFFER_DOUBLE
     ************/
    //define two set of storage buffers, one for input, the other for output
	static std::vector<CWxjBuffer> storageBuffers;
    static std::vector<void*> storageBuffersMapped;
    static VkDeviceSize m_storageBufferSize;
    void addStorageBuffer(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); //the same function to add storage 1&2
    void uploadStorageBuffer(uint32_t currentFrame, const void* data, size_t size);
    void downloadStorageBuffer(uint32_t currentFrame, void* data, size_t size);


    /************
     * 3 COMPUTE_STORAGEIMAGE_TEXTURE
     * 4 COMPUTE_STORAGEIMAGE_SWAPCHAIN
     ************/
    void addStorageImage(VkBufferUsageFlags usage);
   

     /************
     * Helper Functions
     ************/
    static int getPoolSize();
    static int getLayoutSize();
    static int getSetSize();
    void DestroyAndFree();
};

} //namespace