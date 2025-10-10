#ifndef H_COMPUTEDESCRIPTOR
#define H_COMPUTEDESCRIPTOR

#include "context.h"
#include "dataBuffer.hpp"
#include "../include/texture.h"

class CComputeDescriptorManager{
public:
    CComputeDescriptorManager(){}
    ~CComputeDescriptorManager(){}

    /************
     * Pool
     ************/
    static int computeUniformTypes;
    static VkDescriptorPool computeDescriptorPool;
    static std::vector<VkDescriptorPoolSize> computeDescriptorPoolSizes;
    static void createDescriptorPool();

    /************
     * Layout
     ************/
    static std::vector<VkDescriptorSetLayoutBinding> computeBindings;
    static VkDescriptorSetLayout descriptorSetLayout;
    static void createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr);
    

    /************
     * Set
     ************/
    std::vector<VkDescriptorSet> descriptorSets; //one descriptor set for each host resource (MAX_FRAMES_IN_FLIGHT)
    void createDescriptorSets(std::vector<CTextureImage> *textureImages = NULL, std::vector<VkImageView> *swapchainImageViews = NULL);


    /************
     * 1 COMPUTE_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    static void addCustomUniformBuffer(VkDeviceSize customUniformBufferSize);
    template <typename T>
    void updateCustomUniformBuffer(uint32_t currentFrame, T customUniformBufferObject){
        if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM)
            memcpy(customUniformBuffersMapped[currentFrame], &customUniformBufferObject, sizeof(customUniformBufferObject));
    }


    /************
     * 2 COMPUTE_STORAGEBUFFER_DOUBLE
     ************/
    //define two set of storage buffers, one for input, the other for output
	static std::vector<CWxjBuffer> storageBuffers; 
    static std::vector<void*> storageBuffersMapped;
    static VkDeviceSize m_storageBufferSize;
    static void addStorageBuffer(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT); //the same function to add storage 1&2
    template <typename T>
    void updateStorageBuffer(uint32_t currentFrame, T storageBufferObject){ 
        memcpy(storageBuffersMapped[currentFrame], &storageBufferObject, sizeof(storageBufferObject));
    }

    /************
     * 3 COMPUTE_STORAGEIMAGE_TEXTURE
     * 4 COMPUTE_STORAGEIMAGE_SWAPCHAIN
     ************/
    static void addStorageImage(VkBufferUsageFlags usage);
   

     /************
     * Helper Functions
     ************/
    static int getPoolSize();
    static int getLayoutSize();
    static int getSetSize();
    void DestroyAndFree();
};



#endif