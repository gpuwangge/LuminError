#pragma once
#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "TypeDataBuffer.h"
#include "ILogCore.h"
#include "Utility.h"

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

        /**************************
         * RenderProcess
         * ***********************/
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

        /**************************
         * Graphics Descriptor
         * ***********************/
        virtual int GetTextureImageSamplersSize() = 0;
        virtual int GetGraphicsUniformTypes() = 0;
        virtual void SetGraphicsUniformTypes(int value) = 0;
        virtual VkDescriptorSetLayout GetDescriptorSetLayout_General() = 0;
        virtual VkDescriptorSetLayout& GetDescriptorSetLayout_TextureImageSampler() = 0;
        virtual int GetSetSize_General() = 0;

        virtual std::vector<VkDescriptorSet>& GetDescriptorSets_General() = 0;
        virtual VkDescriptorPool& GetGraphicsDescriptorPool() = 0;
        virtual std::vector<VkSampler>& GetTextureImageSamplers() = 0;

        virtual void createGraphicsDescriptorPool(unsigned int object_textbox_count = 0) = 0;
        virtual void createGraphicsDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding = nullptr) = 0;
        virtual void createGraphicsDescriptorSetLayout_TextureImageSampler() = 0;
        virtual void createGraphicsDescriptorSets_General(VkImageView depthImageView, std::vector<VkImageView> &depthlight_imageviews) = 0;

        virtual void addMVPUniformBuffer(std::vector<void*>& mvpUniformBuffersMapped) = 0;
        virtual void addTextMVPUniformBuffer(std::vector<void*>& textMVPUniformBuffersMapped) = 0;
        virtual void addGraphicsCustomUniformBuffer(VkDeviceSize customUniformBufferSize) = 0;
        virtual void uploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) = 0;
        virtual void addLightingUniformBuffer(std::vector<void*>& lightingUniformBuffersMapped) = 0;
        virtual void addVPUniformBuffer(std::vector<void*>& vpUniformBuffersMapped) = 0;
        virtual void addTextureImageSamplerUniformBuffer(std::vector<int> &mipLevels, std::vector<std::array<bool,3>> &UVWRepeats) = 0;
        virtual void addDepthImageSamplerUniformBuffer() = 0;
        virtual void addLightDepthImageSamplerUniformBuffer() = 0;
        virtual void addLightDepthImageSamplerUniformBuffer_hardwareDepthBias() = 0;

        virtual void GraphicsDescriptorManagerDestroyAndFree() = 0;

        /**************************
         * Compute Descriptor
         * ***********************/
        virtual int GetComputeUniformTypes() = 0;
        virtual VkDescriptorSetLayout& GetComputeDescriptorSetLayout() = 0;
        virtual std::vector<CWxjBuffer>& GetStorageBuffers() = 0;
        virtual std::vector<VkDescriptorSet>& GetDescriptorSets() = 0;

        virtual void createComputeDescriptorPool() = 0;
        virtual void createComputeDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr) = 0;
        virtual void createComputeDescriptorSets(VkImageView textureImageView = NULL, std::vector<VkImageView> *swapchainImageViews = NULL) = 0;

        virtual void addComputeCustomUniformBuffer(VkDeviceSize customUniformBufferSize) = 0;
        virtual void uploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) = 0;
        virtual void addStorageBuffer(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) = 0; //the same function to add storage 1&2
        virtual void uploadStorageBuffer(uint32_t currentFrame, const void* data, size_t size) = 0;
        virtual void downloadStorageBuffer(uint32_t currentFrame, void* data, size_t size) = 0;
        virtual void addStorageImage(VkBufferUsageFlags usage) = 0;

	    virtual void ComputeDescriptorManagerDestroyAndFree() = 0;

        /**************************
         * Swapchain
         * ***********************/
        virtual void ResizeSwapchain_buffer_depthlight(int size) = 0;
        virtual void ResizeSwapchain_framebuffers_shadowmap(int size) = 0;

        virtual VkSampleCountFlagBits GetSwapchainMSAASamples() = 0;
        virtual VkFormat GetSwapchainDepthFormat() = 0;
        virtual VkFormat GetSwapchainImageFormat() = 0;
        virtual int GetSwapchain_FrameBuffersSize_Shadowmap() = 0;
        virtual int GetSwapchain_BufferSize_Depthlight() = 0;
        virtual int GetSwapchain_ImageSize() = 0;
        virtual void SetSwapchain_ImageSize(int value) = 0;
        virtual void SetSwapchain_Compute_Image(bool value) = 0;
        virtual void GetSwapchainMaxUsableSampleCount() = 0;

        virtual void CreateSwapchain_attachment_resource_depthlight(VkSampleCountFlagBits msaaSamples) = 0;
        virtual void CreateSwapchain_attachment_resource_depthcamera() = 0;
        virtual void CreateSwapchain_attachment_resource_colorresolve() = 0;
        virtual void CreateSwapchainImages(VkSurfaceKHR surface, int width, int height) = 0;
        virtual void CreateSwapchainViews(VkImageAspectFlags aspectFlags) = 0;
        virtual void CreateFramebuffer_shadowmap(VkRenderPass &renderPass, int shadowmapIndex) = 0; //will use Resource#1
        virtual void CreateFramebuffer_mainscene(VkRenderPass &renderPass) = 0;  //will use Resource#1/2/3/4

        virtual void SetSwapchain_ShadowmapAttachmentDepthLight(int value) = 0;
        virtual void SetSwapchain_MainSceneAttachmentDepthLight(int value) = 0;
        virtual void SetSwapchain_MainSceneAttachmentDepthCamera(int value) = 0;
        virtual void SetSwapchain_MainSceneAttachmentColorResolve(int value) = 0;
        virtual void SetSwapchain_MainSceneAttachmentColorPresent(int value) = 0;
        virtual int GetSwapchain_ShadowmapAttachmentDepthLight() = 0;
        virtual int GetSwapchain_MainSceneAttachmentDepthLight() = 0;
        virtual int GetSwapchain_MainSceneAttachmentDepthCamera() = 0;
        virtual int GetSwapchain_MainSceneAttachmentColorResolve() = 0;
        virtual int GetSwapchain_MainSceneAttachmentColorPresent() = 0;

        virtual VkImageView GetSwapchain_Buffer_DepthLight_View(int index) = 0;
        virtual VkImageView GetSwapchain_Buffer_DepthCamera_View() = 0;
        virtual std::vector<VkImageView>& GetSwapchain_Views() = 0;
        virtual std::vector<VkImage>& GetSwapchain_Images() = 0;
        virtual VkSwapchainKHR GetSwapchainHandle() = 0;
        virtual std::vector<VkFramebuffer>& GetSwapchain_FrameBuffers_Mainscene() = 0;
        virtual std::vector<VkFramebuffer>& GetSwapchain_FrameBuffer_Shadowmap(int index) = 0;
        virtual VkExtent2D& GetSwapchainExtent() = 0;

        virtual void SetSwapchainDevice() = 0;
        virtual void SwapchainCleanup() = 0;

        /**************************
         * Context
         * ***********************/
        virtual VkSurfaceKHR& GetSurface() = 0;

        virtual void CreateInstance(const std::vector<const char*> &requiredValidationLayers, std::vector<const char*> &requiredExtensions, LELog::ILogCore *logger) = 0;
        virtual VkInstance GetInstance() = 0;

        virtual void CreatePhysicalDevice(const std::vector<const char*>  requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies, const std::vector<const char*> requiredValidationLayers) = 0;
        
        virtual VkDebugUtilsMessengerEXT GetDebugMessenger() = 0;
        virtual void ContextQuit() = 0;

        virtual QueueFamilyIndices GetQueueFamilyIndices() = 0;
        virtual VkDevice GetLogicalDevice() = 0;
        virtual VkPhysicalDevice GetPhysicalDevice() = 0;

        virtual VkQueue GetGraphicsQueue() = 0;
        virtual VkQueue GetPresentQueue() = 0;
        virtual VkQueue GetComputeQueue() = 0;

        virtual QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface, std::string s) = 0;
        virtual VkSampleCountFlagBits GetMaxUsableSampleCount() = 0;
        virtual SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) = 0;

    protected:
        //AppInfo appInfo;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
