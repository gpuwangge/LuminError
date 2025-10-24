#include "../include/application.h"

namespace LEApplication{

void Application::Initialize(){
    instance_game->Initialize();

    bool bVerboseInitialization = false;
    TimePoint T0 = now();

    instance_yamlcore->ReadYAMLFile(m_sampleName);

    /****************************
    * 1 Process blend and push constant
    ****************************/   
    renderer.m_renderMode = (RenderModes)appInfo->RenderMode;
    if(appInfo->Feature.b_feature_graphics_push_constant)
        shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);
    if(appInfo->Feature.b_feature_graphics_global_blend)
        renderProcess.addColorBlendAttachment(
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
    swapchain.buffer_depthlight.resize(lights.size());
    swapchain.framebuffers_shadowmap.resize(lights.size());
    std::cout<<"Light Size: "<<lights.size()<<std::endl;

    CGraphicsDescriptorManager::m_lightingUBO.lightNum = lights.size(); //update light number to ubo

    TimePoint T2 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for object/textbox/light", T1, T2);

    /****************************
    * 4 Initialize Uniforms
    ****************************/
    if(appInfo->Uniform.b_uniform_graphics_custom) CGraphicsDescriptorManager::addCustomUniformBuffer(appInfo->Uniform.GraphicsCustom.Size);
    if(appInfo->Uniform.b_uniform_graphics_lighting) CGraphicsDescriptorManager::addLightingUniformBuffer();
    if(appInfo->Uniform.b_uniform_graphics_object_mvp){
        CGraphicsDescriptorManager::addMVPUniformBuffer();
        renderer.bUseObjectMVP = true;
    }
    if(appInfo->Uniform.b_uniform_graphics_text_mvp){
        CGraphicsDescriptorManager::addTextMVPUniformBuffer();
        renderer.bUseTextboxMVP = true;
    }   
    if(appInfo->Uniform.b_uniform_graphics_object_vp){
        CGraphicsDescriptorManager::addVPUniformBuffer();
        renderer.bUseObjectMVP = true; //reuse MVP bool
    }
    if(appInfo->Uniform.b_uniform_graphics_depth_image_sampler) CGraphicsDescriptorManager::addDepthImageSamplerUniformBuffer();
    if(appInfo->Uniform.b_uniform_graphics_lightdepth_image_sampler) CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer();
    if(appInfo->Uniform.b_uniform_graphics_lightdepth_image_sampler_hardware) CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();
    if(appInfo->Uniform.b_uniform_compute_custom) CComputeDescriptorManager::addCustomUniformBuffer(appInfo->Uniform.ComputeCustom.Size);
    if(appInfo->Uniform.b_uniform_compute_storage) CComputeDescriptorManager::addStorageBuffer(appInfo->Uniform.ComputeStorageBuffer.Size, appInfo->Uniform.ComputeStorageBuffer.Usage);
    if(appInfo->Uniform.b_uniform_compute_texture_storage) CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_TEXTURE);
    if(appInfo->Uniform.b_uniform_compute_swapchain_storage) CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_SWAPCHAIN);

    if(appInfo->Samplers.size() > 0){
        CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE;
        std::vector<int> mipLevels;
        std::vector<std::array<bool,3>> UVWRepeats;
        for(int i = 0; i < appInfo->Samplers.size(); i++){
            mipLevels.push_back(appInfo->Samplers[i].sampler_miplevels);
            UVWRepeats.push_back(appInfo->Samplers[i].sampler_uvwRepeats);
        }
        CGraphicsDescriptorManager::addTextureImageSamplerUniformBuffer(mipLevels, UVWRepeats);
    }

    TimePoint T3 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for uniforms", T2, T3);

    /****************************
    * 5 Initialize attachments
    ****************************/
    renderProcess.iShadowmapAttachmentDepthLight = appInfo->Attachment.bShadowmapAttachmentDepthLight ? 0 : -1; //shadowmap renderpass attachment depth light, only one attachment, so id is 0

    int AttachmentCount = 0;
    renderProcess.iMainSceneAttachmentDepthLight = appInfo->Attachment.bMainSceneAttachmentDepthLight ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentDepthCamera = appInfo->Attachment.bMainSceneAttachmentDepthCamera ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorResovle = appInfo->Attachment.bMainSceneAttachmentColorResovle ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorPresent = appInfo->Attachment.bMainSceneAttachmentColorPresent ? AttachmentCount++ : -1;

    swapchain.iShadowmapAttachmentDepthLight = renderProcess.iShadowmapAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthLight = renderProcess.iMainSceneAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthCamera = renderProcess.iMainSceneAttachmentDepthCamera;
    swapchain.iMainSceneAttachmentColorResovle = renderProcess.iMainSceneAttachmentColorResovle;
    swapchain.iMainSceneAttachmentColorPresent = renderProcess.iMainSceneAttachmentColorPresent;

    //when creating attachment resource, need 1.create attachment description in renderProcess; 2.create attachment buffer in swapchain
    if(swapchain.iMainSceneAttachmentColorResovle >= 0) swapchain.GetMaxUsableSampleCount(); //calcuate max sampler count first

    if(swapchain.iShadowmapAttachmentDepthLight >= 0){ //if shadowmap renderpass attachment depth light is enabled
        swapchain.create_attachment_resource_depthlight(VK_SAMPLE_COUNT_1_BIT); //hardware bias todo
        renderProcess.create_attachmentdescription_shadowmap_depthlight(swapchain.depthFormat); 
    }else if(swapchain.iMainSceneAttachmentDepthLight >= 0){
        swapchain.create_attachment_resource_depthlight(swapchain.msaaSamples);
        renderProcess.create_attachmentdescription_mainscene_depthlight(swapchain.depthFormat, swapchain.msaaSamples);
    }
    if(swapchain.iMainSceneAttachmentDepthCamera >= 0){//If enable MSAA, must also enable Depth Test
        swapchain.create_attachment_resource_depthcamera();
        renderProcess.create_attachmentdescription_mainscene_depthcamera(swapchain.depthFormat, swapchain.msaaSamples);
    }
    if(swapchain.iMainSceneAttachmentColorResovle >= 0){
        swapchain.create_attachment_resource_colorresolve();
        renderProcess.create_attachmentdescription_mainscene_colorresolve(swapchain.swapChainImageFormat, swapchain.msaaSamples, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
    if(swapchain.iMainSceneAttachmentColorPresent >= 0) //dont need create swapchain attachment resource here
        renderProcess.create_attachmentdescription_mainscene_colorpresent(swapchain.swapChainImageFormat);

    TimePoint T4 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for attachements", T3, T4);
    

    /****************************
    * 6 Initialize Subpasses
    ****************************/
    renderProcess.bEnableShadowmapRenderpassSubpassShadowmap = appInfo->Subpass.bEnableShadowmapRenderpassSubpassShadowmap;
    renderProcess.bEnableMainSceneRenderpassSubpassShadowmap = appInfo->Subpass.bEnableMainSceneRenderpassSubpassShadowmap;
    renderProcess.bEnableMainSceneRenderpassSubpassDraw = appInfo->Subpass.bEnableMainSceneRenderpassSubpassDraw;
    renderProcess.bEnableMainSceneRenderpassSubpassObserve = appInfo->Subpass.bEnableMainSceneRenderpassSubpassObserve;

    //for shadowmap renderpass (this renderpass is optional)
    if(renderProcess.bEnableShadowmapRenderpassSubpassShadowmap){
        // std::cout<<"Application: Create Shadowmap Render Pass."<<std::endl;
        renderProcess.createSubpass_shadowmap();
        renderProcess.createDependency_shadowmap();
        renderProcess.createRenderPass_shadowmap();

        // std::cout<<"Application: Create Shadowmap Framebuffer."<<std::endl;
        for(int i = 0; i < swapchain.framebuffers_shadowmap.size(); i++)
            swapchain.CreateFramebuffer_shadowmap(renderProcess.renderPass_shadowmap, i);
    }

    //for mainscene renderpass (this renderpass is mandatory)
    renderProcess.createSubpass_mainscene(appInfo->Feature.feature_graphics_observe_attachment_id);
    renderProcess.createDependency_mainscene();
    renderProcess.createRenderPass_mainscene();

    //create framebuffer
    swapchain.CreateFramebuffer_mainscene(renderProcess.renderPass_mainscene);

    TimePoint T5 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading subpasses", T4, T5);
    
    /****************************
    * 7 Initialize Resources
    * When creating texture resource, need uniform information, so must read uniforms before read resources
    ****************************/
    if( appInfo->Font.font_size > 0){
        textManager.SetFontSize(appInfo->Font.font_size);
        textManager.SetSamplerID(appInfo->Font.font_samplerid);
        textManager.SetOutlineColor(glm::vec4(appInfo->Font.font_outlineColor[0], appInfo->Font.font_outlineColor[1], appInfo->Font.font_outlineColor[2], appInfo->Font.font_outlineColor[3]));
        textManager.SetTextColor(glm::vec4(appInfo->Font.font_textColor[0], appInfo->Font.font_textColor[1], appInfo->Font.font_textColor[2], appInfo->Font.font_textColor[3]));
        textManager.p_renderer = &renderer;
        textManager.p_textImageManager = &textImageManager;
        textManager.p_modelManager = &modelManager;

        textManager.CreateTextImage(); //create text atlas image and push to textImageManager
        textManager.CreateGlyphMap(); //create glyph map
        textManager.CreateTextResource(); //loop every textbox[i], create instance data, and create model based on instance data
    }

    if(appInfo->Models.size() > 0){
        for(int i = 0; i < appInfo->Models.size(); i++){
            std::string modelName = appInfo->Models[i].model_names;
            if(modelName == "CUSTOM3D0"){
                renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[0].vertices);
                renderer.CreateIndexBuffer(modelManager.customModels3D[0].indices);
                
                modelManager.modelLengths.push_back(modelManager.customModels3D[0].length);
                modelManager.modelLengthsMin.push_back(modelManager.customModels3D[0].lengthMin);
                modelManager.modelLengthsMax.push_back(modelManager.customModels3D[0].lengthMax);
            // }else if(name == "CUSTOM3D1"){
            //     renderer.CreateVertexBuffer<Vertex3D>(modelManager.customModels3D[1].vertices);
            //     renderer.CreateIndexBuffer(modelManager.customModels3D[1].indices);

            //     modelManager.modelLengths.push_back(modelManager.customModels3D[1].length);
            //     modelManager.modelLengthsMin.push_back(modelManager.customModels3D[1].lengthMin);
            //     modelManager.modelLengthsMax.push_back(modelManager.customModels3D[1].lengthMax);
            }else if(modelName == "TEXTBOXIMAGE"){
                renderer.CreateVertexBuffer<Vertex3D>(modelManager.textboxImageModels[0].vertices);
                renderer.CreateIndexBuffer(modelManager.textboxImageModels[0].indices);
                
                modelManager.modelLengths.push_back(modelManager.textboxImageModels[0].length);
                modelManager.modelLengthsMin.push_back(modelManager.textboxImageModels[0].lengthMin);
                modelManager.modelLengthsMax.push_back(modelManager.textboxImageModels[0].lengthMax);
            }else if(modelName == "TEXTQUAD"){ //TODO: vertexBuffer and indexBuffer has the same index# of CUSTOM3D#, but instance buffer is 0
                //appInfo.VertexBufferType = VertexStructureTypes::TextQuad;
                //std::cout<<"Application: Load "<<std::endl;
                renderer.CreateVertexBuffer<TextQuadVertex>(modelManager.textQuadModels[0].vertices);
                //renderer.CreateInstanceBuffer(modelManager.textModels[0].instanceData);
                renderer.CreateIndexBuffer(modelManager.textQuadModels[0].indices);

                //std::cout<<"Application: Created VertexBuffer, size = "<<renderer.vertexDataBuffers.size()<<std::endl;
                //std::cout<<"Application: Created InstanceBuffer, size = "<<renderer.instanceDataBuffers.size()<<std::endl;
                //std::cout<<"Application: Created IndexBuffer, size = "<<renderer.indexDataBuffers.size()<<std::endl;

                glm::vec3 v(1,1,1); //text quad length is not important, only placeholder
                modelManager.modelLengths.push_back(v);
                modelManager.modelLengthsMin.push_back(v);
                modelManager.modelLengthsMax.push_back(v);
            }else if(modelName == "CUSTOM2D0"){
                //appInfo.VertexBufferType = VertexStructureTypes::TwoDimension;
                renderer.CreateVertexBuffer<Vertex2D>(modelManager.customModels2D[0].vertices); 

                modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
            }else{
                //appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                std::vector<Vertex3D> modelVertices3D;
                std::vector<uint32_t> modelIndices3D;
                modelManager.LoadObjModel(modelName, modelVertices3D, modelIndices3D);
                renderer.CreateVertexBuffer<Vertex3D>(modelVertices3D); 
                renderer.CreateIndexBuffer(modelIndices3D);
            }
        }
        
    }

    if(appInfo->Textures.size() > 0){
        for(int i = 0; i < appInfo->Textures.size(); i++){
            std::string textureName = appInfo->Textures[i].texture_name;
            int textureMipLevel = appInfo->Textures[i].texture_miplevel;
            bool textureEnableCubemap = appInfo->Textures[i].texture_enableCubemap;
            int textureSamplerId = appInfo->Textures[i].texture_samplerid;
            
            VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                if(textureMipLevel > 1) //mipmap
                    usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                else 
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                    else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                
                if(!appInfo->Feature.b_feature_graphics_48pbt){ //24bpt
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN) textureManager.CreateTextureImage(textureName, usage, renderer.commandPool, textureMipLevel, textureSamplerId, swapchain.swapChainImageFormat);
                    else textureManager.CreateTextureImage(textureName, usage, renderer.commandPool, textureMipLevel, textureSamplerId, VK_FORMAT_R8G8B8A8_SRGB, 8, textureEnableCubemap);  
                }else{ //48bpt
                    //textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, enableCubemap); 
                    textureManager.CreateTextureImage(textureName, usage, renderer.commandPool, textureMipLevel, textureSamplerId, VK_FORMAT_R16G16B16A16_SFLOAT, 16, textureEnableCubemap); 
                }
                
                if(appInfo->Feature.b_feature_graphics_rainbow_mipmap){
                    VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(textureMipLevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps("checkerboard", usage_mipmap);
                }else if(textureMipLevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps();

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
    CGraphicsDescriptorManager::createDescriptorPool(objects.size()+textManager.m_textBoxes.size());//need size of both objects and textboxes, because each need a sampler
    CComputeDescriptorManager::createDescriptorPool();

    //UNIFORM STEP 2/3 (Layer)
    if(b_uniform_graphics){
        if(appInfo->Uniform.b_uniform_graphics_custom) 
             CGraphicsDescriptorManager::createDescriptorSetLayout_General(&appInfo->Uniform.GraphicsCustom.Binding); 
        else CGraphicsDescriptorManager::createDescriptorSetLayout_General(); 
        if(CGraphicsDescriptorManager::textureImageSamplers.size()>0) CGraphicsDescriptorManager::createDescriptorSetLayout_TextureImageSampler(); 
    }
    if(b_uniform_compute){
        if(appInfo->Uniform.b_uniform_compute_custom) CComputeDescriptorManager::createDescriptorSetLayout(&appInfo->Uniform.ComputeCustom.Binding);
        else CComputeDescriptorManager::createDescriptorSetLayout();
    }

    //UNIFORM STEP 3/3 (Set)
    if(b_uniform_graphics) graphicsDescriptorManager.createDescriptorSets_General(swapchain.buffer_depthcamera.view, swapchain.buffer_depthlight);
    if(b_uniform_compute){
        if(appInfo->Uniform.b_uniform_compute_swapchain_storage) {
            if(appInfo->Uniform.b_uniform_compute_texture_storage)
                computeDescriptorManager.createDescriptorSets(&(textureManager.textureImages), &(swapchain.swapchain_views));//this must be called after texture resource is loaded
            else computeDescriptorManager.createDescriptorSets(NULL, &(swapchain.swapchain_views));
        }else computeDescriptorManager.createDescriptorSets();
    }

    TimePoint T7 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating uniform descriptors", T6, T7);

    /****************************
    * 6 Create Pipelines
    ****************************/
    CreatePipelines();

    TimePoint T8 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating pipelines", T7, T8);

    /****************************
    * 7 Read and Register Objects
    ****************************/
    ReadRegisterObjects();

    TimePoint T9 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading register objects", T8, T9);


    /****************************
    * 8 Read and Register Textboxes
    ****************************/
    ReadRegisterTextboxes();
    

    TimePoint T10 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading register textboxes", T9, T10);

    /****************************
    * 9 Read Lightings
    ****************************/
    ReadLightings();

    TimePoint T11 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading lightings", T10, T11);
    
    /****************************
    * 9 Read Main Camera
    ****************************/
    ReadCameras();

    TimePoint T12 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading cameras", T11, T12);

    /****************************
    * 10 Create Sync Objects and Clean up Shaders (+and call example initialization)
    ****************************/
    renderer.CreateSyncObjects(swapchain.swapchainImageSize);
    shaderManager.Destroy();


    TimePoint T13 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating sync objects and destroy shaders", T12, T13);

    instance_game->PostInitialize();
}


}