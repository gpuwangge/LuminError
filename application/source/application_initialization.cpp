#include "../include/application.h"

namespace LEApplication{

void Application::Initialize(){
    bool bVerboseInitialization = false;
    TimePoint T0 = now();
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
            //std::cout<<"test:"<<i<<", modelName="<<modelName<<std::endl;
            if(modelName == "CUSTOM3D0"){
                renderer.CreateVertexBuffer(modelManager.customModels3D[0].vertices.data(), sizeof(Vertex3D), modelManager.customModels3D[0].vertices.size());
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
                renderer.CreateVertexBuffer(modelManager.textboxImageModels[0].vertices.data(), sizeof(Vertex3D), modelManager.textboxImageModels[0].vertices.size());
                renderer.CreateIndexBuffer(modelManager.textboxImageModels[0].indices);
                
                modelManager.modelLengths.push_back(modelManager.textboxImageModels[0].length);
                modelManager.modelLengthsMin.push_back(modelManager.textboxImageModels[0].lengthMin);
                modelManager.modelLengthsMax.push_back(modelManager.textboxImageModels[0].lengthMax);
            }else if(modelName == "TEXTQUAD"){ //TODO: vertexBuffer and indexBuffer has the same index# of CUSTOM3D#, but instance buffer is 0
                //appInfo.VertexBufferType = VertexStructureTypes::TextQuad;
                //std::cout<<"Application: Load "<<std::endl;
                renderer.CreateVertexBuffer(modelManager.textQuadModels[0].vertices.data(), sizeof(TextQuadVertex), modelManager.textQuadModels[0].vertices.size());
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
                renderer.CreateVertexBuffer(modelManager.customModels2D[0].vertices.data(), sizeof(Vertex2D), modelManager.customModels2D[0].vertices.size()); 

                modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
            }else{
                //appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                std::vector<Vertex3D> modelVertices3D;
                std::vector<uint32_t> modelIndices3D;
                modelManager.LoadObjModel(modelName, modelVertices3D, modelIndices3D);
                renderer.CreateVertexBuffer(modelVertices3D.data(), sizeof(Vertex3D), modelVertices3D.size()); 
                renderer.CreateIndexBuffer(modelIndices3D);
            }
            //std::cout<<"test end"<<std::endl;
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
    * 9 Create Pipelines
    ****************************/
    bool bPipelineVerbose = false;

    /****************************
    * 9.1 Command Buffer
    ****************************/
    //if(appInfo->VertexShader && appInfo->VertexShader->size() > 0) renderer.CreateGraphicsCommandBuffer();
    if(appInfo->GraphicsPipelines.size() > 0) renderer.CreateGraphicsCommandBuffer();
    //if(appInfo->ComputeShader && appInfo->ComputeShader->size() > 0) renderer.CreateComputeCommandBuffer();
    if(appInfo->ComputePipelines.size() > 0) renderer.CreateComputeCommandBuffer();
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Command Buffer"<<std::endl;
    
    /****************************
    * 9.2 Create Shaders
    ****************************/
    //if(appInfo->VertexShader && appInfo->VertexShader->size() > 0){
    if(appInfo->GraphicsPipelines.size() > 0){
        for(int i = 0; i < appInfo->GraphicsPipelines.size(); i++){
            //std::cout<<appInfo->GraphicsPipeline[i].graphics_pipeline_vertexshader_name<<std::endl;
            shaderManager.CreateShader(appInfo->GraphicsPipelines[i].graphics_pipeline_vertexshader_name, shaderManager.VERT);
            shaderManager.CreateShader(appInfo->GraphicsPipelines[i].graphics_pipeline_fragmentshader_name, shaderManager.FRAG);
        }
    }
    if(appInfo->ComputePipelines.size() > 0)
        for(int i = 0; i < appInfo->ComputePipelines.size(); i++)
            shaderManager.CreateShader(appInfo->ComputePipelines[i].compute_pipeline_computeshader_name, shaderManager.COMP);
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Shaders"<<std::endl;

    /****************************
    * 9.3 Create Pipelines
    ****************************/
    if(appInfo->GraphicsPipelines.size() > 0){
        std::vector<VkDescriptorSetLayout> dsLayouts; //2 sets for graphics

        if((CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) || 
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) || 
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP) ||
            (CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP)){
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Add layout set0: graphics general layout"<<std::endl;
            dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout_general); //set = 0
        }

        if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE) {
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Add layout set1: sampler(texture) layout"<<std::endl;
            dsLayouts.push_back(CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler); //set = 1
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
            if(shaderManager.bEnablePushConstant){
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create Push Constant Layout"<<std::endl;
                renderProcess.createGraphicsPipelineLayout(dsLayouts,  shaderManager.pushConstantRange, true, i);
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Push Constant Layout"<<std::endl;
            }
            else renderProcess.createGraphicsPipelineLayout(dsLayouts, i);

            
            //int vertexDatatype = appInfo->VertexDatatype ? (*appInfo->VertexDatatype)[i] : 0;
            int vertexDatatype = appInfo->GraphicsPipelines[i].graphics_pipeline_vertexdatatype;
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create graphics pipeline: "<<i<<", VertexStructureType="<<vertexDatatype<<std::endl;

            switch(vertexDatatype){
                case VertexStructureTypes::NoType:
                    renderProcess.createGraphicsPipeline(NULL, NULL,
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], false, false, 
                        renderProcess.renderPass_mainscene, i, appInfo);
                break;
                case VertexStructureTypes::ThreeDimension:
                    //for 2-renderpass case, each pipeline for different renderpass
                    //if((*appInfo->RenderPassShadowmap)[i]) {
                    if(appInfo->GraphicsPipelines[i].graphics_pipeline_renderpasses_shadowmap) {
                        renderProcess.createGraphicsPipeline(Vertex3D::getBindingDescription, Vertex3D::getAttributeDescriptions, 
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], true, false, 
                            renderProcess.renderPass_shadowmap, i, appInfo); 
                    }else{
                        renderProcess.createGraphicsPipeline(Vertex3D::getBindingDescription, Vertex3D::getAttributeDescriptions, 
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], true, false, 
                            renderProcess.renderPass_mainscene, i, appInfo);
                    }   
                break;
                case VertexStructureTypes::TwoDimension:
                    //std::cout<<"CreatePipeline: Create 2D pipeline"<<std::endl;
                    renderProcess.createGraphicsPipeline(Vertex2D::getBindingDescription, Vertex2D::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], true, false, 
                        renderProcess.renderPass_mainscene, i, appInfo);
                break;
                case VertexStructureTypes::ParticleType:
                    renderProcess.createGraphicsPipeline(Particle::getBindingDescription, Particle::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], true, false, 
                        renderProcess.renderPass_mainscene, i, appInfo);
                break;
                case VertexStructureTypes::TextQuad:
                    renderProcess.createGraphicsPipeline(NULL, NULL, //TextQuadVertex::getBindingDescription, TextQuadVertex::getAttributeDescriptions, 
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, shaderManager.vertShaderModules[i], shaderManager.fragShaderModules[i], true, true, 
                        renderProcess.renderPass_mainscene, i, appInfo);
                break;
                default:
                break;
            }
            if(bPipelineVerbose) std::cout<<"Done create one graphics pipeline"<<std::endl;
        }
        
    }
    if(appInfo->ComputePipelines.size() > 0){ //for now assume only one compute pipeline
        //! only support one compute pipeline
        renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
        renderProcess.createComputePipeline(shaderManager.compShaderModules[0]);
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
        objects[i].Register((Application*)this);
        
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
        logManager.print("Object ID: %d", i);
        logManager.print("\tName: %s", objects[i].Name.c_str());
        logManager.print("\tPosition: %f, %f, %f", objects[i].Position);
        logManager.print("\tLength_original: %f, %f, %f", objects[i].Length_original);
        logManager.print("\tLengthMin_original: %f, %f, %f", objects[i].LengthMin_original);
        logManager.print("\tLengthMax_original: %f, %f, %f", objects[i].LengthMax_original);
        logManager.print("\tScale: %f, %f, %f", objects[i].Scale);
        logManager.print("\tLength: %f, %f, %f", objects[i].Length);
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
        textManager.m_textBoxes[i].Register((Application*)this);
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
        logManager.print("Textbox ID: %d", i);
        logManager.print("\tName: %s", textManager.m_textBoxes[i].Name.c_str());
        logManager.print("\tPosition: %f, %f, %f", textManager.m_textBoxes[i].Position);
        logManager.print("\tLength_original: %f, %f, %f", textManager.m_textBoxes[i].Length_original);
        logManager.print("\tLengthMin_original: %f, %f, %f", textManager.m_textBoxes[i].LengthMin_original);
        logManager.print("\tLengthMax_original: %f, %f, %f", textManager.m_textBoxes[i].LengthMax_original);
        logManager.print("\tScale: %f, %f, %f", textManager.m_textBoxes[i].Scale);
        logManager.print("\tLength: %f, %f, %f", textManager.m_textBoxes[i].Length);
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

    instance_sdlcore->SetKeyboardSensibility(appInfo->MainCamera.camera_keyboard_sensitive);
    instance_sdlcore->SetMouseSensibility(appInfo->MainCamera.camera_mouse_sensitive);

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
    renderer.CreateSyncObjects(swapchain.swapchainImageSize);
    shaderManager.Destroy();

    TimePoint T13 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for creating sync objects and destroy shaders", T12, T13);
}

}