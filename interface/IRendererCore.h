#pragma once
//#include "TypeAppInfo.h"
#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeBuffer.h"

namespace LEApplication{
    class IApplication;
}

//class AppInfo;

namespace LERenderer{
    class IRendererCore {
    public:
        virtual ~IRendererCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        void greet() {std::cout<<"rendere greet!"<<std::endl;}

        virtual void SetRenderMode(int value) = 0;
        virtual int GetRenderMode() = 0;
        virtual void SetEnableObjectMVP(bool value) = 0;
        virtual int GetEnableObjectMVP() = 0;
        virtual void SetEnableTextboxMVP(bool value) = 0;
        virtual int GetEnableTextboxMVP() = 0;
        virtual uint32_t GetCurrentFrame() = 0;
        virtual void SetCurrentFrame(uint32_t value) = 0;
        virtual VkCommandPool& GetCommandPool() = 0;
        virtual VkCommandBuffer& GetGraphicsCommandBuffer() = 0;
        virtual VkCommandBuffer& GetComputeCommandBuffer() = 0;
        virtual void CreateCommandPool(VkSurfaceKHR &surface) = 0;
        virtual std::vector<VkCommandBuffer>& GetComputeCommandBuffers() = 0;

        virtual void CreateVertexBuffer (void* data, size_t elementSize, size_t elementCount) = 0;
        virtual void CreateIndexBuffer (std::vector<uint32_t> &indices3D) = 0;
        virtual void CreateGraphicsCommandBuffer() = 0;
        virtual void CreateComputeCommandBuffer() = 0;

        virtual void CreateSyncObjects(int swapchainSize) = 0;

        virtual void AquireSwapchainImage(VkSwapchainKHR swapchainHandle) = 0;
        virtual void WaitForComputeFence() = 0;
        virtual void SubmitCompute() = 0;
        virtual void WaitForGraphicsFence() = 0;
        virtual void SubmitGraphics() = 0;
        virtual void PresentSwapchainImage(VkSwapchainKHR swapchainHandle) = 0;

        //virtual void BeginCommandBuffer(int commandBufferIndex) = 0;
        virtual void BeginGraphicsCommandBuffer() = 0;
        virtual void BeginComputeCommandBuffer() = 0;
        virtual void BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues, bool useSingleFramebuffer) = 0;
        //virtual void BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex) = 0;
        virtual void BindGraphicsPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint) = 0;
        virtual void BindComputePipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint) = 0;
        virtual void SetViewport(VkExtent2D &extent) = 0;
        virtual void SetScissor(VkExtent2D &extent) = 0;
        virtual void BindVertexBuffer(int modelId) = 0;
        virtual void BindVertexInstanceBuffer(int modelId, VkBuffer *pBuffer) = 0;
        virtual void BindIndexBuffer(int modelId)  = 0;
        virtual void BindExternalBuffer(std::vector<CWxjBuffer> &buffer) = 0;
        virtual void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1) = 0;
        virtual void BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectMVPOffset = -1, uint32_t dynamicTextboxMVPOffset = -1) = 0;
        virtual void BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets) = 0;

        virtual void EndGraphicsCommandBuffer() = 0;
        virtual void EndComputeCommandBuffer() = 0;
        virtual void EndGraphicsRenderPass() = 0;

        virtual void StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout) = 0;
        virtual void EndRecordComputeCommandBuffer() = 0;
        virtual void RecordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind) = 0;

        virtual void StartRecordGraphicsCommandBuffer(VkRenderPass &renderPass, 
            std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
            std::vector<VkClearValue> &clearValues) = 0;
        virtual void EndRecordGraphicsCommandBuffer() = 0;

        virtual void PushConstantToCommand(void* pcData, VkPipelineLayout graphicsPipelineLayout, VkPushConstantRange &pushConstantRange) = 0;
        virtual void DrawIndexed(int model_id) = 0;
        virtual void Draw(uint32_t n) = 0;
        virtual void DrawInstanceIndexed(int model_id, int instanceCount) = 0;

        virtual void Update() = 0;

        virtual void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) = 0;

        virtual void Destroy() = 0;
        //AppInfo& GetAppInfo() { return appInfo; }
    protected:
        //AppInfo appInfo;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
