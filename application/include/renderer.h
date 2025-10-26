#ifndef H_RENDERER
#define H_RENDERER

#include <vulkan/vulkan.h>
#include <vector>
#include "TypeBuffer.h"
#include "Enum.h"

class CRenderer final{
public:
    CRenderer(){};
    ~CRenderer(){};

    /**************************
     * Universial Render Functions
     * ***********************/
    RenderModes m_renderMode = GRAPHICS;

    bool bUseObjectMVP = false;
    bool bUseTextboxMVP = false;

    void AquireSwapchainImage(VkSwapchainKHR swapchainHandle);
    void WaitForComputeFence();
    void SubmitCompute();
    void WaitForGraphicsFence();
    void SubmitGraphics();
    void PresentSwapchainImage(VkSwapchainKHR swapchainHandle); 


    /**************************
     * Graphics Functions
     * ***********************/
    //Create start() and end() to make sample command recording simple
    void StartRecordGraphicsCommandBuffer(VkRenderPass &renderPass, 
        std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
        std::vector<VkClearValue> &clearValues);
    void EndRecordGraphicsCommandBuffer();

    //Start(...)
    void BeginCommandBuffer(int commandBufferIndex);
    void BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues, bool useSingleFramebuffer);
    void BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex);
    void SetViewport(VkExtent2D &extent);
    void SetScissor(VkExtent2D &extent);
    void BindVertexBuffer(int modelId);
    void BindVertexInstanceBuffer(int modelId, VkBuffer *pBuffer);
    void BindIndexBuffer(int modelId);
    void BindExternalBuffer(std::vector<CWxjBuffer> &buffer);
    void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1);
    void BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1);
    void BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets);

    //Draw
    void PushConstantToCommand(void* pcData, VkPipelineLayout graphicsPipelineLayout, VkPushConstantRange &pushConstantRange) {
        vkCmdPushConstants(commandBuffers[graphicsCmdId][currentFrame], graphicsPipelineLayout, 
                      pushConstantRange.stageFlags, pushConstantRange.offset, 
                      pushConstantRange.size, pcData);
    }
    void DrawIndexed(int model_id);//std::vector<uint32_t> &indices3D
    void Draw(uint32_t n);
    void DrawInstanceIndexed(int model_id, int instanceCount);

    //End()
    void EndRenderPass();
    void EndCommandBuffer(int commandBufferIndex);


    /**************************
     * Compute Shader Functions
     * ***********************/
    void StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout);
    void EndRecordComputeCommandBuffer();

    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);


    /**************************
     * Utility Functions
     * ***********************/
    void RecordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
            VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind);

    /**************************
     * MISC Functions
     * ***********************/
    void CreateVertexBuffer(void* data, size_t elementSize, size_t elementCount);
    void CreateIndexBuffer(std::vector<uint32_t> &indices3D);
    //void CreateInstanceBuffer(std::vector<TextInstanceData> &instanceData);

    int graphicsCmdId = 0;
    int computeCmdId = 0;
    void CreateCommandPool(VkSurfaceKHR &surface);
    void CreateGraphicsCommandBuffer();
    void CreateComputeCommandBuffer();
    void CreateCommandBuffers();

    void CreateSyncObjects(int swapchainSize);

    void Destroy();

    uint32_t currentFrame = 0;
    uint32_t imageIndex = 0;
    void Update(); //update currentFrame

    std::vector<CWxjBuffer> vertexDataBuffers;  //each buffer object is for one model object, the index in this vector is object.id
	std::vector<CWxjBuffer> indexDataBuffers;
    //std::vector<CWxjBuffer> instanceDataBuffers;
    std::vector<std::vector<uint32_t>> indices3Ds;
    std::vector<std::vector<VkCommandBuffer>> commandBuffers;  //commandBuffers[Size][MAX_FRAMES_IN_FLIGHT or currentFrame]
    VkCommandPool commandPool;


    std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::vector<VkSemaphore> computeFinishedSemaphores;
    std::vector<VkFence> computeInFlightFences;


};

#endif
