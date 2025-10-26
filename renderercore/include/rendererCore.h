#pragma once
#include "IRendererCore.h"

#include <vulkan/vulkan.h>
#include <vector>
#include "TypeBuffer.h"
#include "Enum.h"

namespace LERenderer{
    class RendererCore final : public IRendererCore{
    public:
        RendererCore(){}
        ~RendererCore(){}
        //void ReadYAMLFile() override;

        //Expose to application
        void SetRenderMode(int value) override { m_renderMode = (RenderModes)value; }
        int GetRenderMode() override { return m_renderMode; }
        void SetEnableObjectMVP(bool value) override { bUseObjectMVP = value; }
        int GetEnableObjectMVP() override { return bUseObjectMVP; }
        void SetEnableTextboxMVP(bool value) override { bUseTextboxMVP = value; }
        int GetEnableTextboxMVP() override { return bUseTextboxMVP; }
        uint32_t GetCurrentFrame() override { return currentFrame; }
        void SetCurrentFrame(uint32_t value) override { currentFrame = value; }
        VkCommandPool& GetCommandPool() override { return commandPool; }
        VkCommandBuffer& GetGraphicsCommandBuffer() override { return commandBuffers[graphicsCmdId][currentFrame]; }
        VkCommandBuffer& GetComputeCommandBuffer() override { return commandBuffers[computeCmdId][currentFrame]; }
        std::vector<VkCommandBuffer>& GetComputeCommandBuffers() override { return commandBuffers[computeCmdId]; }

        /**************************
         * Universial Render Functions
         * ***********************/
        RenderModes m_renderMode = GRAPHICS;

        bool bUseObjectMVP = false;
        bool bUseTextboxMVP = false;

        void AquireSwapchainImage(VkSwapchainKHR swapchainHandle) override;
        void WaitForComputeFence() override;
        void SubmitCompute() override;
        void WaitForGraphicsFence() override;
        void SubmitGraphics() override;
        void PresentSwapchainImage(VkSwapchainKHR swapchainHandle) override; 


        /**************************
         * Graphics Functions
         * ***********************/
        //Create start() and end() to make sample command recording simple
        void StartRecordGraphicsCommandBuffer(VkRenderPass &renderPass, 
            std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
            std::vector<VkClearValue> &clearValues) override;
        void EndRecordGraphicsCommandBuffer() override;

        //Start(...)
        void BeginCommandBuffer(int commandBufferIndex);
        void BeginGraphicsCommandBuffer() override;
        void BeginComputeCommandBuffer() override;
        void BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues, bool useSingleFramebuffer) override;
        void BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex);
        void BindGraphicsPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint) override;
        void BindComputePipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint) override;
        void SetViewport(VkExtent2D &extent) override;
        void SetScissor(VkExtent2D &extent) override;
        void BindVertexBuffer(int modelId) override;
        void BindVertexInstanceBuffer(int modelId, VkBuffer *pBuffer) override;
        void BindIndexBuffer(int modelId) override;
        void BindExternalBuffer(std::vector<CWxjBuffer> &buffer) override;
        void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1) override;
        void BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1) override;
        void BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets) override;

        //Draw
        void PushConstantToCommand(void* pcData, VkPipelineLayout graphicsPipelineLayout, VkPushConstantRange &pushConstantRange) override;
        void DrawIndexed(int model_id) override;//std::vector<uint32_t> &indices3D
        void Draw(uint32_t n) override;
        void DrawInstanceIndexed(int model_id, int instanceCount) override;

        //End()
        void EndGraphicsRenderPass() override;
        void EndCommandBuffer(int commandBufferIndex);
        void EndGraphicsCommandBuffer() override;
        void EndComputeCommandBuffer() override;

        /**************************
         * Compute Shader Functions
         * ***********************/
        void StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout) override;
        void EndRecordComputeCommandBuffer() override;

        void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;


        /**************************
         * Utility Functions
         * ***********************/
        void RecordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind) override;

        /**************************
         * MISC Functions
         * ***********************/
        void CreateVertexBuffer (void* data, size_t elementSize, size_t elementCount) override;
        void CreateIndexBuffer (std::vector<uint32_t> &indices3D) override;
        //void CreateInstanceBuffer(std::vector<TextInstanceData> &instanceData);

        int graphicsCmdId = 0;
        int computeCmdId = 0;
        void CreateCommandPool(VkSurfaceKHR &surface) override;
        void CreateGraphicsCommandBuffer() override;
        void CreateComputeCommandBuffer() override;
        void CreateCommandBuffers();

        void CreateSyncObjects(int swapchainSize) override;

        void Destroy() override;

        uint32_t currentFrame = 0;
        uint32_t imageIndex = 0;
        void Update() override; //update currentFrame

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

    private:
        //YAML::Node yamlNode;
    };
    EXPORT_FACTORY_FOR(RendererCore);
}

