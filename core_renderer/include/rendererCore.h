#pragma once
#include "IRendererCore.h"
#include <vulkan/vulkan.h>
#include <vector>
//#include <deque>
#include "TypeDataBuffer.h"
#include "Enum.h"
#include "renderProcess.h"
#include "graphicsDescriptor.h"
#include "computeDescriptor.h"
#include "raytracingDescriptor.h"
#include "swapchain.h"

#include "instance.h"
#include "context.h"

#include "ILogCore.h"

//added this to remove windows.h
#ifdef _WIN32
// 前向声明 HMODULE
//typedef void* HMODULE;
// 或者更精确的方式：
struct HINSTANCE__;
typedef HINSTANCE__* HMODULE;
#endif


namespace LERenderer{
    class RendererCore final : public IRendererCore{
    public:
        RendererCore(){
            CContext::Init();
        }
        ~RendererCore(){}

        //Module Related
        HMODULE handle_module_logcore;
        LELog::ILogCore *logger = NULL;
        void DestroyInstance(HMODULE handle, void* instance);

        //Expose to application
        void SetRenderMode(int value) override { m_renderMode = (RenderModes)value; }
        int GetRenderMode() override { return m_renderMode; }
        void SetEnableObject(bool value) override { bEnableObject = value; }
        int GetEnableObject() override { return bEnableObject; }
        void SetEnableText(bool value) override { bEnableText = value; }
        int GetEnableText() override { return bEnableText; }
        uint32_t GetCurrentFrame() override { return currentFrame; }
        uint32_t GetCurrentImage() override { return currentImage; }
        void SetCurrentFrame(uint32_t value) override { currentFrame = value; }
        VkCommandPool& GetCommandPool() override { return commandPool; }
        VkCommandBuffer& GetGraphicsCommandBuffer() override { return commandBuffers[graphicsCmdId][currentFrame]; }
        VkCommandBuffer& GetComputeCommandBuffer() override { return commandBuffers[computeCmdId][currentFrame]; }
        VkCommandBuffer& GetRaytracingCommandBuffer() override { return commandBuffers[raytracingCmdId][currentFrame]; }
        std::vector<VkCommandBuffer>& GetComputeCommandBuffers() override { return commandBuffers[computeCmdId]; }

        /**************************
         * Universial Render Functions
         * ***********************/
        RenderModes m_renderMode = GRAPHICS;

        bool bEnableObject = false;
        bool bEnableText = false;
        //bool bUseGlobal = false;

        void AquireSwapchainImage(VkSwapchainKHR swapchainHandle, bool bVerbose = false) override;
        void WaitForComputeFence() override;
        void WaitForRaytracingFence() override;
        void SubmitCompute(bool bVerbose = false) override;
        void SubmitRaytracing(bool bVerbose = false) override;
        void WaitForGraphicsFence() override;
        void SubmitGraphics(bool bVerbose = false) override;
        void PresentSwapchainImage(VkSwapchainKHR swapchainHandle, bool bVerbose = false) override; 

        /**************************
         * Graphics Shader Functions
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
        void BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, uint32_t dynamicObjectOffset = -1, uint32_t dynamicTextOffset = -1) override;
        void BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectOffset = -1, uint32_t dynamicTextOffset = -1) override;
        void BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets) override;
        void BindRaytracingDescriptorSets(VkPipelineLayout &pipelineLayout,  std::vector<std::vector<VkDescriptorSet>> &descriptorSets) override;

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

        /**************************
         * Raytracing Shader Functions
         * ***********************/
        void StartRecordRaytracingCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout) override;
        void EndRecordRaytracingCommandBuffer() override;

        void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;
        void Trace(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;

        /**************************
         * Utility Functions
         * ***********************/
        void RecordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                VkAccessFlags scrAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) override;

        /**************************
         * MISC Functions
         * ***********************/
        void CreateVertexBuffer (void* data, size_t elementSize, size_t elementCount) override;
        void CreateIndexBuffer (std::vector<uint32_t> &indices3D) override;
        //void CreateInstanceBuffer(std::vector<TextInstanceData> &instanceData);

        int graphicsCmdId = 0;
        int computeCmdId = 0;
        int raytracingCmdId = 0;
        void CreateCommandPool(VkSurfaceKHR &surface) override;
        void CreateGraphicsCommandBuffer() override;
        void CreateComputeCommandBuffer() override;
        void CreateRaytracingCommandBuffer() override;
        void CreateCommandBuffers();

        void CreateInitSyncObjects() override;
        void CreateSyncObjects(int swapchainSize, bool bVerbose = false) override;

        void Destroy() override;

        uint32_t currentFrame = 0;
        uint32_t currentImage = 0;
        void Update() override; //update currentFrame

        std::vector<CWxjBuffer> vertexDataBuffers;  //each buffer object is for one model object, the index in this vector is object.id
        std::vector<CWxjBuffer> indexDataBuffers;
        //std::vector<CWxjBuffer> instanceDataBuffers;
        std::vector<std::vector<uint32_t>> indices3Ds;
        std::vector<std::vector<VkCommandBuffer>> commandBuffers;  //commandBuffers[Size][MAX_FRAMES_IN_FLIGHT or currentFrame]
        VkCommandPool commandPool;

        VkCommandPool initCommandPool;
        VkCommandBuffer initCommandBuffer;


        int semaphoreIndex = 0;
        //std::deque<VkSemaphore> availableSemaphores;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        std::vector<VkSemaphore> computeFinishedSemaphores;
        std::vector<VkFence> computeInFlightFences;

        std::vector<VkSemaphore> raytracingFinishedSemaphores;
        std::vector<VkFence> raytracingInFlightFences;

        VkFence rtInitFence = VK_NULL_HANDLE;

        void SetApplication(LEGameEngine::IGameEngine* pApplication) override;
        void LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName);


        /*********
        * Ray Tracing Related
        *********/
        void InitialRaytracing() override;
        void CreateSBT() override;

        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
        void QueryRayTracingProperties();

        PFN_vkGetRayTracingShaderGroupHandlesKHR       fpGetRayTracingShaderGroupHandlesKHR       = nullptr;
        PFN_vkGetBufferDeviceAddressKHR                fpGetBufferDeviceAddressKHR                = nullptr;
        PFN_vkCmdTraceRaysKHR                          fpCmdTraceRaysKHR                          = nullptr;
        
        PFN_vkGetAccelerationStructureBuildSizesKHR    fpGetAccelerationStructureBuildSizesKHR    = nullptr; //used in create blas
        PFN_vkCreateAccelerationStructureKHR           fpCreateAccelerationStructureKHR           = nullptr; //used in create blas
        PFN_vkCmdBuildAccelerationStructuresKHR        fpCmdBuildAccelerationStructuresKHR        = nullptr; //used in create blas
        PFN_vkGetAccelerationStructureDeviceAddressKHR fpGetAccelerationStructureDeviceAddressKHR = nullptr; //used in create blas
        PFN_vkDestroyAccelerationStructureKHR          fpDestroyAccelerationStructureKHR          = nullptr; //used in create blas

        PFN_vkBuildAccelerationStructuresKHR           fpBuildAccelerationStructuresKHR           = nullptr; //not used yet, this is optional?

        bool LoadRayTracingFunctions_core();

        VkDeviceAddress GetBufferAddress(VkDevice device, VkBuffer buffer);

        CWxjBuffer sbt_buffer;
        VkStridedDeviceAddressRegionKHR rgenRegion{};
        VkStridedDeviceAddressRegionKHR missRegion{};
        VkStridedDeviceAddressRegionKHR hitRegion{};
        VkStridedDeviceAddressRegionKHR callRegion{};
        static uint32_t AlignUp(uint32_t value, uint32_t alignment) {
            return (value + alignment - 1) & ~(alignment - 1);
        }
       
        //Blas related
        void BeginCommandBuffer_Raytracing(int commandBufferIndex);
        void EndCommandBuffer_Raytracing(int commandBufferIndex);
        void SubmitCommandBufferAndWait_Raytracing(int commandBufferIndex, VkQueue queue);
        //CWxjBuffer blas_buffer;
        CWxjBuffer blas_scratch_buffer;
        //VkAccelerationStructureKHR blas = VK_NULL_HANDLE;
        //VkDeviceAddress blasDeviceAddress = 0;
        void CreateTriangleBlas();

        //For Sphere BLAS AABB
        // struct SphereGpu{
        //     glm::vec3 center;
        //     float radius;
        //     uint32_t materialIndex;
        //     uint32_t pad0, pad1, pad2;
        // };
        //std::vector<VkAabbPositionsKHR> aabbs;
        //std::vector<SphereGpu> spheres;
        //CWxjBuffer sphereAabbBuffer;
        //std::vector<VkAabbPositionsKHR> sphereAabbBuffer;
        //CWxjBuffer blas_sphere_scratch_buffer;

        //All spheres share one BLAS
        VkAccelerationStructureKHR sphere_blas = VK_NULL_HANDLE;
        CWxjBuffer sphere_blasBuffer;
        VkDeviceAddress sphere_blasAddress = 0;//blasDeviceAddress
        void CreateSphereBlas();

        //Instance buffer related
        std::vector<VkAccelerationStructureInstanceKHR> instances;

        CWxjBuffer instance_buffer; //only need one instance buffer, with all instances inside
        VkDeviceAddress instanceBufferAddress = 0;
        //uint32_t instanceCount = 0;
        void CreateInstanceBuffer();

        //Tlas related
        CWxjBuffer tlas_buffer;
        CWxjBuffer tlas_scratch_buffer;
        VkDeviceAddress tlasDeviceAddress = 0;
        VkAccelerationStructureKHR tlas = VK_NULL_HANDLE;
        VkAccelerationStructureKHR GetTlas() override { return tlas; }
        void CreateTlas();

        /**************************
         * RenderProcess
         * ***********************/
        CRenderProcess renderProcess;

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
        void CreateRaytracingPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout) override { renderProcess.createRaytracingPipelineLayout(descriptorSetLayout); }
        void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id) override { renderProcess.createGraphicsPipelineLayout(descriptorSetLayouts, graphicsPipelineLayout_id); }
        void CreateGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id) override {
            renderProcess.createGraphicsPipelineLayout(descriptorSetLayouts, pushConstantRange, bUsePushConstant, graphicsPipelineLayout_id);
        }
        void CreateComputePipeline(VkShaderModule &computeShaderModule) override { renderProcess.createComputePipeline(computeShaderModule); }
        void CreateRaytracingPipeline(VkShaderModule &rgenModule, VkShaderModule &primaryMissModule, VkShaderModule &shadowMissModule, 
            VkShaderModule &primaryRchitModule, VkShaderModule &shadowRchitModule,
            VkShaderModule &primaryRahitModule, VkShaderModule &shadowRahitModule,
            VkShaderModule &sphereIntersectionModule, VkShaderModule &spherePrimaryRchitModule, VkShaderModule &sphereShadowRchitModule) override { 
            renderProcess.createRaytracingPipeline(rgenModule, primaryMissModule, shadowMissModule, 
                primaryRchitModule, shadowRchitModule, primaryRahitModule, shadowRahitModule, 
                sphereIntersectionModule, spherePrimaryRchitModule, sphereShadowRchitModule); 
        }
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
        VkPipelineLayout& GetRaytracingPipelineLayout() override { return renderProcess.raytracingPipelineLayout; }
        VkPipeline& GetRaytracingPipeline() override { return renderProcess.raytracingPipeline; }
        
        VkPipelineLayout& GetGraphicsPipelineLayout(int pipelineId) override { return renderProcess.graphicsPipelineLayouts[pipelineId]; }
        VkPipeline& GetGraphicsPipeline(int pipelineId) override { return renderProcess.graphicsPipelines[pipelineId]; }
        std::vector<VkPipelineLayout> GetGraphicsPipelineLayouts() override { return renderProcess.graphicsPipelineLayouts; }
        std::vector<VkPipeline> GetGraphicsPipelines() override { return renderProcess.graphicsPipelines; }

        std::vector<VkClearValue>& GetClearValues() override { return renderProcess.clearValues; }
        std::vector<VkClearValue>& GetClearValues_shadowmap() override { return renderProcess.clearValues_shadowmap; }

        void RenderProcessCleanup() { renderProcess.Cleanup(); }

        /**************************
         * Graphics Descriptor
         * ***********************/
        CGraphicsDescriptorManager graphicsDescriptorManager;

        int GetTextureImageSamplersSize() override { return graphicsDescriptorManager.textureImageSamplers.size(); }
        int GetGraphicsUniformTypes() override { return graphicsDescriptorManager.graphicsUniformTypes; }
        void SetGraphicsUniformTypes(int value) override { graphicsDescriptorManager.graphicsUniformTypes = value; }
        VkDescriptorSetLayout GetDescriptorSetLayout_General() override { return graphicsDescriptorManager.descriptorSetLayout_general; }
        VkDescriptorSetLayout& GetDescriptorSetLayout_TextureImageSampler() override { return graphicsDescriptorManager.descriptorSetLayout_textureImageSampler; }
        int GetSetSize_General() override { return graphicsDescriptorManager.getSetSize_General(); }

        std::vector<VkDescriptorSet>& GetDescriptorSets_General() override { return graphicsDescriptorManager.descriptorSets_general; }
        VkDescriptorPool& GetGraphicsDescriptorPool() override { return graphicsDescriptorManager.graphicsDescriptorPool; }
        //VkDescriptorSetLayout& GetDescriptorSetLayout_TextureImageSampler() override { return graphicsDescriptorManager.descriptorSetLayout_textureImageSampler; }
        std::vector<VkSampler>& GetTextureImageSamplers() override { return graphicsDescriptorManager.textureImageSamplers; }

        void createGraphicsDescriptorPool(unsigned int object_textbox_count = 0) override { graphicsDescriptorManager.createDescriptorPool(object_textbox_count); }
        void createGraphicsDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding = nullptr) override { graphicsDescriptorManager.createDescriptorSetLayout_General(customBinding); }
        void createGraphicsDescriptorSetLayout_TextureImageSampler() override { graphicsDescriptorManager.createDescriptorSetLayout_TextureImageSampler(); }
        void createGraphicsDescriptorSets_General(VkImageView depthImageView, std::vector<VkImageView> &depthlight_imageviews) override { graphicsDescriptorManager.createDescriptorSets_General(depthImageView, depthlight_imageviews); }

        void AddObjectUniformBuffer(std::vector<void*>& objectUniformBuffersMapped) override { graphicsDescriptorManager.addObjectUniformBuffer(objectUniformBuffersMapped); }
        void AddTextUniformBuffer(std::vector<void*>& textUniformBuffersMapped) override { graphicsDescriptorManager.addTextUniformBuffer(textUniformBuffersMapped); }
        void addGraphicsCustomUniformBuffer(VkDeviceSize customUniformBufferSize) override { graphicsDescriptorManager.addCustomUniformBuffer(customUniformBufferSize); }
        void uploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) override { graphicsDescriptorManager.uploadCustomUniformBuffer(currentFrame, data, dataSize); }
        void addLightingUniformBuffer(std::vector<void*>& lightingUniformBuffersMapped) override { graphicsDescriptorManager.addLightingUniformBuffer(lightingUniformBuffersMapped); }
        void addGraphicsGlobalUniformBuffer() override { graphicsDescriptorManager.addGlobalUniformBuffer(); }
        void uploadGraphicsGlobalUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) override { graphicsDescriptorManager.uploadGlobalUniformBuffer(currentFrame, data, dataSize); }
        void addTextureImageSamplerUniformBuffer(std::vector<int> &mipLevels, std::vector<std::array<bool,3>> &UVWRepeats) override { graphicsDescriptorManager.addTextureImageSamplerUniformBuffer(mipLevels, UVWRepeats); }
        void addDepthImageSamplerUniformBuffer() override {graphicsDescriptorManager.addDepthImageSamplerUniformBuffer(); }
        void addLightDepthImageSamplerUniformBuffer() override { graphicsDescriptorManager.addLightDepthImageSamplerUniformBuffer(); }
        void addLightDepthImageSamplerUniformBuffer_hardwareDepthBias() override { graphicsDescriptorManager.addLightDepthImageSamplerUniformBuffer_hardwareDepthBias(); }

        virtual void GraphicsDescriptorManagerDestroyAndFree() override { graphicsDescriptorManager.DestroyAndFree(); }

        /**************************
         * Compute/Raytracing Descriptor
         * ***********************/
        CComputeDescriptorManager computeDescriptorManager;
        CRaytracingDescriptorManager raytracingDescriptorManager;

        int GetComputeUniformTypes() override { return computeDescriptorManager.computeUniformTypes; }
        VkDescriptorSetLayout& GetComputeDescriptorSetLayout() override { return computeDescriptorManager.descriptorSetLayout; }
        VkDescriptorSetLayout& GetRaytracingDescriptorSetLayout() override { return raytracingDescriptorManager.descriptorSetLayout; }
        std::vector<CWxjBuffer>& GetStorageBuffers() override { return computeDescriptorManager.storageBuffers_customswap; }
        std::vector<VkDescriptorSet>& GetComputeDescriptorSets() override { 
            return computeDescriptorManager.descriptorSets;
        }
        std::vector<VkDescriptorSet>& GetRaytracingDescriptorSets() override { 
            return raytracingDescriptorManager.descriptorSets;
        }

        void createComputeDescriptorPool() override { computeDescriptorManager.createDescriptorPool(); }
        void createComputeDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr) override { computeDescriptorManager.createDescriptorSetLayout(customBinding); }
        void createComputeDescriptorSets(VkImageView textureImageView = NULL) override { computeDescriptorManager.createDescriptorSets(textureImageView); }

        void createRaytracingDescriptorPool() override { raytracingDescriptorManager.createDescriptorPool(); }
        void createRaytracingDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding = nullptr) override { raytracingDescriptorManager.createDescriptorSetLayout(customBinding); }
        void createRaytracingDescriptorSets(VkImageView textureImageView, VkAccelerationStructureKHR tlas) override { raytracingDescriptorManager.createDescriptorSets(textureImageView, tlas); }

        void addComputeGlobalUniformBuffer() { computeDescriptorManager.addGlobalUniformBuffer(); }
        void uploadComputeGlobalUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) { computeDescriptorManager.uploadGlobalUniformBuffer(currentFrame, data, dataSize); }
        void addComputeCustomUniformBuffer(VkDeviceSize customUniformBufferSize) override { computeDescriptorManager.addCustomUniformBuffer(customUniformBufferSize); }
        void uploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) override { computeDescriptorManager.uploadCustomUniformBuffer(currentFrame, data, dataSize); }
        
        void addStorageBuffer_windowswap() override { computeDescriptorManager.addStorageBuffer_windowswap(); }
        void uploadStorageBuffer_windowswap(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_windowswap(currentFrame, data, size); }
        void downloadStorageBuffer_windowswap(uint32_t currentFrame, void* data, size_t size) override { computeDescriptorManager.downloadStorageBuffer_windowswap(currentFrame, data, size); }
        
        void addStorageBuffer_material() override { computeDescriptorManager.addStorageBuffer_material(); }
        void uploadStorageBuffer_material(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_material(currentFrame, data, size); }
        void addStorageBuffer_triangleVertexAttribute() override { computeDescriptorManager.addStorageBuffer_triangleVertexAttribute(); }
        void uploadStorageBuffer_triangleVertexAttribute(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_triangleVertexAttribute(currentFrame, data, size); }
        void addStorageBuffer_triangleVertexIndex() override { computeDescriptorManager.addStorageBuffer_triangleVertexIndex(); }
        void uploadStorageBuffer_triangleVertexIndex(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_triangleVertexIndex(currentFrame, data, size); }
        void addStorageBuffer_triangleReorderIndex() override { computeDescriptorManager.addStorageBuffer_triangleReorderIndex(); }
        void uploadStorageBuffer_triangleReorderIndex(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_triangleReorderIndex(currentFrame, data, size); }
        void addStorageBuffer_bvhNode() override { computeDescriptorManager.addStorageBuffer_bvhNode(); }
        void uploadStorageBuffer_bvhNode(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_bvhNode(currentFrame, data, size); }
        void addStorageBuffer_sphere() override { computeDescriptorManager.addStorageBuffer_sphere(); }
        void uploadStorageBuffer_sphere(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_sphere(currentFrame, data, size); }

        //void addRaytracingStorageBuffer_triangleVertexAttribute() override { raytracingDescriptorManager.addStorageBuffer_triangleVertexAttribute(); }
        //void uploadRaytracingStorageBuffer_triangleVertexAttribute(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_triangleVertexAttribute(currentFrame, data, size); }
        //void addRaytracingStorageBuffer_triangleVertexIndex() override { raytracingDescriptorManager.addStorageBuffer_triangleVertexIndex(); }
        //void uploadRaytracingStorageBuffer_triangleVertexIndex(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_triangleVertexIndex(currentFrame, data, size); }
        void addRaytracingStorageBuffer_geometryInfo() override { raytracingDescriptorManager.addStorageBuffer_geometryInfo(); }
        void uploadRaytracingStorageBuffer_geometryInfo(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_geometryInfo(currentFrame, data, size); }
        void addRaytracingStorageBuffer_material() override { raytracingDescriptorManager.addStorageBuffer_material(); }
        void uploadRaytracingStorageBuffer_material(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_material(currentFrame, data, size); }
        void addRaytracingStorageBuffer_global() override { raytracingDescriptorManager.addGlobalUniformBuffer_raytracing(); }
        void uploadRaytracingStorageBuffer_global(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadGlobalUniformBuffer_raytracing(currentFrame, data, size); }
        void addRaytracingStorageBuffer_custom(VkDeviceSize customUniformBufferSize) override { raytracingDescriptorManager.addCustomUniformBuffer_raytracing(customUniformBufferSize); }
        void uploadRaytracingStorageBuffer_custom(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadCustomUniformBuffer_raytracing(currentFrame, data, size); }
        void addRaytracingStorageBuffer_rtLight() override { raytracingDescriptorManager.addStorageBuffer_rtLight(); }
        void uploadRaytracingStorageBuffer_rtLight(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_rtLight(currentFrame, data, size); }
        void addRaytracingStorageBuffer_instance() override { raytracingDescriptorManager.addStorageBuffer_instance(); }
        void uploadRaytracingStorageBuffer_instance(uint32_t currentFrame, const void* data, size_t size) override { raytracingDescriptorManager.uploadStorageBuffer_instance(currentFrame, data, size); }

        void addStorageBuffer_customswap(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) override { computeDescriptorManager.addStorageBuffer_customswap(storageBufferSize, usage); }
        void uploadStorageBuffer_customswap(uint32_t currentFrame, const void* data, size_t size) override { computeDescriptorManager.uploadStorageBuffer_customswap(currentFrame, data, size); }
        void downloadStorageBuffer_customswap(uint32_t currentFrame, void* data, size_t size) override { computeDescriptorManager.downloadStorageBuffer_customswap(currentFrame, data, size); }
        
        void addStorageImage(VkBufferUsageFlags usage) override {
            if(usage == COMPUTE_STORAGEIMAGE_SWAPCHAIN){
                computeDescriptorManager.addStorageImage(usage);
                //std::cout<<"Added Compute Storage Image for Swapchain"<<std::endl;
            }else if(usage == COMPUTE_STORAGEIMAGE_TEXTURE){
                computeDescriptorManager.addStorageImage(usage);
                //std::cout<<"Added Compute Storage Image for Texture"<<std::endl;
            }   
            else if(usage == RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
                raytracingDescriptorManager.addStorageImage(usage);
                //std::cout<<"Added Raytracing Storage Image for Swapchain"<<std::endl;
            }
        }

	    void ComputeDescriptorManagerDestroyAndFree() override { computeDescriptorManager.DestroyAndFree(); }
        void RaytracingDescriptorManagerDestroyAndFree() override { raytracingDescriptorManager.DestroyAndFree(); }

        /**************************
         * Swapchain
         * ***********************/
        CSwapchain swapchain;

        void ResizeSwapchain_buffer_depthlight(int size) override { swapchain.buffer_depthlight.resize(size); }
        void ResizeSwapchain_framebuffers_shadowmap(int size) override { swapchain.framebuffers_shadowmap.resize(size); }

        VkSampleCountFlagBits GetSwapchainMSAASamples() override { return swapchain.msaaSamples; }
        VkFormat GetSwapchainDepthFormat() override { return swapchain.depthFormat; }
        VkFormat GetSwapchainImageFormat() override { return swapchain.swapChainImageFormat; }
        int GetSwapchain_FrameBuffersSize_Shadowmap() override { return swapchain.framebuffers_shadowmap.size(); }
        int GetSwapchain_BufferSize_Depthlight() override { return swapchain.buffer_depthlight.size(); }
        int GetSwapchain_ImageSize() override { return swapchain.swapchainImageSize; }
        void SetSwapchain_ImageSize(int value) override { swapchain.swapchainImageSize = value; }
        //void SetSwapchain_Compute_Image(bool value) override { swapchain.bComputeSwapChainImage = value; }
        void GetSwapchainMaxUsableSampleCount() override { swapchain.GetMaxUsableSampleCount(); }

        void CreateSwapchain_attachment_resource_depthlight(VkSampleCountFlagBits msaaSamples) override { swapchain.create_attachment_resource_depthlight(msaaSamples); }
        void CreateSwapchain_attachment_resource_depthcamera() override { swapchain.create_attachment_resource_depthcamera(); }
        void CreateSwapchain_attachment_resource_colorresolve() override { swapchain.create_attachment_resource_colorresolve(); }
        void CreateSwapchainImages(VkSurfaceKHR surface, int width, int height) override { swapchain.createSwapchainImages(surface, width, height); }
        void CreateSwapchainViews(VkImageAspectFlags aspectFlags) override { swapchain.createSwapchainViews(aspectFlags); }
        void CreateFramebuffer_shadowmap(VkRenderPass &renderPass, int shadowmapIndex) override { swapchain.CreateFramebuffer_shadowmap(renderPass, shadowmapIndex); }
        void CreateFramebuffer_mainscene(VkRenderPass &renderPass) override { swapchain.CreateFramebuffer_mainscene(renderPass); }  //will use Resource#1/2/3/4

        void SetSwapchain_ShadowmapAttachmentDepthLight(int value) override { swapchain.iShadowmapAttachmentDepthLight = value; }
        void SetSwapchain_MainSceneAttachmentDepthLight(int value) override { swapchain.iMainSceneAttachmentDepthLight = value; }
        void SetSwapchain_MainSceneAttachmentDepthCamera(int value) override { swapchain.iMainSceneAttachmentDepthCamera = value; }
        void SetSwapchain_MainSceneAttachmentColorResolve(int value) override { swapchain.iMainSceneAttachmentColorResovle = value; }
        void SetSwapchain_MainSceneAttachmentColorPresent(int value) override { swapchain.iMainSceneAttachmentColorPresent = value; }
        int GetSwapchain_ShadowmapAttachmentDepthLight() override { return swapchain.iShadowmapAttachmentDepthLight; }
        int GetSwapchain_MainSceneAttachmentDepthLight() override { return swapchain.iMainSceneAttachmentDepthLight; }
        int GetSwapchain_MainSceneAttachmentDepthCamera() override { return swapchain.iMainSceneAttachmentDepthCamera; }
        int GetSwapchain_MainSceneAttachmentColorResolve() override { return swapchain.iMainSceneAttachmentColorResovle; }
        int GetSwapchain_MainSceneAttachmentColorPresent() override { return swapchain.iMainSceneAttachmentColorPresent; }

        VkImageView GetSwapchain_Buffer_DepthLight_View(int index) override { return swapchain.buffer_depthlight[index].view; }
        VkImageView GetSwapchain_Buffer_DepthCamera_View() override { return swapchain.buffer_depthcamera.view; }
        std::vector<VkImageView>& GetSwapchain_Views() override { return swapchain.swapchain_views; }
        std::vector<VkImage>& GetSwapchain_Images() override { return swapchain.swapchain_images; }
        VkImage GetIntermediaColor_Image(int index) override { return swapchain.intermediaColor[index].image; }
        VkSwapchainKHR GetSwapchainHandle() override { return swapchain.getHandle(); }
        std::vector<VkFramebuffer>& GetSwapchain_FrameBuffers_Mainscene() override { return swapchain.framebuffers_mainscene; }
        std::vector<VkFramebuffer>& GetSwapchain_FrameBuffer_Shadowmap(int index) override { return swapchain.framebuffers_shadowmap[index]; }
        VkExtent2D& GetSwapchainExtent() override { return swapchain.swapChainExtent; }

        void SetSwapchainDevice() override { swapchain.SetDevice(); }
        void SwapchainCleanup() override { swapchain.CleanUp(); }

        /**************************
         * Context
         * ***********************/
        std::unique_ptr<CInstance> instance{nullptr};

        VkSurfaceKHR surface;//03
        VkSurfaceKHR& GetSurface() override { return surface; }

        void CreateInstance(const std::vector<const char*> &requiredValidationLayers, std::vector<const char*> &requiredExtensions) override {
            instance = std::make_unique<CInstance>(requiredValidationLayers, requiredExtensions, logger);
        }
        VkInstance GetInstance() override { return instance.get()->getHandle(); }

        void CreatePhysicalDevice(const std::vector<const char*> requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies, const std::vector<const char*> requiredValidationLayers, const bool enableRaytracingPipeline) override {
            instance->findAllPhysicalDevices();

            CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);
            //App dev can only query properties from physical device, but can not directly operate it
            //App dev operates logical device, can logical device communicate with physical device by command queues
            //App dev will fill command buffer with commands later
            //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
            CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions, enableRaytracingPipeline);
        }

        VkDebugUtilsMessengerEXT GetDebugMessenger() override {
            return instance->debugMessenger;
        }
        void ContextQuit() override {
            CContext::Quit();
        }

        QueueFamilyIndices GetQueueFamilyIndices() override { return CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool"); }
        VkDevice GetLogicalDevice() override { return CContext::GetHandle().GetLogicalDevice(); }
        VkPhysicalDevice GetPhysicalDevice() override { return CContext::GetHandle().GetPhysicalDevice(); }

        VkQueue GetGraphicsQueue() override{ return CContext::GetHandle().GetGraphicsQueue(); }
        VkQueue GetPresentQueue() override{ return CContext::GetHandle().GetPresentQueue(); }
        VkQueue GetComputeQueue() override{ return CContext::GetHandle().GetComputeQueue(); }

        QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface, std::string s) override { return CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, s); }
        VkSampleCountFlagBits GetMaxUsableSampleCount() override { return CContext::GetHandle().physicalDevice->get()->getMaxUsableSampleCount(); }
        SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) override { return CContext::GetHandle().physicalDevice->get()->querySwapChainSupport(surface); }

    };
    EXPORT_FACTORY_FOR(RendererCore);
}

