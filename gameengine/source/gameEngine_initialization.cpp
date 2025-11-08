#include "gameEngine.h"
#include "TypeVertex.h"
namespace LEGameEngine{

void GameEngine::Initialize(){
    bool bVerboseInitialization = false;
    TimePoint T0 = now();
    /****************************
    * 1 Process blend and push constant
    ****************************/   
    //renderer.m_renderMode = (RenderModes)appInfo->RenderMode;
    renderer->SetRenderMode(appInfo->RenderMode);
    if(appInfo->Feature.b_feature_graphics_push_constant)
        resourcer->CreateShaderPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPushConstants));
        //shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);
    if(appInfo->Feature.b_feature_graphics_global_blend)
        renderer->AddColorBlendAttachment(
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);        

    TimePoint T1 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading features", T0, T1);

    /****************************
    * 2 Initialize control nodes
    ****************************/
    if(appInfo->Feature.feature_graphics_enable_controls){
        controlNodes.push_back(std::make_unique<CControlPerfMetric>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlAttachment>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlGraphicsUniform>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlComputeUniform>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlSubpass>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlHotkey>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlFeature>());
        controlNodes.back()->Register(this);
        controlNodes.push_back(std::make_unique<CControlStatistics>());
        controlNodes.back()->Register(this);

        for(int i = 0; i < controlNodes.size(); i++) controlNodes[i]->bVisible = appInfo->Feature.feature_graphics_show_all_metric_controls;
        if(!appInfo->Feature.feature_graphics_show_all_metric_controls) controlNodes[0]->bVisible = appInfo->Feature.feature_graphics_show_performance_control; //show performance control only
    }

    /****************************
    * 3 Precompute size for object/textbox/light
    ****************************/
    objects.resize(appInfo->Objects.size() + objectCountControl);
    std::cout<<"Object Size: "<<objects.size()<<std::endl;
    
    textManager.m_textBoxes.resize(appInfo->Textboxes.size() + textboxCountControl);
    for(int i = 0; i < textManager.m_textBoxes.size(); i++)
        textManager.m_textBoxes[i].p_textManager = &textManager;
    std::cout<<"Textbox Size: "<<textManager.m_textBoxes.size()<<std::endl;
    
    lights.resize(appInfo->Lights.size() + lightCountControl);
    renderer->ResizeSwapchain_buffer_depthlight(lights.size());
    renderer->ResizeSwapchain_framebuffers_shadowmap(lights.size());
    std::cout<<"Light Size: "<<lights.size()<<std::endl;

    CLightManager::m_lightingUBO.lightNum = lights.size(); //update light number to ubo

    renderer->SetSwapchainDevice();
    
    TimePoint T2 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for object/textbox/light", T1, T2);

    /****************************
    * 4 Initialize Uniforms
    ****************************/
    if(appInfo->Uniform.b_uniform_graphics_custom) renderer->addGraphicsCustomUniformBuffer(appInfo->Uniform.GraphicsCustom.Size);
    if(appInfo->Uniform.b_uniform_graphics_lighting) renderer->addLightingUniformBuffer(CLightManager::m_lightingUniformBuffersMapped);
    if(appInfo->Uniform.b_uniform_graphics_object_mvp){
        renderer->addMVPUniformBuffer(CObjectManager::mvpUniformBuffersMapped);
        //renderer.bUseObjectMVP = true;
        renderer->SetEnableObjectMVP(true);
    }
    if(appInfo->Uniform.b_uniform_graphics_text_mvp){
        renderer->addTextMVPUniformBuffer(CTextManager::textMVPUniformBuffersMapped);
        //renderer.bUseTextboxMVP = true;
        renderer->SetEnableTextboxMVP(true);
    }   
    if(appInfo->Uniform.b_uniform_graphics_object_vp){
        renderer->addVPUniformBuffer(CObjectManager::vpUniformBuffersMapped);
        //renderer.bUseObjectMVP = true; //reuse MVP bool
        renderer->SetEnableObjectMVP(true);
    }
    if(appInfo->Uniform.b_uniform_graphics_global){
        renderer->addGlobalUniformBuffer();
        //renderer->SetEnableObjectMVP(true);
    }
    if(appInfo->Uniform.b_uniform_graphics_depth_image_sampler) renderer->addDepthImageSamplerUniformBuffer();
    if(appInfo->Uniform.b_uniform_graphics_lightdepth_image_sampler) renderer->addLightDepthImageSamplerUniformBuffer();
    if(appInfo->Uniform.b_uniform_graphics_lightdepth_image_sampler_hardware) renderer->addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();
    if(appInfo->Uniform.b_uniform_compute_custom) renderer->addComputeCustomUniformBuffer(appInfo->Uniform.ComputeCustom.Size);
    if(appInfo->Uniform.b_uniform_compute_storage) renderer->addStorageBuffer(appInfo->Uniform.ComputeStorageBuffer.Size, appInfo->Uniform.ComputeStorageBuffer.Usage);
    if(appInfo->Uniform.b_uniform_compute_texture_storage) renderer->addStorageImage(COMPUTE_STORAGEIMAGE_TEXTURE);
    if(appInfo->Uniform.b_uniform_compute_swapchain_storage) renderer->addStorageImage(COMPUTE_STORAGEIMAGE_SWAPCHAIN);

    if(appInfo->Samplers.size() > 0){
        //CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE;
        renderer->SetGraphicsUniformTypes(renderer->GetGraphicsUniformTypes() | GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE);
        std::vector<int> mipLevels;
        std::vector<std::array<bool,3>> UVWRepeats;
        for(int i = 0; i < appInfo->Samplers.size(); i++){
            mipLevels.push_back(appInfo->Samplers[i].sampler_miplevels);
            UVWRepeats.push_back(appInfo->Samplers[i].sampler_uvwRepeats);
        }
        renderer->addTextureImageSamplerUniformBuffer(mipLevels, UVWRepeats);
    }

    TimePoint T3 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for uniforms", T2, T3);

    /****************************
    * 5 Initialize attachments
    ****************************/
    //renderProcess.iShadowmapAttachmentDepthLight = appInfo->Attachment.bShadowmapAttachmentDepthLight ? 0 : -1; //shadowmap renderpass attachment depth light, only one attachment, so id is 0
    renderer->SetShadowmapAttachmentDepthLight(appInfo->Attachment.bShadowmapAttachmentDepthLight ? 0 : -1);

    int AttachmentCount = 0;
    renderer->SetMainSceneAttachmentDepthLight(appInfo->Attachment.bMainSceneAttachmentDepthLight ? AttachmentCount++ : -1);
    renderer->SetMainSceneAttachmentDepthCamera(appInfo->Attachment.bMainSceneAttachmentDepthCamera ? AttachmentCount++ : -1);
    renderer->SetMainSceneAttachmentColorResovle(appInfo->Attachment.bMainSceneAttachmentColorResovle ? AttachmentCount++ : -1);
    renderer->SetMainSceneAttachmentColorPresent(appInfo->Attachment.bMainSceneAttachmentColorPresent ? AttachmentCount++ : -1);

    renderer->SetSwapchain_ShadowmapAttachmentDepthLight(renderer->GetShadowmapAttachmentDepthLight());
    renderer->SetSwapchain_MainSceneAttachmentDepthLight(renderer->GetMainSceneAttachmentDepthLight());
    renderer->SetSwapchain_MainSceneAttachmentDepthCamera(renderer->GetMainSceneAttachmentDepthCamera());
    renderer->SetSwapchain_MainSceneAttachmentColorResolve(renderer->GetMainSceneAttachmentColorResovle());
    renderer->SetSwapchain_MainSceneAttachmentColorPresent(renderer->GetMainSceneAttachmentColorPresent());

    //when creating attachment resource, need 1.create attachment description in renderProcess; 2.create attachment buffer in swapchain
    if(renderer->GetMainSceneAttachmentColorResovle() >= 0) renderer->GetSwapchainMaxUsableSampleCount(); //calcuate max sampler count first

    if(renderer->GetShadowmapAttachmentDepthLight() >= 0){ //if shadowmap renderpass attachment depth light is enabled
        renderer->CreateSwapchain_attachment_resource_depthlight(VK_SAMPLE_COUNT_1_BIT); //hardware bias todo
        renderer->Create_attachmentdescription_shadowmap_depthlight(renderer->GetSwapchainDepthFormat());
    }else if(renderer->GetMainSceneAttachmentDepthLight() >= 0){
        renderer->CreateSwapchain_attachment_resource_depthlight(renderer->GetSwapchainMSAASamples());
        renderer->Create_attachmentdescription_mainscene_depthlight(renderer->GetSwapchainDepthFormat(), renderer->GetSwapchainMSAASamples());
    }
    if(renderer->GetMainSceneAttachmentDepthCamera() >= 0){//If enable MSAA, must also enable Depth Test
        renderer->CreateSwapchain_attachment_resource_depthcamera();
        renderer->Create_attachmentdescription_mainscene_depthcamera(renderer->GetSwapchainDepthFormat(), renderer->GetSwapchainMSAASamples());
    }
    if(renderer->GetMainSceneAttachmentColorResovle() >= 0){
        renderer->CreateSwapchain_attachment_resource_colorresolve();
        renderer->Create_attachmentdescription_mainscene_colorresolve(renderer->GetSwapchainImageFormat(), renderer->GetSwapchainMSAASamples(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
    if(renderer->GetMainSceneAttachmentColorPresent() >= 0) //dont need create swapchain attachment resource here
        renderer->Create_attachmentdescription_mainscene_colorpresent(renderer->GetSwapchainImageFormat());

    TimePoint T4 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for attachements", T3, T4);
    

    /****************************
    * 6 Initialize Subpasses
    ****************************/
    renderer->SetEnableShadowmapRenderpassSubpassShadowmap(appInfo->Subpass.bEnableShadowmapRenderpassSubpassShadowmap);
    renderer->SetEnableMainSceneRenderpassSubpassShadowmap(appInfo->Subpass.bEnableMainSceneRenderpassSubpassShadowmap);
    renderer->SetEnableMainSceneRenderpassSubpassDraw(appInfo->Subpass.bEnableMainSceneRenderpassSubpassDraw);
    renderer->SetEnableMainSceneRenderpassSubpassObserve(appInfo->Subpass.bEnableMainSceneRenderpassSubpassObserve);
    //for shadowmap renderpass (this renderpass is optional)
    //if(renderProcess.bEnableShadowmapRenderpassSubpassShadowmap){
    if(renderer->GetEnableShadowmapRenderpassSubpassShadowmap()){
        // std::cout<<"Application: Create Shadowmap Render Pass."<<std::endl;
        renderer->CreateSubpass_shadowmap();
        renderer->CreateDependency_shadowmap();
        renderer->CreateRenderPass_shadowmap();

        // std::cout<<"Application: Create Shadowmap Framebuffer."<<std::endl;
        for(int i = 0; i < renderer->GetSwapchain_FrameBuffersSize_Shadowmap(); i++)
            renderer->CreateFramebuffer_shadowmap(renderer->GetRenderpass_shadowmap(), i);
    }
    
    //for mainscene renderpass (this renderpass is mandatory)
    renderer->CreateSubpass_mainscene(appInfo->Feature.feature_graphics_observe_attachment_id);
    renderer->CreateDependency_mainscene();
    renderer->CreateRenderPass_mainscene();
    //create framebuffer
    renderer->CreateFramebuffer_mainscene(renderer->GetRenderpass_mainscene());

    TimePoint T5 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading subpasses", T4, T5);
    
    /****************************
    * 7 Initialize Resources
    * When creating texture resource, need uniform information, so must read uniforms before read resources
    ****************************/
    if( appInfo->Font.font_size > 0){
        //std::cout<<"textManager"<<std::endl;
        textManager.SetFontSize(appInfo->Font.font_size);
        textManager.SetSamplerID(appInfo->Font.font_samplerid);
        textManager.SetOutlineColor(glm::vec4(appInfo->Font.font_outlineColor[0], appInfo->Font.font_outlineColor[1], appInfo->Font.font_outlineColor[2], appInfo->Font.font_outlineColor[3]));
        textManager.SetTextColor(glm::vec4(appInfo->Font.font_textColor[0], appInfo->Font.font_textColor[1], appInfo->Font.font_textColor[2], appInfo->Font.font_textColor[3]));
        //textManager.p_renderer = &renderer;
        textManager.renderer = renderer;
        textManager.resourcer = resourcer;
        //textManager.p_textImageManager = &textImageManager;
        //textManager.p_modelManager = &modelManager;
        
        textManager.CreateTextImage(); //create text atlas image and push to textImageManager
        textManager.CreateGlyphMap(); //create glyph map
        textManager.CreateTextResource(); //loop every textbox[i], create instance data, and create model based on instance data
    }

    if(appInfo->Models.size() > 0){
        for(int i = 0; i < appInfo->Models.size(); i++){
            std::string modelName = appInfo->Models[i].model_names;
            //std::cout<<"test:"<<i<<", modelName="<<modelName<<std::endl;
            if(modelName == "CUSTOM3D0"){
                renderer->CreateVertexBuffer(resourcer->GetModelCustomModel3DData(0), sizeof(Vertex3D), resourcer->GetModelCustomModel3DSize(0));
                renderer->CreateIndexBuffer(resourcer->GetModelCustomModel3DIndices(0));
                
                resourcer->GetModelLengths().push_back(resourcer->GetModelCustomModel3DLength(0)[0]);
                resourcer->GetModelLengthsMin().push_back(resourcer->GetModelCustomModel3DLength(0)[1]);
                resourcer->GetModelLengthsMax().push_back(resourcer->GetModelCustomModel3DLength(0)[2]);

            // }else if(name == "CUSTOM3D1"){
            //     renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[1].vertices);
            //     renderer.CreateIndexBuffer(modelManager.customModels3D[1].indices);

            //     modelManager.modelLengths.push_back(modelManager.customModels3D[1].length);
            //     modelManager.modelLengthsMin.push_back(modelManager.customModels3D[1].lengthMin);
            //     modelManager.modelLengthsMax.push_back(modelManager.customModels3D[1].lengthMax);
            }else if(modelName == "TEXTBOXIMAGE"){
                renderer->CreateVertexBuffer(resourcer->GetModelTextboxImageModelData(0), sizeof(Vertex3D), resourcer->GetModelTextboxImageModelSize(0));
                renderer->CreateIndexBuffer(resourcer->GetModelTextboxImageModelIndices(0));
                
                //modelManager.modelLengths.push_back(modelManager.textboxImageModels[0].length);
                //modelManager.modelLengthsMin.push_back(modelManager.textboxImageModels[0].lengthMin);
                //modelManager.modelLengthsMax.push_back(modelManager.textboxImageModels[0].lengthMax);
                resourcer->GetModelLengths().push_back(resourcer->GetModelTextboxImageModelLength(0)[0]);
                resourcer->GetModelLengthsMin().push_back(resourcer->GetModelTextboxImageModelLength(0)[1]);
                resourcer->GetModelLengthsMax().push_back(resourcer->GetModelTextboxImageModelLength(0)[2]);

            }else if(modelName == "TEXTQUAD"){ //TODO: vertexBuffer and indexBuffer has the same index# of CUSTOM3D#, but instance buffer is 0
                //appInfo.VertexBufferType = VertexStructureTypes::TextQuad;
                //std::cout<<"Application: Load "<<std::endl;
                renderer->CreateVertexBuffer(resourcer->GetModelTextQuadModelData(0), sizeof(TextQuadVertex), resourcer->GetModelTextQuadModelSize(0));
                //renderer.CreateInstanceBuffer(modelManager.textModels[0].instanceData);
                renderer->CreateIndexBuffer(resourcer->GetModelTextQuadModelIndices(0));

                //std::cout<<"Application: Created VertexBuffer, size = "<<renderer.vertexDataBuffers.size()<<std::endl;
                //std::cout<<"Application: Created InstanceBuffer, size = "<<renderer.instanceDataBuffers.size()<<std::endl;
                //std::cout<<"Application: Created IndexBuffer, size = "<<renderer.indexDataBuffers.size()<<std::endl;

                glm::vec3 v(1,1,1); //text quad length is not important, only placeholder
                //modelManager.modelLengths.push_back(v);
                //modelManager.modelLengthsMin.push_back(v);
                //modelManager.modelLengthsMax.push_back(v);
                resourcer->GetModelLengths().push_back(v);
                resourcer->GetModelLengthsMin().push_back(v);
                resourcer->GetModelLengthsMax().push_back(v);
            }else if(modelName == "CUSTOM2D0"){
                //appInfo.VertexBufferType = VertexStructureTypes::TwoDimension;
                renderer->CreateVertexBuffer(resourcer->GetModelCustomModel2DData(0), sizeof(Vertex2D), resourcer->GetModelCustomModel2DSize(0)); 

                //modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                //modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                //modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
                resourcer->GetModelLengths().push_back(resourcer->GetModelCustomModel2DLength(0)[0]);
                resourcer->GetModelLengthsMin().push_back(resourcer->GetModelCustomModel2DLength(0)[1]);
                resourcer->GetModelLengthsMax().push_back(resourcer->GetModelCustomModel2DLength(0)[2]);
            }else{
                //appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                std::vector<Vertex3D> modelVertices3D;
                std::vector<uint32_t> modelIndices3D;
                resourcer->LoadModelObj(modelName, modelVertices3D, modelIndices3D);
                renderer->CreateVertexBuffer(modelVertices3D.data(), sizeof(Vertex3D), modelVertices3D.size()); 
                renderer->CreateIndexBuffer(modelIndices3D);
            }
            //std::cout<<"test end"<<std::endl;
        }

    }

    if(appInfo->Textures.size() > 0){
        for(int i = 0; i < appInfo->Textures.size(); i++){
            //std::cout<<"test Textures:"<<i<<std::endl;
            std::string textureName = appInfo->Textures[i].texture_name;
            int textureMipLevel = appInfo->Textures[i].texture_miplevel;
            bool textureEnableCubemap = appInfo->Textures[i].texture_enableCubemap;
            int textureSamplerId = appInfo->Textures[i].texture_samplerid;
            VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            if(textureMipLevel > 1) //mipmap
                usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            else 
                if(renderer->GetComputeUniformTypes() & COMPUTE_STORAGEIMAGE_TEXTURE) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            //std::cout<<"appInfo->Feature.b_feature_graphics_48pbt="<<appInfo->Feature.b_feature_graphics_48pbt<<std::endl;
            if(!appInfo->Feature.b_feature_graphics_48pbt){ //24bpt
                //std::cout<<"textureSamplerId = "<<textureSamplerId<<std::endl;
                if(renderer->GetComputeUniformTypes() & COMPUTE_STORAGEIMAGE_SWAPCHAIN) resourcer->CreateTextureImage(textureName, usage, renderer->GetCommandPool(), textureMipLevel, textureSamplerId, renderer->GetSwapchainImageFormat());
                else resourcer->CreateTextureImage(textureName, usage, renderer->GetCommandPool(), textureMipLevel, textureSamplerId, VK_FORMAT_R8G8B8A8_SRGB, 8, textureEnableCubemap);  
            }else{ //48bpt
                //textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, enableCubemap); 
                resourcer->CreateTextureImage(textureName, usage, renderer->GetCommandPool(), textureMipLevel, textureSamplerId, VK_FORMAT_R16G16B16A16_SFLOAT, 16, textureEnableCubemap); 
            }
            if(appInfo->Feature.b_feature_graphics_rainbow_mipmap){
                VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                //if(textureMipLevel > 1) resourcer->GetTextureImage(resourcer->GetTextureImageSize()-1).generateMipmaps("checkerboard", usage_mipmap);
                if(textureMipLevel > 1) resourcer->GenerateMipmaps(resourcer->GetTextureImageSize()-1, "checkerboard", usage_mipmap);
            }else 
                //if(textureMipLevel > 1) resourcer->GetTextureImage(resourcer->GetTextureImageSize()-1).generateMipmaps();
                if(textureMipLevel > 1) resourcer->GenerateMipmaps(resourcer->GetTextureImageSize()-1);

        }
    }


    TimePoint T6 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for resources", T5, T6);

    /****************************
    * 8 Create Uniform Descriptors
    ****************************/
    bool b_uniform_graphics = appInfo->Uniform.b_uniform_graphics_custom || appInfo->Uniform.b_uniform_graphics_object_mvp || appInfo->Uniform.b_uniform_graphics_text_mvp || appInfo->Uniform.b_uniform_graphics_object_vp;
    bool b_uniform_compute = appInfo->Uniform.b_uniform_compute_custom || appInfo->Uniform.b_uniform_compute_storage || appInfo->Uniform.b_uniform_compute_swapchain_storage || appInfo->Uniform.b_uniform_compute_texture_storage;
    //UNIFORM STEP 1/3 (Pool)
    renderer->createGraphicsDescriptorPool(objects.size()+textManager.m_textBoxes.size());//need size of both objects and textboxes, because each need a sampler
    renderer->createComputeDescriptorPool();

    //UNIFORM STEP 2/3 (Layer)
    if(b_uniform_graphics){
        if(appInfo->Uniform.b_uniform_graphics_custom){
            renderer->createGraphicsDescriptorSetLayout_General(&appInfo->Uniform.GraphicsCustom.Binding);
        }
        else {
            renderer->createGraphicsDescriptorSetLayout_General();
        }
        if(renderer->GetTextureImageSamplersSize() > 0) renderer->createGraphicsDescriptorSetLayout_TextureImageSampler(); 
    }
    if(b_uniform_compute){
        if(appInfo->Uniform.b_uniform_compute_custom) {
            renderer->createComputeDescriptorSetLayout(&appInfo->Uniform.ComputeCustom.Binding);
        }else {
            renderer->createComputeDescriptorSetLayout();
        }
    }

    //UNIFORM STEP 3/3 (Set)
    std::vector<VkImageView> depthlight_imageviews;
    for(int i = 0; i < renderer->GetSwapchain_BufferSize_Depthlight(); i++) depthlight_imageviews.push_back(renderer->GetSwapchain_Buffer_DepthLight_View(i));
    if(b_uniform_graphics) renderer->createGraphicsDescriptorSets_General(renderer->GetSwapchain_Buffer_DepthCamera_View(), depthlight_imageviews);
    if(b_uniform_compute){
        if(appInfo->Uniform.b_uniform_compute_swapchain_storage) {
            if(appInfo->Uniform.b_uniform_compute_texture_storage)
                renderer->createComputeDescriptorSets(resourcer->GetTextureImageView(0), &(renderer->GetSwapchain_Views()));//this must be called after texture resource is loaded
            else renderer->createComputeDescriptorSets(NULL, &(renderer->GetSwapchain_Views()));
        }else renderer->createComputeDescriptorSets();
    }

    TimePoint T7 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating uniform descriptors", T6, T7);

    /****************************
    * 9 Create Pipelines
    ****************************/
    bool bPipelineVerbose = bVerboseInitialization;

    /****************************
    * 9.1 Command Buffer
    ****************************/
    //if(appInfo->VertexShader && appInfo->VertexShader->size() > 0) renderer.CreateGraphicsCommandBuffer();
    if(appInfo->GraphicsPipelines.size() > 0) renderer->CreateGraphicsCommandBuffer();
    //if(appInfo->ComputeShader && appInfo->ComputeShader->size() > 0) renderer.CreateComputeCommandBuffer();
    if(appInfo->ComputePipelines.size() > 0) renderer->CreateComputeCommandBuffer();
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Command Buffer"<<std::endl;
    
    /****************************
    * 9.2 Create Shaders
    ****************************/
    //if(appInfo->VertexShader && appInfo->VertexShader->size() > 0){
    if(appInfo->GraphicsPipelines.size() > 0){
        for(int i = 0; i < appInfo->GraphicsPipelines.size(); i++){
            //std::cout<<appInfo->GraphicsPipeline[i].graphics_pipeline_vertexshader_name<<std::endl;
            resourcer->CreateShader(appInfo->GraphicsPipelines[i].graphics_pipeline_vertexshader_name, VERT);
            resourcer->CreateShader(appInfo->GraphicsPipelines[i].graphics_pipeline_fragmentshader_name, FRAG);
        }
    }
    if(appInfo->ComputePipelines.size() > 0)
        for(int i = 0; i < appInfo->ComputePipelines.size(); i++)
            resourcer->CreateShader(appInfo->ComputePipelines[i].compute_pipeline_computeshader_name, COMP);
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Shaders"<<std::endl;

    /****************************
    * 9.3 Create Pipelines
    ****************************/
    if(appInfo->GraphicsPipelines.size() > 0){
        std::vector<VkDescriptorSetLayout> dsLayouts; //2 sets for graphics

        int type = renderer->GetGraphicsUniformTypes();

        if((type & GRAPHCIS_UNIFORMBUFFER_CUSTOM) || 
            (type & GRAPHCIS_UNIFORMBUFFER_LIGHTING) || 
            (type & GRAPHCIS_UNIFORMBUFFER_MVP) ||
            (type & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP) ||
            (type & GRAPHCIS_UNIFORMBUFFER_GLOBAL) ||
            (type & GRAPHCIS_UNIFORMBUFFER_VP)){
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Add layout set0: graphics general layout"<<std::endl;
            dsLayouts.push_back(renderer->GetDescriptorSetLayout_General()); //set = 0
        }

        if(type & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE) {
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Add layout set1: sampler(texture) layout"<<std::endl;
            dsLayouts.push_back(renderer->GetDescriptorSetLayout_TextureImageSampler()); //set = 1
        }
  
        //Different cube can share the same texture descriptor.
        //suppose we have 100 objects, 100 different textures. cube x 50, sphere x 50. How many texture layouts? How many texture descriptor?
        //obviously, every objects need a different texture, so bind with objectId
        //but for layout, can use one. That means texture layout should be object property, while the descriptor set(associate with image) should be cube[i]/sphere[i] bound

        //each object can have muti texture image, multi descriptor set(when creating descritpor set, need a sampler)
        //all objects share the same descriptor pool and descriptor layout, they are universal
        //sampler should also be universal
        
        for(int i = 0; i < appInfo->GraphicsPipelines.size(); i++){
            //std::cout<<"test create pipeline"<<std::endl;
            //! All graphics pipelines use the same dsLayouts
            if(resourcer->GetShaderEnablePushConstant()){
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create Push Constant Layout"<<std::endl;
                renderer->CreateGraphicsPipelineLayout(dsLayouts, resourcer->GetShaderPushConstantRange(), true, i);
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Push Constant Layout"<<std::endl;
            }
            else renderer->CreateGraphicsPipelineLayout(dsLayouts, i);

            
            //int vertexDatatype = appInfo->VertexDatatype ? (*appInfo->VertexDatatype)[i] : 0;
            int vertexDatatype = appInfo->GraphicsPipelines[i].graphics_pipeline_vertexdatatype;
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create graphics pipeline: "<<i<<", VertexStructureType="<<vertexDatatype<<std::endl;

            switch(vertexDatatype){
                case VertexStructureTypes::NoType:
                    renderer->CreateGraphicsPipeline(NULL, NULL,
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), false, false, 
                        renderer->GetRenderpass_mainscene(), i, appInfo);
                break;
                case VertexStructureTypes::ThreeDimension:
                    //for 2-renderpass case, each pipeline for different renderpass
                    //if((*appInfo->RenderPassShadowmap)[i]) {
                    if(appInfo->GraphicsPipelines[i].graphics_pipeline_renderpasses_shadowmap) {
                        renderer->CreateGraphicsPipeline(Vertex3D::getBindingDescription, Vertex3D::getAttributeDescriptions, 
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), true, false, 
                            renderer->GetRenderpass_shadowmap(), i, appInfo); 
                    }else{
                        renderer->CreateGraphicsPipeline(Vertex3D::getBindingDescription, Vertex3D::getAttributeDescriptions, 
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), true, false, 
                            renderer->GetRenderpass_mainscene(), i, appInfo);
                    }   
                break;
                case VertexStructureTypes::TwoDimension:
                    //std::cout<<"CreatePipeline: Create 2D pipeline"<<std::endl;
                    renderer->CreateGraphicsPipeline(Vertex2D::getBindingDescription, Vertex2D::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), true, false, 
                        renderer->GetRenderpass_mainscene(), i, appInfo);
                break;
                case VertexStructureTypes::ParticleType:
                    renderer->CreateGraphicsPipeline(Particle::getBindingDescription, Particle::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), true, false, 
                        renderer->GetRenderpass_mainscene(), i, appInfo);
                break;
                case VertexStructureTypes::TextQuad:
                    renderer->CreateGraphicsPipeline(NULL, NULL, //TextQuadVertex::getBindingDescription, TextQuadVertex::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, resourcer->GetVertexShaderModule(i), resourcer->GetFragmentShaderModule(i), true, true, 
                        renderer->GetRenderpass_mainscene(), i, appInfo);
                break;
                default:
                break;
            }
            if(bPipelineVerbose) std::cout<<"Done create one graphics pipeline"<<std::endl;
        }
        
    }
    if(appInfo->ComputePipelines.size() > 0){ //for now assume only one compute pipeline
        //! only support one compute pipeline
        renderer->CreateComputePipelineLayout(renderer->GetComputeDescriptorSetLayout());
        renderer->CreateComputePipeline(resourcer->GetComputeShaderModule(0));
    }
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Pipelines"<<std::endl;

    TimePoint T8 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating pipelines", T7, T8);

    /****************************
    * 10 Register Objects
    ****************************/
    for(int i = 0; i < appInfo->Objects.size(); i++){
        objects[i].m_object_id = appInfo->Objects[i].object_id;
        objects[i].m_model_id = appInfo->Objects[i].object_resource_model_id;
        objects[i].m_texture_ids = appInfo->Objects[i].object_resource_texture_id_list;
        objects[i].m_default_graphics_pipeline_id = appInfo->Objects[i].object_resource_default_graphics_pipeline_id;
        objects[i].Name = appInfo->Objects[i].object_name;
        objects[i].bSticker = appInfo->Objects[i].object_bSticker;
        objects[i].SetPosition(appInfo->Objects[i].object_position[0], appInfo->Objects[i].object_position[1], appInfo->Objects[i].object_position[2]);
        objects[i].SetRotation(appInfo->Objects[i].object_rotation[0], appInfo->Objects[i].object_rotation[1], appInfo->Objects[i].object_rotation[2]);
        objects[i].SetVelocity(appInfo->Objects[i].object_velocity[0], appInfo->Objects[i].object_velocity[1], appInfo->Objects[i].object_velocity[2]);
        objects[i].SetAngularVelocity(appInfo->Objects[i].object_angular_velocity[0], appInfo->Objects[i].object_angular_velocity[1], appInfo->Objects[i].object_angular_velocity[2]);

        //must load resources before object register
        if(objects[i].bRegistered) {
            std::cout<<"WARNING: Trying to register a registered Object id("<<i<<")!"<<std::endl;
            continue;
        }
        objects[i].Register((GameEngine*)this);
        if(appInfo->Objects[i].object_scale != 1.0f){
            objects[i].SetScale(appInfo->Objects[i].object_scale, appInfo->Objects[i].object_scale, appInfo->Objects[i].object_scale);
        }else{
            auto object_scale_3 = appInfo->Objects[i].object_scale_3;
            objects[i].SetScale(object_scale_3[0], object_scale_3[1], object_scale_3[2]);//set scale after model is registered, otherwise the length will not be computed correctly
        }
    }

    //register objects for controls
    if(appInfo->Feature.feature_graphics_enable_controls){
        int indexOffset = appInfo->Objects.size();
        for(int i = 0; i < controlNodes.size(); i++){
            controlNodes[i]->RegisterObject(indexOffset);
            indexOffset += controlNodes[i]->m_object_count;
        }
    }

    for(int i = 0; i < objects.size(); i++){
        if(!objects[i].bRegistered) std::cout<<"WARNING: Object id("<<i<<") is not registered!"<<std::endl;
        logger->Log("Object ID: {}", i);
        logger->Log("\tName: {}", objects[i].Name.c_str());
        logger->LogVec3("\tPosition", objects[i].Position);
        logger->LogVec3("\tLength_original", objects[i].Length_original);
        logger->LogVec3("\tLengthMin_original", objects[i].LengthMin_original);
        logger->LogVec3("\tLengthMax_original", objects[i].LengthMax_original);
        logger->LogVec3("\tScale", objects[i].Scale);
        logger->LogVec3("\tLength", objects[i].Length);
        logger->Log("");
    }

    TimePoint T9 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for register objects", T8, T9);

    /****************************
    * 11 Register Textboxes
    ****************************/
    for(int i = 0; i < appInfo->Textboxes.size(); i++){
        textManager.m_textBoxes[i].Name = appInfo->Textboxes[i].textbox_name;
        textManager.m_textBoxes[i].m_textBoxID = appInfo->Textboxes[i].textbox_id;
        textManager.m_textBoxes[i].SetPosition(appInfo->Textboxes[i].textbox_position[0], appInfo->Textboxes[i].textbox_position[1], appInfo->Textboxes[i].textbox_position[2]);
        textManager.m_textBoxes[i].SetRotation(appInfo->Textboxes[i].textbox_rotation[0], appInfo->Textboxes[i].textbox_rotation[1], appInfo->Textboxes[i].textbox_rotation[2]);
        textManager.m_textBoxes[i].bSticker = appInfo->Textboxes[i].textbox_bSticker;
        textManager.m_textBoxes[i].SetScale(appInfo->Textboxes[i].textbox_scale);
        textManager.m_textBoxes[i].SetBoxColor(glm::vec4(appInfo->Textboxes[i].textbox_color[0], appInfo->Textboxes[i].textbox_color[1], appInfo->Textboxes[i].textbox_color[2], appInfo->Textboxes[i].textbox_color[3]));
        textManager.m_textBoxes[i].m_model_id = appInfo->Textboxes[i].textbox_resource_model_id;
        textManager.m_textBoxes[i].m_text_content = appInfo->Textboxes[i].textbox_text_content;
        textManager.m_textBoxes[i].SetTextColor(glm::vec4(appInfo->Textboxes[i].textbox_text_color[0], appInfo->Textboxes[i].textbox_text_color[1], appInfo->Textboxes[i].textbox_text_color[2], appInfo->Textboxes[i].textbox_text_color[3]));
        textManager.m_textBoxes[i].m_default_graphics_pipeline_id = appInfo->Textboxes[i].textbox_resource_default_graphics_pipeline_id;

        if(textManager.m_textBoxes[i].bRegistered) {
            std::cout<<"WARNING: Trying to register a registered Textbox id("<<i<<")!"<<std::endl;
            continue;
        }
        textManager.m_textBoxes[i].Register((GameEngine*)this);
    }

    //register textbox for controls
    if(appInfo->Feature.feature_graphics_enable_controls){
        int indexOffset = appInfo->Textboxes.size();
        for(int i = 0; i < controlNodes.size(); i++){
            controlNodes[i]->RegisterTextbox(indexOffset);
            indexOffset += controlNodes[i]->m_textbox_count;
        }
    }

    for(int i = 0; i < textManager.m_textBoxes.size(); i++){
        if(!textManager.m_textBoxes[i].bRegistered) std::cout<<"WARNING: Textbox id("<<i<<") is not registered!"<<std::endl;
        logger->Log("Textbox ID: {}", i);
        logger->Log("\tName: {}", textManager.m_textBoxes[i].Name.c_str());
        logger->LogVec3("\tPosition", textManager.m_textBoxes[i].Position);
        logger->LogVec3("\tLength_original", textManager.m_textBoxes[i].Length_original);
        logger->LogVec3("\tLengthMin_original", textManager.m_textBoxes[i].LengthMin_original);
        logger->LogVec3("\tLengthMax_original", textManager.m_textBoxes[i].LengthMax_original);
        logger->LogVec3("\tScale", textManager.m_textBoxes[i].Scale);
        logger->LogVec3("\tLength", textManager.m_textBoxes[i].Length);
        logger->Log("");
    }

    TimePoint T10 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for register textboxes", T9, T10);

    /****************************
    * 12 Register Lightings
    ****************************/
    for(int i = 0; i < appInfo->Lights.size(); i++){
        int light_id = appInfo->Lights[i].light_id;
        if(lights[light_id].bRegistered) {
            std::cout<<"WARNING: Trying to register a registered Light id("<<light_id<<")!"<<std::endl;
            continue;
        }
        
        std::string name = appInfo->Lights[i].light_name;
        auto position = appInfo->Lights[i].light_position;
        glm::vec3 glm_position(position[0], position[1], position[2]);
        auto intensity = appInfo->Lights[i].light_intensity;
        auto color = appInfo->Lights[i].light_color;
        glm::vec3 glm_color(color[0], color[1], color[2]);
        auto spotAngle = appInfo->Lights[i].light_spotAngle; //the default value is [180,180] degrees which sets the light to point light instead of spot light
        float spotInnerAngle = spotAngle[0];
        float spotOuterAngle = spotAngle[1];

        lights[light_id].Register(name, light_id, glm_position, intensity, glm_color, spotInnerAngle, spotOuterAngle);
    }

    for(int i = 0; i < lights.size(); i++) if(!lights[i].bRegistered) std::cout<<"WARNING: Light id("<<i<<") is not registered!"<<std::endl;

    TimePoint T11 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for register lightings", T10, T11);
    
    /****************************
    * 13 Set Main Camera
    ****************************/
    mainCamera.cameraType = (CameraType)appInfo->MainCamera.camera_mode;
    mainCamera.SetPosition(appInfo->MainCamera.camera_position[0], appInfo->MainCamera.camera_position[1],  appInfo->MainCamera.camera_position[2]);
    mainCamera.SetRotation(appInfo->MainCamera.camera_rotation[0], appInfo->MainCamera.camera_rotation[1],  appInfo->MainCamera.camera_rotation[2]);
    mainCamera.focusObjectId = appInfo->MainCamera.object_id_target;
    mainCamera.bEnableOrthographic = appInfo->MainCamera.camera_projection_enable_orthographic;
    float nearPlane = appInfo->MainCamera.camera_z[0];
    float farPlane = appInfo->MainCamera.camera_z[1];
    if(!mainCamera.bEnableOrthographic){ mainCamera.setPerspective(appInfo->MainCamera.camera_projection_perspective_fov, 1.0f, nearPlane, farPlane);
    }else{
        float orthoWidth = appInfo->MainCamera.camera_projection_orthographic_width;
        float orthoHeight = appInfo->MainCamera.camera_projection_orthographic_height;
        mainCamera.setOrthographic(
            -orthoWidth / 2.0f, orthoWidth / 2.0f,
            -orthoHeight / 2.0f, orthoHeight / 2.0f,
            nearPlane, farPlane);
    }
    mainCamera.SetRotationSensitivity(200.0f);

    sdler->SetKeyboardSensibility(appInfo->MainCamera.camera_keyboard_sensitive);
    sdler->SetMouseSensibility(appInfo->MainCamera.camera_mouse_sensitive);

    lightCameras[0].cameraType = (CameraType)appInfo->LightCamera.camera_mode;
    lightCameras[0].SetPosition(appInfo->LightCamera.camera_position[0], appInfo->LightCamera.camera_position[1],  appInfo->LightCamera.camera_position[2]);
    lightCameras[0].SetRotation(appInfo->LightCamera.camera_rotation[0], appInfo->LightCamera.camera_rotation[1],  appInfo->LightCamera.camera_rotation[2]);
    lightCameras[0].focusObjectId = appInfo->LightCamera.object_id_target;
    lightCameras[0].bEnableOrthographic = appInfo->LightCamera.camera_projection_enable_orthographic;
    nearPlane = appInfo->LightCamera.camera_z[0];
    farPlane = appInfo->LightCamera.camera_z[1];
    if(!lightCameras[0].bEnableOrthographic){ lightCameras[0].setPerspective(appInfo->LightCamera.camera_projection_perspective_fov, 1.0f, nearPlane, farPlane);
    }else{
        float orthoWidth = appInfo->LightCamera.camera_projection_orthographic_width;
        float orthoHeight = appInfo->LightCamera.camera_projection_orthographic_height;
        lightCameras[0].setOrthographic(
            -orthoWidth / 2.0f, orthoWidth / 2.0f,
            -orthoHeight / 2.0f, orthoHeight / 2.0f,
            nearPlane, farPlane);
    }
    //lightCameras[0].SetRotationSensitivity(100.0f);

    for(int i = 1; i < lights.size(); i++){//lightCameras.size()
        lightCameras[i].cameraType = lightCameras[0].cameraType; //default to light camera type
        lightCameras[i].SetPosition(lightCameras[0].Position);
        lightCameras[i].SetRotation(lightCameras[0].Rotation);
        lightCameras[i].setPerspective(lightCameras[0].fov,  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, lightCameras[0].znear, lightCameras[0].zfar);
        lightCameras[i].setOrthographic(-20, 20, -20, 20, lightCameras[0].znear, lightCameras[0].zfar);
        lightCameras[i].focusObjectId = lightCameras[0].focusObjectId; //default to main camera focus object id
        lightCameras[i].bEnableOrthographic = lightCameras[0].bEnableOrthographic; //default to main camera orthographic mode
        //lightCameras[i].SetRotationSensitivity(100.0f);
    }

    TimePoint T12 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for set cameras", T11, T12);

    /****************************
    * 14 Create Sync Objects and Clean up Shaders (+and call example initialization)
    ****************************/
    renderer->CreateSyncObjects(renderer->GetSwapchain_ImageSize());
    resourcer->DestroyShaderManager();

    TimePoint T13 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating sync objects and destroy shaders", T12, T13);
}

}