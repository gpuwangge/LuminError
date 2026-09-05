#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeText.h"
#include <iostream>

//renderProcess constains all attachment descriptions
//when creating renderPass, it will use these attachment descriptions (as well as subpasses and dependencies) 

namespace LEGameEngine{
    class IGameEngine;
}

class AppInfo;


namespace LERenderer{

class CRenderProcess final{
public:
    CRenderProcess(){};
    ~CRenderProcess(){};

    void Cleanup();

    LEGameEngine::IGameEngine* game;

    /**************************
    * Attachments(Description) 
    **************************/
    int iShadowmapAttachmentDepthLight = -1; //this is for shadowmap renderpass, not main scene renderpass
    int iMainSceneAttachmentDepthLight = -1;
    int iMainSceneAttachmentDepthCamera = -1;
    int iMainSceneAttachmentColorresolve = -1;
    int iMainSceneAttachmentColorPresent = -1;

    void create_attachmentdescription_shadowmap_depthlight(VkFormat depthFormat);
    void create_attachmentdescription_mainscene_depthlight(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples);
    void create_attachmentdescription_mainscene_depthcamera(VkFormat depthFormat, VkSampleCountFlagBits msaaSamples);
    void create_attachmentdescription_mainscene_colorresolve(VkFormat swapChainImageFormat,VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT, VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    void create_attachmentdescription_mainscene_colorpresent(VkFormat swapChainImageFormat);

    VkAttachmentDescription attachmentdescription_shadowmap_depthlight{};
    VkAttachmentDescription attachmentdescription_mainscene_depthlight{};
    VkAttachmentDescription attachmentdescription_mainscene_depthcamera{};
    VkAttachmentDescription attachmentdescription_mainscene_colorresolve{};
    VkAttachmentDescription attachmentdescription_mainscene_colorpresent{};//these are descriptions, not attachment buffer, each has many(9) properties

    /*********
    * Attachments Reference
    * (number of refs can be different from descriptions)
    **********/
    //for shadowmap renderpass
    VkAttachmentReference attachmentRef_shadowmap_lightdepth{};

    //for mainscene renderpass: there are 3 subpass
    //subpass1.for subpass_shadowmap
    VkAttachmentReference attachmentRef_mainscene_lightdepth{};
    //subpass2.for subpass_draw
    std::vector<VkAttachmentReference> attachmentRef_mainscene_draw_input = {
        {0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
    };
    VkAttachmentReference attachmentRef_mainscene_draw_color{};
    VkAttachmentReference attachmentRef_mainscene_draw_depth{};
    VkAttachmentReference attachmentRef_mainscene_draw_color_multisample{};
    //subpass3.for subpass_observe
    VkAttachmentReference attachmentRef_mainscene_observe{}; //target: attachment-lightDepth or cameraDepth as input attachment
    VkAttachmentReference attachmentRef_mainscene_observe_color{};
    VkAttachmentReference attachmentRef_mainscene_observe_color_multisample{};
    

    /*********
    * Subpasses
    **********/
    bool bEnableShadowmapRenderpassSubpassShadowmap = false;   
    bool bEnableMainSceneRenderpassSubpassShadowmap = false;   
    bool bEnableMainSceneRenderpassSubpassDraw = true;
    bool bEnableMainSceneRenderpassSubpassObserve = false;

    std::vector<VkSubpassDescription> subpasses_shadowmap;
    void createSubpass_shadowmap(); //for shadowmap, this is the only subpass

    std::vector<VkSubpassDescription> subpasses_mainscene;
    void createSubpass_mainscene(int attachment_id_to_observe); //this function will call shadowmap/draw/observe
    void createSubpass_mainscene_lightdepth();
    void createSubpass_mainscene_draw();
    void createSubpass_mainscene_observe(int attachment_id_to_observe);


    /*********
    * Dependency
    **********/
    std::vector<VkSubpassDependency> dependencies_shadowmap;
    void createDependency_shadowmap();
    std::vector<VkSubpassDependency> dependencies_mainscene;
    void createDependency_mainscene();
    
    /*********
    * Renderpass
    **********/
    VkRenderPass renderPass_shadowmap = VK_NULL_HANDLE;
    void createRenderPass_shadowmap();
    VkRenderPass renderPass_mainscene = VK_NULL_HANDLE; 
    void createRenderPass_mainscene();
    

    /*********
    * Help variables and functions
    **********/
    std::vector<VkClearValue> clearValues;
    std::vector<VkClearValue> clearValues_shadowmap;

    bool bUseColorBlendAttachment = false;
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    void addColorBlendAttachment(VkBlendOp colorBlendOp, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, 
								 VkBlendOp alphaBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor);

    VkSampleCountFlagBits m_msaaSamples_renderProcess = VK_SAMPLE_COUNT_1_BIT;
    //VkFormat m_swapChainImageFormat;

    /*******************************
    * Pipeline Layouts and Pipelines
    ********************************/
    /*Layouts*/
    void createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id);
    void createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id);
    void createComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout);
    void createRaytracingPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout);

    /*Pipeline variables*/
    bool bCreateGraphicsPipeline = false;
    std::vector<VkPipelineLayout> graphicsPipelineLayouts;
    std::vector<VkPipeline> graphicsPipelines;  

    bool bCreateComputePipeline = false;
    VkPipelineLayout computePipelineLayout;
	VkPipeline computePipeline;

    bool bCreateRaytracingPipeline = false;
    VkPipelineLayout raytracingPipelineLayout;
    VkPipeline raytracingPipeline;
    
    /*Pipeline create functions*/
    
    using GetBindingDescFunc = VkVertexInputBindingDescription(*)();
    using GetAttributeDescFunc = std::vector<VkVertexInputAttributeDescription>(*)();
    void createGraphicsPipeline(GetBindingDescFunc getBindingDesc, GetAttributeDescFunc getAttributeDesc,
        VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer, bool bUseInstanceBuffer,
        VkRenderPass renderPass, int graphcisPipeline_id, AppInfo *appInfo);
    void createComputePipeline(VkShaderModule &computeShaderModule);
    void createRaytracingPipeline(VkShaderModule& rgenModule, VkShaderModule& primaryMissModule, VkShaderModule& shadowMissModule, 
        VkShaderModule& primaryRchitModule, VkShaderModule& shadowRchitModule,
        VkShaderModule& primaryRahitModule, VkShaderModule& shadowRahitModule,
        VkShaderModule &sphereIntersectionModule, VkShaderModule &spherePrimaryRchitModule, VkShaderModule &sphereShadowRchitModule);

    PFN_vkCreateRayTracingPipelinesKHR             fpCreateRayTracingPipelinesKHR             = nullptr;
    bool LoadRayTracingFunctions_process();

};


}//namespace