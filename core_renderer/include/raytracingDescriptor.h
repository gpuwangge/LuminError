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

    //LEGameEngine::IGameEngine* game;
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
    void createDescriptorSets(VkImageView textureImageView, VkAccelerationStructureKHR tlas);


    /************
     * 2 COMPUTE_STORAGEBUFFER_WINDOWSWAP
     * Read and Write
     ************/
    //define two sets(and layouts) of storage buffers, one for input, the other for output; however they share the same descriptor pool
	// static std::vector<CWxjBuffer> storageBuffers_windowswap;
    // static std::vector<void*> storageBuffersMapped_windowswap;
    // //static VkDeviceSize m_storageBufferSize_windowswap;
    // void addStorageBuffer_windowswap(); //the same function to add storage 1&2
    // void uploadStorageBuffer_windowswap(uint32_t currentFrame, const void* data, size_t size);
    // void downloadStorageBuffer_windowswap(uint32_t currentFrame, void* data, size_t size);

    /************
     * 4 COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXATTRIBUTE
     ************/
    static std::vector<CWxjBuffer> storageBuffers_triangleVertexAttribute;
    static std::vector<void*> storageBuffersMapped_triangleVertexAttribute;
    void addStorageBuffer_triangleVertexAttribute();
    void uploadStorageBuffer_triangleVertexAttribute(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 5 COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX
     ************/
    static std::vector<CWxjBuffer> storageBuffers_triangleVertexIndex;
    static std::vector<void*> storageBuffersMapped_triangleVertexIndex;
    void addStorageBuffer_triangleVertexIndex();
    void uploadStorageBuffer_triangleVertexIndex(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 5.2 geometry info
     ************/
    static std::vector<CWxjBuffer> storageBuffers_geometryInfo;
    static std::vector<void*> storageBuffersMapped_geometryInfo;
    void addStorageBuffer_geometryInfo();
    void uploadStorageBuffer_geometryInfo(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 3 COMPUTE_STORAGEBUFFER_MATERIAL
     * Read only
     ************/
    static std::vector<CWxjBuffer> storageBuffers_material;
    static std::vector<void*> storageBuffersMapped_material;
    void addStorageBuffer_material();
    void uploadStorageBuffer_material(uint32_t currentFrame, const void* data, size_t size);

    /************
     * 1 COMPUTE_UNIFORMBUFFER_GLOBAL
     ************/
    static std::vector<CWxjBuffer> m_globalUniformBuffers; 
	static std::vector<void*> m_globalUniformBuffersMapped;
    //static VkDeviceSize m_globalUniformBufferSize;
    void addGlobalUniformBuffer_raytracing();
    void uploadGlobalUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize);

    /************
     * 8 COMPUTE_UNIFORMBUFFER_CUSTOM
     ************/
    static std::vector<CWxjBuffer> customUniformBuffers; 
	static std::vector<void*> customUniformBuffersMapped;
    static VkDeviceSize m_customUniformBufferSize;
    void addCustomUniformBuffer_raytracing(VkDeviceSize customUniformBufferSize);
    void uploadCustomUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize);


    /************
     * ? COMPUTE_UNIFORMBUFFER_RTLIGHT
     ************/
    static std::vector<CWxjBuffer> m_storageBuffers_rtLight; 
	static std::vector<void*> m_storageBuffersMapped_rtLight;
    void addStorageBuffer_rtLight();
    void uploadStorageBuffer_rtLight(uint32_t currentFrame, const void* data, size_t dataSize);




    /************
     * 10 COMPUTE_STORAGEIMAGE_TEXTURE
     * 11 COMPUTE_STORAGEIMAGE_SWAPCHAIN
     ************/
    void addStorageImage(VkBufferUsageFlags usage);
   

     /************
     * Helper Functions
     ************/
    bool bVerbose = false;
    static int getPoolSize();
    static int getLayoutSize();
    static int getSetSize();
    void DestroyAndFree();
};

} //namespace