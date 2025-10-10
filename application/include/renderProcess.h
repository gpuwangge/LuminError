#ifndef H_RENDERPROCESS
#define H_RENDERPROCESS

#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"

//renderProcess constains all attachment descriptions
//when creating renderPass, it will use these attachment descriptions (as well as subpasses and dependencies) 

class CRenderProcess final{
public:
    CRenderProcess(){};
    ~CRenderProcess(){};

    void Cleanup();

    /**************************
    * Attachments(Description) 
    **************************/
    int iShadowmapAttachmentDepthLight = -1; //this is for shadowmap renderpass, not main scene renderpass
    int iMainSceneAttachmentDepthLight = -1;
    int iMainSceneAttachmentDepthCamera = -1;
    int iMainSceneAttachmentColorResovle = -1;
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
    void createComputePipelineLayout(VkDescriptorSetLayout &descriptorSetLayout);

    void createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, int graphicsPipelineLayout_id);
    void createGraphicsPipelineLayout(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkPushConstantRange &pushConstantRange, bool bUsePushConstant, int graphicsPipelineLayout_id);

    bool bCreateComputePipeline = false;
    VkPipelineLayout computePipelineLayout;
	VkPipeline computePipeline;

    bool bCreateGraphicsPipeline = false;
    std::vector<VkPipelineLayout> graphicsPipelineLayouts;
    std::vector<VkPipeline> graphicsPipelines;  
    int skyboxID = -1;
    
    void createComputePipeline(VkShaderModule &computeShaderModule);

    struct DummyVertex {
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            return bindingDescription;
        }
        static std::array<VkVertexInputAttributeDescription, 0> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 0> attributeDescriptions{};
            return attributeDescriptions;
        }
    };

    /*******
     * 
     * Graphics Pipeline Template
     * 
     * 
     * *****/
    //this function is for samples that are NOT using vertex shader
    void createGraphicsPipeline(VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, int graphcisPipeline_id, 
        int subpass_id, bool bEnableDepthBias, VkRenderPass renderPass){
        createGraphicsPipeline<DummyVertex>(topology, vertShaderModule, fragShaderModule, false, false, graphcisPipeline_id, subpass_id, bEnableDepthBias, renderPass); //DummyVertex doesn't really matter here, because no vertex attributes used
    }

    //this function is for samples that are  using vertex shader
    template <typename T>
    void createGraphicsPipeline(VkPrimitiveTopology topology, VkShaderModule &vertShaderModule, VkShaderModule &fragShaderModule, bool bUseVertexBuffer, bool bUseInstanceBuffer,
        int graphcisPipeline_id, int subpass_id, bool bEnableDepthBias, VkRenderPass renderPass){
        //HERE_I_AM("CreateGraphicsPipeline");
        bCreateGraphicsPipeline = true;

        VkResult result = VK_SUCCESS;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
       

        /*********1 Asemble Shader**********/
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        
        /*********2 Asemble Vertex Info**********/
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        auto bindingDescription = T::getBindingDescription();
        auto attributeDescriptions = T::getAttributeDescriptions();

        if(bUseInstanceBuffer){
            //Combine two bindings
            auto bindingDescriptions = std::array{
                TextQuadVertex::getBindingDescription(),
                TextInstanceData::getBindingDescription()
            };
            auto vertexAttributes = TextQuadVertex::getAttributeDescriptions();
            auto instanceAttributes = TextInstanceData::getAttributeDescriptions();
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
            attributeDescriptions.insert(attributeDescriptions.end(), vertexAttributes.begin(), vertexAttributes.end());
            attributeDescriptions.insert(attributeDescriptions.end(), instanceAttributes.begin(), instanceAttributes.end());

            //vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        }else if(bUseVertexBuffer){     
            //NTODO: particle and vertex3d
            // if (pt == PIPELINE_COMPUTE) {
            // 	auto bindingDescription = Particle::getBindingDescription();
            // 	auto attributeDescriptions = Particle::getAttributeDescriptions();

            // 	vertexInputInfo.vertexBindingDescriptionCount = 1;
            // 	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            // 	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            // 	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
            // }
            // else {
                //auto bindingDescription = Vertex3D::getBindingDescription();
                //auto attributeDescriptions = Vertex3D::getAttributeDescriptions();
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
            //}
        }   
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        /*********3 Assemble**********/
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        pipelineInfo.pInputAssemblyState = &inputAssembly;

        /*********4 Viewport**********/
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        pipelineInfo.pViewportState = &viewportState;
        

        /*********5 Rasterizazer**********/
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        if(bEnableDepthBias){
            //std::cout<< "Enable depthBiasEnable for rasterizer" << std::endl;
            rasterizer.depthBiasEnable = VK_TRUE; // for hardware depthibias shadowmap
            rasterizer.depthBiasConstantFactor = 1.25f;   // for hardware depthibias shadowmap
            rasterizer.depthBiasClamp = 0.0f; // for hardware depthibias shadowmap
            rasterizer.depthBiasSlopeFactor = 1.75f;      // for hardware depthibias shadowmap
        }else{
            //std::cout<< "Disable depthBiasEnable" << std::endl;
            rasterizer.depthBiasEnable = VK_FALSE;
        }
        pipelineInfo.pRasterizationState = &rasterizer;
        

        /*********6 Multisample**********/
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        if(bEnableDepthBias)
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //for hardware depthbias shadowmap, we use 1 sample
        else
            multisampling.rasterizationSamples = m_msaaSamples_renderProcess;
        pipelineInfo.pMultisampleState = &multisampling; 

        /*********7 Color Blend**********/
        //VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        if(!bUseColorBlendAttachment){
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        pipelineInfo.pColorBlendState = &colorBlending;	//7 
        

        /*********8**********/
        //tell gpu which part will be changed, so need update these for each frame
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        if(bEnableDepthBias){
            dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
        }
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        pipelineInfo.pDynamicState = &dynamicState;	//8 
         

        /*********9 Layout(Vulkan Special Concept)**********/
        //CreateLayout(descriptorSetLayout);
        pipelineInfo.layout = graphicsPipelineLayouts[graphcisPipeline_id];	//9
        //pipelineInfo.layout = graphicsPipelineLayout;	//9
       
        
        /*********10 Renderpass Layout(Vulkan Special Concept)**********/
        //Renderpass is to specify what kind of data goes to graphics pipeline
        pipelineInfo.renderPass = renderPass;	//10 
        pipelineInfo.subpass = subpass_id; 
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        /*********11**********/
        if (iMainSceneAttachmentDepthCamera >= 0) {
            bool bSkybox = false;
            if(graphicsPipelines.size() == skyboxID) bSkybox = true;
            //std::cout<<"bSkybox="<<bSkybox<<"(skyboxID="<<skyboxID<<")"<<std::endl;
            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE; //change this to true will make text invisible
            depthStencil.depthWriteEnable = VK_TRUE;
            if(bEnableDepthBias)
                depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; //for hardware depthbias shadowmap
            else
                depthStencil.depthCompareOp = bSkybox ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.stencilTestEnable = VK_FALSE;
            pipelineInfo.pDepthStencilState = &depthStencil;
        }

        /*********Create Graphics Pipeline**********/
        VkPipeline newpipeline;
        graphicsPipelines.push_back(newpipeline);
        //std::cout<<"begin create graphics pipeline "<<std::endl;
        result = vkCreateGraphicsPipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipelines[graphcisPipeline_id]);
        //std::cout<<"done create graphcis pipeline "<<std::endl;
        //result = vkCreateGraphicsPipelines(CContext::GetHandle().GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
        if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics pipeline!");
        //REPORT("vkCreateGraphicsPipelines");


        /*********Clean up**********/
        

        //HERE_I_AM("DrawFrame() will begin");
    }

    //CDebugger * debugger;
};


#endif