#include "../include/application.h"

namespace LEApplication{

void Application::Initialize(){
    instance_game->Initialize();

    bool bVerboseInitialization = false;
    TimePoint T0 = now();

    instance_yamlcore->ReadYAMLFile(m_sampleName);

    /****************************
    * 1 Read Features and Controls
    ****************************/   
    renderer.m_renderMode = (RenderModes)appInfo->RenderMode;
    if(appInfo->Feature.b_feature_graphics_push_constant)
        shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);
    if(appInfo->Feature.b_feature_graphics_global_blend)
        renderProcess.addColorBlendAttachment(
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);        


    TimePoint T1 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading features", T0, T1);
        //printElapsed("Application: Total initialize elapsed time", T0, T1);
    }

    /****************************
    * 2 Initialize ObjectList and LightList
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
    * Precompute size for object/textbox/light
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
    if(bVerboseInitialization) printElapsed("Application: Initialize time for object and light List", T1, T2);
    

    /****************************
    * 3 Read Uniforms
    ****************************/
    ReadUniforms();

    TimePoint T3 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading uniforms", T2, T3);

    /****************************
    * 3.2 Read Subpasses
    ****************************/
    ReadAttachments();

    TimePoint T4 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading attachements", T3, T4);
    

    /****************************
    * 3.5 Read Subpasses
    ****************************/
    ReadSubpasses();

    TimePoint T5 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading subpasses", T4, T5);
    
    /****************************
    * 4 Read Resources
    ****************************/
    //When creating texture resource, need uniform information, so must read uniforms before read resources
    ReadResources();

    TimePoint T6 = now();
    if(bVerboseInitialization) printElapsed("Application: Initialize time for reading resources", T5, T6);

    /****************************
    * 5 Create Uniform Descriptors
    ****************************/
    bool b_uniform_graphics = appInfo->Uniform.b_uniform_graphics_custom || appInfo->Uniform.b_uniform_graphics_object_mvp || appInfo->Uniform.b_uniform_graphics_text_mvp || appInfo->Uniform.b_uniform_graphics_object_vp;
    bool b_uniform_compute = appInfo->Uniform.b_uniform_compute_custom || appInfo->Uniform.b_uniform_compute_storage || appInfo->Uniform.b_uniform_compute_swapchain_storage || appInfo->Uniform.b_uniform_compute_texture_storage;
    CreateUniformDescriptors(b_uniform_graphics, b_uniform_compute);

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