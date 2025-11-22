#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeUniform.h"
#include "TypeDataBuffer.h"

namespace LEGameEngine{
    class IGameEngine;
}

namespace LERenderer{

class CComputeDescriptorManager{
public:
    CComputeDescriptorManager(){}
    ~CComputeDescriptorManager(){}

    LEGameEngine::IGameEngine* game;

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
     * 1 COMPUTE_UNIFORMBUFFER_GLOBAL
     ************/
    static std::vector<CWxjBuffer> m_globalUniformBuffers; 
	static std::vector<void*> m_globalUniformBuffersMapped;
    //static VkDeviceSize m_globalUniformBufferSize;
    void addGlobalUniformBuffer();
    void uploadGlobalUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 2 COMPUTE_STORAGEBUFFER_WINDOWSWAP
     * Read and Write
     ************/
    //define two sets(and layouts) of storage buffers, one for input, the other for output; however they share the same descriptor pool
	static std::vector<CWxjBuffer> storageBuffers_windowswap;
    static std::vector<void*> storageBuffersMapped_windowswap;
    //static VkDeviceSize m_storageBufferSize_windowswap;
    void addStorageBuffer_windowswap(); //the same function to add storage 1&2
    void uploadStorageBuffer_windowswap(uint32_t currentFrame, const void* data, size_t size);
    void downloadStorageBuffer_windowswap(uint32_t currentFrame, void* data, size_t size);
    

    /************
     * 3 COMPUTE_STORAGEBUFFER_MATERIAL
     * Read only
     ************/
    static std::vector<CWxjBuffer> storageBuffers_material;
    static std::vector<void*> storageBuffersMapped_material;
    void addStorageBuffer_material(); //the same function to add storage 1&2
    void uploadStorageBuffer_material(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 4 COMPUTE_STORAGEBUFFER_TRIANGLE
     ************/

    /************
     * 5 COMPUTE_STORAGEBUFFER_SPHERE
     ************/

    /************
     * 6 COMPUTE_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    void uploadCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 7 COMPUTE_STORAGEBUFFER_CUSTOMSWAP
     * Read and Write
     * Need get size from game example
     ************/
    //define two sets(and layouts) of storage buffers, one for input, the other for output; however they share the same descriptor pool
	static std::vector<CWxjBuffer> storageBuffers_customswap;
    static std::vector<void*> storageBuffersMapped_customswap;
    static VkDeviceSize m_storageBufferSize_customswap;
    void addStorageBuffer_customswap(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); //the same function to add storage 1&2
    void uploadStorageBuffer_customswap(uint32_t currentFrame, const void* data, size_t size);
    void downloadStorageBuffer_customswap(uint32_t currentFrame, void* data, size_t size);

    


    /************
     * 8 COMPUTE_STORAGEIMAGE_TEXTURE
     * 9 COMPUTE_STORAGEIMAGE_SWAPCHAIN
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