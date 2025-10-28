#pragma once
#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeDataBuffer.h"

namespace LEApplication{
    class IApplication;
}

class AppInfo;

namespace LERenderer{
    class IRendererCore {
    public:
        virtual ~IRendererCore() = default;
        LEApplication::IApplication* game;
        virtual void SetApplication(LEApplication::IApplication* pApplication) = 0;

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

        //RenderProcess related(for attachement and subpass)
        virtual void SetShadowmapAttachmentDepthLight(int value) = 0;
        virtual void SetMainSceneAttachmentDepthLight(int value) = 0;
        virtual void SetMainSceneAttachmentDepthCamera(int value) = 0;
        virtual void SetMainSceneAttachmentColorResovle(int value) = 0;
        virtual void SetMainSceneAttachmentColorPresent(int value) = 0;
        virtual int GetShadowmapAttachmentDepthLight() = 0;
        virtual int GetMainSceneAttachmentDepthLight() = 0;
        virtual int GetMainSceneAttachmentDepthCamera() = 0;
        virtual int GetMainSceneAttachmentColorResovle() = 0;
        virtual int GetMainSceneAttachmentColorPresent() = 0;

        virtual void Create_attachmentdescription_shadowmap_depthlight(VkFormat depthFormat) = 0;
        virtual void Create_attachmentdescription_mainscene_depthlight(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples) = 0;
        virtual void Create_attachmentdescription_mainscene_depthcamera(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples) = 0;
        virtual void Create_attachmentdescription_mainscene_colorresolve(VkFormat swapChainImageFormat,VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) = 0;
        virtual void Create_attachmentdescription_mainscene_colorpresent(VkFormat swapChainImageFormat) = 0;

        virtual void SetEnableShadowmapRenderpassSubpassShadowmap(bool value) = 0;
        virtual void SetEnableMainSceneRenderpassSubpassShadowmap(bool value) = 0;
        virtual void SetEnableMainSceneRenderpassSubpassDraw(bool value) = 0;
        virtual void SetEnableMainSceneRenderpassSubpassObserve(bool value) = 0;
        virtual bool GetEnableShadowmapRenderpassSubpassShadowmap() = 0;
        virtual bool GetEnableMainSceneRenderpassSubpassShadowmap() = 0;
        virtual bool GetEnableMainSceneRenderpassSubpassDraw() = 0;
        virtual bool GetEnableMainSceneRenderpassSubpassObserve() = 0;

        virtual void CreateSubpass_shadowmap() = 0;
        virtual void CreateSubpass_mainscene(int attachment_id_to_observe) = 0;
        virtual void CreateSubpass_mainscene_lightdepth() = 0;
        virtual void CreateSubpass_mainscene_draw() = 0;
        virtual void CreateSubpass_mainscene_observe(int attachment_id_to_observe) = 0;

        virtual void CreateDependency_shadowmap() = 0;
        virtual void CreateDependency_mainscene() = 0;
        
        virtual void CreateRenderPass_shadowmap() = 0;
        virtual void CreateRenderPass_mainscene() = 0;

        virtual VkRenderPass& GetRenderpass_shadowmap() = 0;
        virtual VkRenderPass& GetRenderpass_mainscene() = 0;

        virtual void CreateComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout) = 0;
        virtual void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id) = 0;
        virtual void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id) = 0;
        virtual void CreateComputePipeline(VkShaderModule &computeShaderModule) = 0;
        using GetBindingDescFunc = VkVertexInputBindingDescription(*)();
        using GetAttributeDescFunc = std::vector<VkVertexInputAttributeDescription>(*)();
        virtual void CreateGraphicsPipeline(GetBindingDescFunc getBindingDesc, GetAttributeDescFunc getAttributeDesc,
            VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer, bool bUseInstanceBuffer,
            VkRenderPass renderPass, int graphcisPipeline_id, AppInfo *appInfo) = 0;

        virtual void AddColorBlendAttachment(VkBlendOp colorBlendOp, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
								 VkBlendOp alphaBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor) = 0;

        virtual VkPipelineLayout& GetComputePipelineLayout() = 0;
        virtual VkPipeline& GetComputePipeline() = 0;

        virtual VkPipelineLayout& GetGraphicsPipelineLayout(int pipelineId) = 0;
        virtual VkPipeline& GetGraphicsPipeline(int pipelineId) = 0;
        virtual std::vector<VkPipelineLayout> GetGraphicsPipelineLayouts() = 0;
        virtual std::vector<VkPipeline> GetGraphicsPipelines() = 0;

        virtual std::vector<VkClearValue>& GetClearValues() = 0;
        virtual std::vector<VkClearValue>& GetClearValues_shadowmap() = 0;


        virtual void RenderProcessCleanup() = 0;



    protected:
        //AppInfo appInfo;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
