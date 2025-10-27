#pragma once
#include "IRendererCore.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeBuffer.h"
#include "Enum.h"
#include "renderProcess.h"

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

        /**************************
         * RenderProcess
         * ***********************/
        CRenderProcess renderProcess;
        void SetApplication(LEApplication::IApplication* pApplication) override {
            game = pApplication;
            renderProcess.game = game; 
        }

        void SetShadowmapAttachmentDepthLight(int value) override { renderProcess.iShadowmapAttachmentDepthLight = value; }
        void SetMainSceneAttachmentDepthLight(int value) override { renderProcess.iMainSceneAttachmentDepthLight = value; }
        void SetMainSceneAttachmentDepthCamera(int value) override { renderProcess.iMainSceneAttachmentDepthCamera = value; }
        void SetMainSceneAttachmentColorResovle(int value) override { renderProcess.iMainSceneAttachmentColorResovle = value; }
        void SetMainSceneAttachmentColorPresent(int value) override { renderProcess.iMainSceneAttachmentColorPresent = value; }
        int GetShadowmapAttachmentDepthLight() override { return renderProcess.iShadowmapAttachmentDepthLight; }
        int GetMainSceneAttachmentDepthLight() override { return renderProcess.iMainSceneAttachmentDepthLight; }
        int GetMainSceneAttachmentDepthCamera() override { return renderProcess.iMainSceneAttachmentDepthCamera; }
        int GetMainSceneAttachmentColorResovle() override { return renderProcess.iMainSceneAttachmentColorResovle; }
        int GetMainSceneAttachmentColorPresent() override { return renderProcess.iMainSceneAttachmentColorPresent; }

        void Create_attachmentdescription_shadowmap_depthlight(VkFormat depthFormat) override { renderProcess.create_attachmentdescription_shadowmap_depthlight(depthFormat); }
        void Create_attachmentdescription_mainscene_depthlight(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples) override { renderProcess.create_attachmentdescription_mainscene_depthlight(depthFormat, msaaSamples); }
        void Create_attachmentdescription_mainscene_depthcamera(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples) override { renderProcess.create_attachmentdescription_mainscene_depthcamera(depthFormat, msaaSamples); }
        void Create_attachmentdescription_mainscene_colorresolve(VkFormat swapChainImageFormat,VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) override { 
            renderProcess.create_attachmentdescription_mainscene_colorresolve(swapChainImageFormat, msaaSamples, imageLayout); }
        void Create_attachmentdescription_mainscene_colorpresent(VkFormat swapChainImageFormat) override { renderProcess.create_attachmentdescription_mainscene_colorpresent(swapChainImageFormat); }

        void SetEnableShadowmapRenderpassSubpassShadowmap(bool value) override { renderProcess.bEnableShadowmapRenderpassSubpassShadowmap = value; }
        void SetEnableMainSceneRenderpassSubpassShadowmap(bool value) override { renderProcess.bEnableMainSceneRenderpassSubpassShadowmap = value; }
        void SetEnableMainSceneRenderpassSubpassDraw(bool value) override { renderProcess.bEnableMainSceneRenderpassSubpassDraw = value; }
        void SetEnableMainSceneRenderpassSubpassObserve(bool value) override { renderProcess.bEnableMainSceneRenderpassSubpassObserve = value; }
        bool GetEnableShadowmapRenderpassSubpassShadowmap() override { return renderProcess.bEnableShadowmapRenderpassSubpassShadowmap; }
        bool GetEnableMainSceneRenderpassSubpassShadowmap() override { return renderProcess.bEnableMainSceneRenderpassSubpassShadowmap; }
        bool GetEnableMainSceneRenderpassSubpassDraw() override { return renderProcess.bEnableMainSceneRenderpassSubpassDraw; }
        bool GetEnableMainSceneRenderpassSubpassObserve() override { return renderProcess.bEnableMainSceneRenderpassSubpassObserve; }

        void CreateSubpass_shadowmap() override { renderProcess.createSubpass_shadowmap(); }
        void CreateSubpass_mainscene(int attachment_id_to_observe) override { renderProcess.createSubpass_mainscene(attachment_id_to_observe); }
        void CreateSubpass_mainscene_lightdepth() override { renderProcess.createSubpass_mainscene_lightdepth(); }
        void CreateSubpass_mainscene_draw() override { renderProcess.createSubpass_mainscene_draw(); }
        void CreateSubpass_mainscene_observe(int attachment_id_to_observe) override { renderProcess.createSubpass_mainscene_observe(attachment_id_to_observe); }

        void CreateDependency_shadowmap() override { renderProcess.createDependency_shadowmap(); }
        void CreateDependency_mainscene() override { renderProcess.createDependency_mainscene(); }
        
        void CreateRenderPass_shadowmap() override { renderProcess.createRenderPass_shadowmap(); }
        void CreateRenderPass_mainscene() override { renderProcess.createRenderPass_mainscene(); }

        VkRenderPass& GetRenderpass_shadowmap() override { return renderProcess.renderPass_shadowmap; }
        VkRenderPass& GetRenderpass_mainscene() override { return renderProcess.renderPass_mainscene; }

        void CreateComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout) override { renderProcess.createComputePipelineLayout(descriptorSetLayout); }
        void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id) override { renderProcess.createGraphicsPipelineLayout(descriptorSetLayouts, graphicsPipelineLayout_id); }
        void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id) override {
            renderProcess.createGraphicsPipelineLayout(descriptorSetLayouts, pushConstantRange, bUsePushConstant, graphicsPipelineLayout_id);
        }
        void CreateComputePipeline(VkShaderModule &computeShaderModule) override { renderProcess.createComputePipeline(computeShaderModule); }
        using GetBindingDescFunc = VkVertexInputBindingDescription(*)();
        using GetAttributeDescFunc = std::vector<VkVertexInputAttributeDescription>(*)();
        void CreateGraphicsPipeline(GetBindingDescFunc getBindingDesc, GetAttributeDescFunc getAttributeDesc,
            VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer, bool bUseInstanceBuffer,
            VkRenderPass renderPass, int graphcisPipeline_id, AppInfo *appInfo) override{
                renderProcess.createGraphicsPipeline(getBindingDesc, getAttributeDesc, topology, vertShaderModule, fragShaderModule, bUseVertexBuffer, bUseInstanceBuffer, renderPass, graphcisPipeline_id, appInfo);
            }
        
        void AddColorBlendAttachment(VkBlendOp colorBlendOp, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
								 VkBlendOp alphaBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor) override {
            renderProcess.addColorBlendAttachment(colorBlendOp, srcColorBlendFactor, dstColorBlendFactor,
                                                alphaBlendOp, srcAlphaBlendFactor, dstAlphaBlendFactor);
        }

        VkPipelineLayout& GetComputePipelineLayout() override { return renderProcess.computePipelineLayout; }
        VkPipeline& GetComputePipeline() override { return renderProcess.computePipeline; }
        
        VkPipelineLayout& GetGraphicsPipelineLayout(int pipelineId) override { return renderProcess.graphicsPipelineLayouts[pipelineId]; }
        VkPipeline& GetGraphicsPipeline(int pipelineId) override { return renderProcess.graphicsPipelines[pipelineId]; }
        std::vector<VkPipelineLayout> GetGraphicsPipelineLayouts() override { return renderProcess.graphicsPipelineLayouts; }
        std::vector<VkPipeline> GetGraphicsPipelines() override { return renderProcess.graphicsPipelines; }

        std::vector<VkClearValue>& GetClearValues() override { return renderProcess.clearValues; }
        std::vector<VkClearValue>& GetClearValues_shadowmap() override { return renderProcess.clearValues_shadowmap; }


        void RenderProcessCleanup() { renderProcess.Cleanup(); }
    private:
        //YAML::Node yamlNode;
    };
    EXPORT_FACTORY_FOR(RendererCore);
}

