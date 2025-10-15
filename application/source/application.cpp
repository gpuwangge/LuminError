#include "../include/application.h"
#include "Foundation.h"
#include <locale>

namespace LEApplication{

//static class members must be defined outside. 
//otherwise invoke 'undefined reference' error when linking
Camera Application::mainCamera;
//Camera CApplication::lightCameras[2];
//std::vector<Camera> CApplication::lightCameras;
bool Application::NeedToExit = false;
bool Application::NeedToPause = false;
//bool CApplication::PrintFPS = false;
//int CApplication::focusObjectId = 0;
std::vector<CObject> Application::objects;
//std::vector<CTextBox> CApplication::textBoxes;
std::vector<CLight> Application::lights;

std::string getPureName(const std::string& path) {
    std::string result = path;
    
    // 移除前导的 .\ 或 ./
    if (result.size() >= 2 && result[0] == '.' && 
        (result[1] == '\\' || result[1] == '/')) {
        result = result.substr(2);
    }
    
    // 找到最后一个路径分隔符
    size_t lastSlash = result.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        result = result.substr(lastSlash + 1);
    }
    
    // 移除扩展名
    size_t lastDot = result.find_last_of('.');
    if (lastDot != std::string::npos) {
        result = result.substr(0, lastDot);
    }
    
    return result;
}

Application::Application(){
    //debugger = new CDebugger("../logs/application.log");

    //NeedToExit = false;
    windowWidth = 0;
    windowHeight = 0;

    //lightCameras.resize(2); //work
    lightCameras.resize(LIGHT_MAX); //TODO: for test purpose, create more cameras than needed

    logManager.setLogFile("application.log");
}

#ifndef ANDROID
void Application::Run(std::string exampleName){ //Entrance Function
    void* pVoid = nullptr;
    LoadModuleAndInstance(handle_module_sdlcore, pVoid, "sdlcore.dll");
    instance_sdlcore = static_cast<LESDL::ISDLCore*>(pVoid);
    //instance_sdlcore->greet();
    instance_sdlcore->SetApplication(this);

    LoadModuleAndInstance(handle_module_game, pVoid, exampleName);
    instance_game = static_cast<LuminError::IGame*>(pVoid);
    instance_game->SetApplication(this);

    instance_game->PreInitialize();

    m_sampleName = exampleName;

    CContext::Init();

    /**************** 
    * Five steps with third-party(GLFW or SDL) initialization
    * Step 1: Create Window
    *****************/
    //std::cout<<m_sampleName<<std::endl;
    //m_sampleName.erase(0, 1);
    m_sampleName = getPureName(m_sampleName);
    //std::cout<<m_sampleName<<std::endl;
#ifdef SDL
   // sdlManager.m_pApp = this;
    //sdlManager.createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
    instance_sdlcore->createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
#else
    glfwManager.createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
#endif
	PRINT("run: Created Window. Window width = %d,  height = %d.", windowWidth, windowHeight);

    /**************** 
    * Step 2: Select required layers
    *****************/
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    
    /**************** 
    * Step 3: Select required instance extensions
    *****************/
    std::vector<const char*> requiredInstanceExtensions;
#ifdef SDL
    //sdlManager.queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
    instance_sdlcore->queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
#else    
    glfwManager.queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
#endif
    if(enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    /**************** 
    * Step 4: create instance
    *****************/
    //prepareVulkanDevices();
    instance = std::make_unique<CInstance>(requiredValidationLayers, requiredInstanceExtensions);

    /**************** 
    * Step 5: create surface
    * Surface is to store view format information for creating swapchain. 
    * Only third party(glfw or sdl) knows what kind of surface can be attached to its window.
    *****************/
#ifdef SDL   
    //sdlManager.createSurface(IN instance, OUT surface);
    instance_sdlcore->createSurface(IN instance, OUT surface);
#else  
    glfwManager.createSurface(IN instance, OUT surface);
#endif

    /**************** 
    * General initialization begins
    * Select required queue families
    * Select required device extensions
    *****************/
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    instance->findAllPhysicalDevices();

    CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(surface, requireDeviceExtensions, requiredQueueFamilies);
    //App dev can only query properties from physical device, but can not directly operate it
    //App dev operates logical device, can logical device communicate with physical device by command queues
    //App dev will fill command buffer with commands later
    //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);

    //query  basic capabilities of surface
    //VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities;
    //std::cout<<vkGetPhysicalDeviceSurfaceCapabilitiesKHR(CContext::GetHandle().GetPhysicalDevice(), surface, pSurfaceCapabilities)<<std::endl;
    //std::cout<<"Surface min extent: width="<<pSurfaceCapabilities->minImageExtent.width<<", Surface min extent: height="<<pSurfaceCapabilities->minImageExtent.height<<std::endl;
    //std::cout<<"Surface max extent: width="<<pSurfaceCapabilities->maxImageExtent.width<<", Surface max extent: height="<<pSurfaceCapabilities->maxImageExtent.height<<std::endl;
    //lightCameras.resize(2);//work
    swapchain.createSwapchainImages(surface, windowWidth, windowHeight);
    //lightCameras.resize(2);//not work
	swapchain.createSwapchainViews(VK_IMAGE_ASPECT_COLOR_BIT);

    renderer.CreateCommandPool(surface);

    std::cout<<"======================================="<<std::endl;
    std::cout<<"======Welcome to Vulkan Platform======="<<std::endl;
    std::cout<<"======================================="<<std::endl;

    //auto startInitialzeTime = std::chrono::high_resolution_clock::now();

    TimePoint T0 = now();
    Initialize();
    TimePoint T1 = now();
    totalInitTime = printElapsed("Application: Total Initialization cost", T0, T1);
    //if(textManager.m_textBoxes.size() > 3) textManager.m_textBoxes[3].SetTextContent("Init:" + to_string_prec(totalInitTime) + " ms");

    //auto endInitializeTime = std::chrono::high_resolution_clock::now();
    //auto durationInitializationTime = std::chrono::duration<float, std::chrono::seconds::period>(endInitializeTime - startInitialzeTime).count() * 1000;
    //std::cout<<"Total Initialization cost: "<<durationInitializationTime<<" milliseconds"<<std::endl;

#ifdef SDL
    while(instance_sdlcore->IsRunning()){
        instance_sdlcore->eventHandle();
        if(!NeedToPause) UpdateRecordRender();
        if(NeedToExit) break;
    }
#else  
    glfwSwapInterval(1); //set V-Sync. wait a refresh period, normally 16.67ms
    while (!glfwWindowShouldClose(glfwManager.window)) {
        glfwPollEvents();
        if(!NeedToPause) UpdateRecordRender();
        if(NeedToExit) break;

        if(mainCamera.cameraType == Camera::CameraType::FREE){
            mainCamera.AngularVelocity.x = 0;
            mainCamera.AngularVelocity.y = 0;
        }

        glfwWaitEventsTimeout(1.0 / 60.0);
    }
#endif

    //std::cout<<"Application: vkDeviceWaitIdle()..."<<std::endl;
	vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
    //std::cout<<"Application: vkDeviceWaitIdle() finished."<<std::endl;
}
#endif

void Application::Initialize(){
    instance_game->Initialize();

    bool bVerboseInitialization = false;
    TimePoint T0 = now();

    // static auto startInitTime = std::chrono::high_resolution_clock::now();
    // static auto lastInitTime = std::chrono::high_resolution_clock::now();
    // auto currentInitTime = std::chrono::high_resolution_clock::now();
    // float elapseInitTime = 0;
    // float deltaInitTime = 0;

    //m_timer.reset();

    //auto currentTime = std::chrono::high_resolution_clock::now();
    //elapseTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    //deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();

    std::string fullYamlName = YAML_PATH + m_sampleName + ".yaml";
    try{
        config = YAML::LoadFile(fullYamlName);
    } catch (...){
        std::cout<<"Error loading yaml file"<<std::endl;
        return;
    }

    /****************************
    * ? ControlNode
    ****************************/
    //pControlNodes.push_back(&perfMetric);

    /****************************
    * 1 Read Features and Controls
    ****************************/   
    renderer.m_renderMode = appInfo.RenderMode;
    ReadFeatures();
    ReadControls();

    TimePoint T1 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading features", T0, T1);
        //printElapsed("Application: Total initialize elapsed time", T0, T1);
    }

    /****************************
    * 2 Initialize ObjectList and LightList
    ****************************/
    if(appInfo.Feature.feature_graphics_enable_controls){
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

        for(int i = 0; i < controlNodes.size(); i++) controlNodes[i]->bVisible = appInfo.Feature.feature_graphics_show_all_metric_controls;
        if(!appInfo.Feature.feature_graphics_show_all_metric_controls) controlNodes[0]->bVisible = appInfo.Feature.feature_graphics_show_performance_control; //show performance control only
    }

    //controlNodes[0]->bVisible = false; //hide fps control node

    int max_object_id = -1;
    if (config["Objects"]) {
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            max_object_id = (object_id > max_object_id) ? object_id : max_object_id;
        }
    }
    customObjectSize = ((max_object_id+1) < config["Objects"].size()) ? (max_object_id+1) : config["Objects"].size();
    objects.resize(customObjectSize + objectCountControl);
    std::cout<<"Object Size: "<<objects.size()<<std::endl;
    
    int max_textbox_id = -1;
    if (config["Textboxes"]) {
        for (const auto& tb : config["Textboxes"]) {
            int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
            max_textbox_id = (textbox_id > max_textbox_id) ? textbox_id : max_textbox_id;
        }
    }
    customTextboxSize = ((max_textbox_id+1) < config["Textboxes"].size()) ? (max_textbox_id+1) : config["Textboxes"].size();
    textManager.m_textBoxes.resize(customTextboxSize + textboxCountControl);
    for(int i = 0; i < textManager.m_textBoxes.size(); i++)
        textManager.m_textBoxes[i].p_textManager = &textManager;
    std::cout<<"Textbox Size: "<<textManager.m_textBoxes.size()<<std::endl;
    
    if (config["Lights"]) {
        int max_light_d = 0;
        for (const auto& light : config["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            max_light_d = (light_id > max_light_d) ? light_id : max_light_d;
        }
        customLightsSize = ((max_light_d+1) < config["Lights"].size())?(max_light_d+1):config["Lights"].size();
        lights.resize(customLightsSize + lightCountControl);
        std::cout<<"Light Size: "<<lights.size()<<std::endl;

        swapchain.buffer_depthlight.resize(lights.size());
        swapchain.framebuffers_shadowmap.resize(lights.size());
    }
    //update light number to ubo
    //std::cout<<"CGraphicsDescriptorManager::m_lightingUBO.lightNum = "<<lights.size()<<std::endl;
    CGraphicsDescriptorManager::m_lightingUBO.lightNum = lights.size();

    // if(bVerboseInitialization){
    //     currentInitTime = std::chrono::high_resolution_clock::now();
    //     elapseInitTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentInitTime - startInitTime).count();
    //     deltaInitTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentInitTime - lastInitTime).count();
    //     std::cout<<": Initialize Object and Light List time cost: "<<deltaInitTime<<" milliseconds"<<std::endl;
    //     std::cout<<": Initialize time elapsed: "<<elapseInitTime<<" milliseconds"<<std::endl;
    //     lastInitTime = currentInitTime;
    // }


    //std::cout<<"Application: Initialize time for object and light List: "<<m_timer.elapsedSinceLastCheck()<<" milliseconds"<<std::endl;
    //std::cout<<"Application: Total initialize elapsed time: "<<m_timer.elapsedMilliseconds()<<" milliseconds"<<std::endl;


    // if(bVerboseInitialization){
    //     currentInitTime = std::chrono::high_resolution_clock::now();
    //     elapseInitTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentInitTime - startInitTime).count();
    //     deltaInitTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentInitTime - lastInitTime).count();
    //     std::cout<<": Initialize ReadFeatures time cost: "<<deltaInitTime<<" milliseconds"<<std::endl;
    //     std::cout<<": Initialize time elapsed: "<<elapseInitTime<<" milliseconds"<<std::endl;
    //     lastInitTime = currentInitTime;
    // }

    //std::cout<<"Application: Initialize time for reading features: "<<m_timer.elapsedSinceLastCheck()<<" milliseconds"<<std::endl;
    //std::cout<<"Application: Initialize total elapsed time: "<<m_timer.elapsedMilliseconds()<<" milliseconds"<<std::endl;

    TimePoint T2 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for object and light List", T1, T2);
        //printElapsed("Application: Total initialize elapsed time", T1, T2);
    }

    /****************************
    * 3 Read Uniforms
    ****************************/
    ReadUniforms();

    TimePoint T3 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading uniforms", T2, T3);
        //printElapsed("Application: Total initialize elapsed time", T0, T3);
    }

    /****************************
    * 3.2 Read Subpasses
    ****************************/
    ReadAttachments();

    TimePoint T4 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading attachements", T3, T4);
        //printElapsed("Application: Total initialize elapsed time", T0, T4);
    }

    /****************************
    * 3.5 Read Subpasses
    ****************************/
    ReadSubpasses();

    TimePoint T5 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading subpasses", T4, T5);
        //printElapsed("Application: Total initialize elapsed time", T0, T5);
    }
    
    /****************************
    * 4 Read Resources
    ****************************/
    //When creating texture resource, need uniform information, so must read uniforms before read resources
    ReadResources();

    TimePoint T6 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading resources", T5, T6);
        //printElapsed("Application: Total initialize elapsed time", T0, T6);
    }

    /****************************
    * 5 Create Uniform Descriptors
    ****************************/
    bool b_uniform_graphics = appInfo.Uniform.b_uniform_graphics_custom || appInfo.Uniform.b_uniform_graphics_object_mvp || appInfo.Uniform.b_uniform_graphics_text_mvp || appInfo.Uniform.b_uniform_graphics_object_vp;
    bool b_uniform_compute = appInfo.Uniform.b_uniform_compute_custom || appInfo.Uniform.b_uniform_compute_storage || appInfo.Uniform.b_uniform_compute_swapchain_storage || appInfo.Uniform.b_uniform_compute_texture_storage;
    CreateUniformDescriptors(b_uniform_graphics, b_uniform_compute);

    TimePoint T7 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for creating uniform descriptors", T6, T7);
        //printElapsed("Application: Total initialize elapsed time", T0, T7);
    }

    /****************************
    * 6 Create Pipelines
    ****************************/
    CreatePipelines();

    TimePoint T8 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for creating pipelines", T7, T8);
        //printElapsed("Application: Total initialize elapsed time", T0, T8);
    }

    /****************************
    * 7 Read and Register Objects
    ****************************/
    ReadRegisterObjects();

    TimePoint T9 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading register objects", T8, T9);
        //printElapsed("Application: Total initialize elapsed time", T0, T9);
    }

    /****************************
    * 8 Read and Register Textboxes
    ****************************/
    ReadRegisterTextboxes();
    

    TimePoint T10 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading register textboxes", T9, T10);
        //printElapsed("Application: Total initialize elapsed time", T0, T10);
    }

    /****************************
    * 9 Read Lightings
    ****************************/
    ReadLightings();

    TimePoint T11 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading lightings", T10, T11);
        //printElapsed("Application: Total initialize elapsed time", T0, T11);
    }
    
    /****************************
    * 9 Read Main Camera
    ****************************/
    ReadCameras();

    TimePoint T12 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for reading cameras", T11, T12);
        //printElapsed("Application: Total initialize elapsed time", T0, T12);
    }

    /****************************
    * 10 Create Sync Objects and Clean up Shaders (+and call example initialization)
    ****************************/
    renderer.CreateSyncObjects(swapchain.swapchainImageSize);
    shaderManager.Destroy();

    

    TimePoint T13 = now();
    if(bVerboseInitialization){
        printElapsed("Application: Initialize time for creating sync objects and destroy shaders", T12, T13);
        //printElapsed("Application: Total initialize elapsed time", T0, T12);
    }

    // CContext::GetHandle().logManager.print("Test single string!\n");
    // CContext::GetHandle().logManager.print("Test interger: %d!\n", 999);
    // CContext::GetHandle().logManager.print("Test float: %f!\n", 1.234f);
    // CContext::GetHandle().logManager.print("Test string: %s!\n", "another string");
    // float mat[4] = {1.1, 2.2, 3.3, 4.4};
    // CContext::GetHandle().logManager.print("Test vector: \n", mat, 4);
    // CContext::GetHandle().logManager.print("Test two floats:  %f, %f!\n", 1.2, 2.3);
    // PRINT("Test single string!");
    // PRINT("Test interger: %d!", 999);
    // PRINT("Test float: %f!", 1.234f);
    // PRINT("Test string: %s!", "another string");
    // float mat[4] = {1.1, 2.2, 3.3, 4.4};
    // PRINT("Test vector: ", mat, 4);
    // PRINT("Test two floats:  %f, %f!", 1.2, 2.3);    
    instance_game->PostInitialize();
}

void Application::Update(){
    instance_game->Update();

    static TimePoint startTimePoint = now();
    static TimePoint lastTimePoint = now();
    TimePoint currentTimePoint = now();
    elapseTime = secondsBetween(startTimePoint, currentTimePoint);
    deltaTime = secondsBetween(lastTimePoint, currentTimePoint);
    lastTimePoint = currentTimePoint;

    //static auto startTime = std::chrono::high_resolution_clock::now();
    //static auto lastTime = std::chrono::high_resolution_clock::now();
    // auto currentTime = std::chrono::high_resolution_clock::now();
    // elapseTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    // deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    // lastTime = currentTime;

    if(objects.size() > 0 && mainCamera.focusObjectId < objects.size())
        mainCamera.SetTargetPosition(objects[mainCamera.focusObjectId].Position);
    mainCamera.update(deltaTime);

    for(int i = 0; i < lights.size(); i++){//lightCameras.size()
        if(lights.size() > 0 && lightCameras[i].focusObjectId < objects.size())
            lightCameras[i].SetTargetPosition(objects[lightCameras[i].focusObjectId].Position);
        lightCameras[i].update(deltaTime);
    }
    // if(objects.size() > 0 && lightCamera[0].focusObjectId < objects.size())
    //     lightCamera[0].SetTargetPosition(objects[lightCamera[0].focusObjectId].Position);
    // if(objects.size() > 0 && lightCamera[1].focusObjectId < objects.size())
    //     lightCamera[1].SetTargetPosition(objects[lightCamera[1].focusObjectId].Position);  
    //lightCamera[0].update(deltaTime);
    //lightCamera[1].update(deltaTime);

    for(int i = 0; i < objects.size(); i++) objects[i].Update(deltaTime, renderer.currentFrame, mainCamera); 
    textManager.Update(deltaTime, renderer.currentFrame, mainCamera);
    for(int i = 0; i < lights.size(); i++) lights[i].Update(deltaTime, renderer.currentFrame, mainCamera, lightCameras[i]);
    if(appInfo.Feature.feature_graphics_enable_controls)
        for(int i = 0; i < controlNodes.size(); i++) controlNodes[i]->Update();

    /*Calcuate FPS*/
    //static int tempCount = 0;
    //static TimePoint intervalStartTimePoint = now();
    // if(PrintFPS){
    //     double intervalElapseTime = millisecondsBetween(intervalStartTimePoint, currentTimePoint);
    //     if(intervalElapseTime > 1000){
    //         std::cout<<"FPS: "<<tempCount<<" interval: "<<intervalElapseTime<<" milliseconds"<<std::endl;
    //         tempCount = 0;
    //         intervalStartTimePoint = now();
    //     }else tempCount++;
    // }
    frameCount++;


    // static auto intervalStartTime = std::chrono::high_resolution_clock::now();
    // static auto intervalEndTime = std::chrono::high_resolution_clock::now();
    // if(PrintFPS){
    //     intervalEndTime = std::chrono::high_resolution_clock::now();
    //     auto intervalElapseTime = std::chrono::duration<float, std::chrono::milliseconds::period>(intervalEndTime - intervalStartTime).count();
    //     if(intervalElapseTime > 1000){
    //         std::cout<<"FPS: "<<frameCount<<" interval: "<<intervalElapseTime<<" milliseconds"<<std::endl;
    //         frameCount = 0;
    //         intervalStartTime = std::chrono::high_resolution_clock::now();
    //     }else frameCount++;
    // }
    
}

//void Application::PostUpdate(){ instance_game->PostUpdate();}

void Application::RecordGraphicsCommandBuffer_RenderpassMainscene(){
    instance_game->Record();
    instance_game->RecordGraphicsCommandBuffer_RenderpassMainscene();
    //for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	//textManager.Draw();
}
void Application::RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex){instance_game->RecordGraphicsCommandBuffer_RenderpassShadowmap(renderpassIndex);}
void Application::RecordComputeCommandBuffer(){instance_game->RecordComputeCommandBuffer();}


void Application::UpdateRecordRender(){
    Update();

    /**************************
     * 
     * Universial Render Functions
     * 
     * ***********************/
    switch(renderer.m_renderMode){
        case RenderModes::GRAPHICS:
        //case renderer.RENDER_GRAPHICS_Mode:
            //std::cout<<"RENDER_GRAPHICS_Mode"<<std::endl;

            //must wait for fence before record command buffer
            renderer.WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain); 

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            renderer.StartRecordGraphicsCommandBuffer(
                renderProcess.renderPass_mainscene, 
                swapchain.framebuffers_mainscene,swapchain.swapChainExtent, 
                renderProcess.clearValues);
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer.EndRecordGraphicsCommandBuffer();

            renderer.SubmitGraphics();

            renderer.PresentSwapchainImage(swapchain);
        break;
        case RenderModes::GRAPHICS_SHADOWMAP:
            //must wait for fence before record command buffer
            renderer.WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain); 

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            renderer.BeginCommandBuffer(renderer.graphicsCmdId);

            for(int i = 0; i < swapchain.framebuffers_shadowmap.size(); i++){
                //std::cout<<"Application: Begin Shadowmap"<<i<<" Render Pass."<<std::endl;
                renderer.BeginRenderPass(renderProcess.renderPass_shadowmap, swapchain.framebuffers_shadowmap[i], swapchain.swapChainExtent, renderProcess.clearValues_shadowmap, true);
                renderer.SetViewport(swapchain.swapChainExtent);
                renderer.SetScissor(swapchain.swapChainExtent);
                RecordGraphicsCommandBuffer_RenderpassShadowmap(i);
                renderer.EndRenderPass();
            }

            //std::cout<<"Application: Begin Mainscene Render Pass."<<std::endl;
            renderer.BeginRenderPass(renderProcess.renderPass_mainscene, swapchain.framebuffers_mainscene, swapchain.swapChainExtent, renderProcess.clearValues, false);
            renderer.SetViewport(swapchain.swapChainExtent);
            renderer.SetScissor(swapchain.swapChainExtent);
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer.EndRenderPass();

	        renderer.EndCommandBuffer(renderer.graphicsCmdId);

            renderer.SubmitGraphics();

            renderer.PresentSwapchainImage(swapchain);


        break;
        case RenderModes::COMPUTE:
        //case renderer.RENDER_COMPUTE_Mode:
            //std::cout<<"Application: RENDER_COMPUTE_Mode."<<std::endl;
            renderer.WaitForComputeFence();//must wait for fence before record
            //std::cout<<"Application: renderer.WaitForComputeFence()"<<std::endl;

            vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            //std::cout<<"Application: vkResetCommandBuffer"<<std::endl;

            renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            RecordComputeCommandBuffer();
            renderer.EndRecordComputeCommandBuffer();
            //std::cout<<"Application: recordComputeCommandBuffer()"<<std::endl;

            renderer.SubmitCompute();
            //std::cout<<"Application: renderer.SubmitCompute()"<<std::endl;

           // renderer.PresentSwapchainImage(swapchain); //???
        break;
        case RenderModes::COMPUTE_SWAPCHAIN:
        //case renderer.RENDER_COMPUTE_SWAPCHAIN_Mode:
            //must wait for fence before record
            renderer.WaitForComputeFence();
            //must aquire swap image before record command buffer
            renderer.AquireSwapchainImage(swapchain);
            //std::cout<<"Application: renderer.imageIndex = "<<renderer.imageIndex<< std::endl;
            //std::cout<<"Application: renderer.currentFrame = "<<renderer.currentFrame<< std::endl;

            //vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            //in this mode, nothing is recorded(all commands are pre-recorded), for NOW. But still, swapchain will be presented.
            //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            //recordComputeCommandBuffer();
            //renderer.EndRecordComputeCommandBuffer();

            renderer.SubmitCompute(); 

            renderer.PresentSwapchainImage(swapchain); 
        break;
        case RenderModes::COMPUTE_GRAPHICS:
        //case renderer.RENDER_COMPUTE_GRAPHICS_Mode:
            renderer.WaitForComputeFence();//must wait for fence before record
            renderer.WaitForGraphicsFence();//must wait for fence before record
            renderer.AquireSwapchainImage(swapchain);//must aquire swap image before record command buffer

            vkResetCommandBuffer(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            
            renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            RecordComputeCommandBuffer();
            renderer.EndRecordComputeCommandBuffer();

            renderer.StartRecordGraphicsCommandBuffer(
                renderProcess.renderPass_mainscene,
                swapchain.framebuffers_mainscene, swapchain.swapChainExtent,
                renderProcess.clearValues);
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer.EndRecordGraphicsCommandBuffer();
            
            renderer.SubmitCompute(); 
            renderer.SubmitGraphics(); 

            renderer.PresentSwapchainImage(swapchain); 
        break;
        default:
        break;
    }

    instance_game->PostUpdate();

    renderer.Update(); //update currentFrame
}


#ifndef ANDROID
void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif

void Application::CleanUp(){
    //std::cout<<"Begin Cleanup()..."<<std::endl;

    //std::cout<<"Application: swapchain.CleanUp()"<<std::endl;
    swapchain.CleanUp();
    //std::cout<<"Application: renderProcess.CleanUp()"<<std::endl;
    renderProcess.Cleanup();

    //std::cout<<"Application: graphicsDescriptorManager.Destroy()"<<std::endl;
    graphicsDescriptorManager.DestroyAndFree();
    //std::cout<<"Application: computeDescriptorManager.DestroyAndFree()"<<std::endl;
    computeDescriptorManager.DestroyAndFree();

    //std::cout<<"Application: textureManager.Destroy()"<<std::endl;
    textureManager.Destroy();
    textImageManager.Destroy();
    textManager.Destroy();
    //std::cout<<"Application: renderer.Destroy()"<<std::endl;
    renderer.Destroy();

    //std::cout<<"Application: vkDestroyDevice()"<<std::endl;
    vkDestroyDevice(CContext::GetHandle().GetLogicalDevice(), nullptr);

#ifndef ANDROID
    if (enableValidationLayers) 
        DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);
#endif

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);
    
    CContext::Quit();

    //std::cout<<"End Cleanup()."<<std::endl;
}

/*************
 * Helper Functions
 *******/

 
void Application::ReadControls(){
    for (const auto& control : config["Controls"])
        if (control["UIContainer"]) appInfo.ControlUIContainer.loadFromYaml(control["UIContainer"]);
}

void Application::ReadFeatures(){
    if (config["Features"]) appInfo.Feature.loadFromYaml(config["Features"]);

    if(appInfo.Feature.b_feature_graphics_push_constant){
        shaderManager.CreatePushConstantRange<ModelPushConstants>(VK_SHADER_STAGE_VERTEX_BIT, 0);
    }
    if(appInfo.Feature.b_feature_graphics_global_blend){
        renderProcess.addColorBlendAttachment(
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO);        
    }
    //renderProcess.skyboxID = appInfo.Feature.feature_graphics_pipeline_skybox_id;
}

void Application::ReadUniforms(){
    auto uniformsNode = config["Uniforms"];

    // Graphics
    if(uniformsNode["Graphics"]) appInfo.Uniform.loadGraphicsFromYaml(uniformsNode["Graphics"]);

    if(appInfo.Uniform.b_uniform_graphics_custom)
        CGraphicsDescriptorManager::addCustomUniformBuffer(appInfo.Uniform.GraphicsCustom.Size);
    
    if(appInfo.Uniform.b_uniform_graphics_lighting)
        CGraphicsDescriptorManager::addLightingUniformBuffer();

    if(appInfo.Uniform.b_uniform_graphics_object_mvp){
        CGraphicsDescriptorManager::addMVPUniformBuffer();
        renderer.bUseObjectMVP = true;
    }

    if(appInfo.Uniform.b_uniform_graphics_text_mvp){
        CGraphicsDescriptorManager::addTextMVPUniformBuffer();
        renderer.bUseTextboxMVP = true;
    }   

    if(appInfo.Uniform.b_uniform_graphics_object_vp){
        CGraphicsDescriptorManager::addVPUniformBuffer();
        renderer.bUseObjectMVP = true; //reuse MVP bool
    }

    if(appInfo.Uniform.b_uniform_graphics_depth_image_sampler)
        CGraphicsDescriptorManager::addDepthImageSamplerUniformBuffer();

    if(appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler)
        CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer();

    if(appInfo.Uniform.b_uniform_graphics_lightdepth_image_sampler_hardware){
        CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias();
        //CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias2();
    }

    //std::cout<<"Begin Read Compute"<<std::endl;
    // Compute
    if (uniformsNode["Compute"]) appInfo.Uniform.loadComputeFromYaml(uniformsNode["Compute"]);
    if(appInfo.Uniform.b_uniform_compute_custom)
        //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_UNIFORMBUFFER_CUSTOM;
        CComputeDescriptorManager::addCustomUniformBuffer(appInfo.Uniform.ComputeCustom.Size);

    if(appInfo.Uniform.b_uniform_compute_storage)
        //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEBUFFER_DOUBLE;
        CComputeDescriptorManager::addStorageBuffer(appInfo.Uniform.ComputeStorageBuffer.Size, appInfo.Uniform.ComputeStorageBuffer.Usage);

    if(appInfo.Uniform.b_uniform_compute_texture_storage)
        //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEIMAGE_TEXTURE;
        CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_TEXTURE);

    if(appInfo.Uniform.b_uniform_compute_swapchain_storage)
        //CComputeDescriptorManager::computeUniformTypes |= COMPUTE_STORAGEIMAGE_SWAPCHAIN;
        CComputeDescriptorManager::addStorageImage(COMPUTE_STORAGEIMAGE_SWAPCHAIN);
    //std::cout<<"End Read Compute"<<std::endl;

    //GraphicsTextureImageSamplers
    if (uniformsNode["GraphicsTextureImageSamplers"]) {
        auto samplersNode = uniformsNode["GraphicsTextureImageSamplers"];
        std::vector<int> miplevels;
        std::vector<std::vector<bool>> uvwRepeats;

        for (const auto& samplerNode : samplersNode) {
            std::string name = getOrDefault<std::string>(samplerNode, "uniform_graphics_texture_image_sampler_name", ""); //not used
            int miplevel = getOrDefault<int>(samplerNode, "uniform_graphics_texture_image_sampler_miplevel", 0);
            std::vector<bool> uvwRepeat = getOrDefault<std::vector<bool>>(samplerNode, "uniform_graphics_texture_image_sampler_uvwrepeat", {true,true,true});

            miplevels.push_back(miplevel);
            uvwRepeats.push_back(uvwRepeat);
        }

        //std::cout<<"miplevels.size "<<miplevels.size()<<std::endl;

        CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE;
        CGraphicsDescriptorManager::addTextureImageSamplerUniformBuffer(miplevels, uvwRepeats);
    }

}

void Application::ReadResources(){
    for (const auto& resource : config["Resources"]) {
        if (resource["Fonts"]) {
            for (const auto& font : resource["Fonts"]) {
                std::string name = font["resource_font_name"].as<std::string>();
                int samplerid = font["uniform_sampler_id"].as<int>();
                //std::vector<bool> uvwRepeat = samplerUniform["uniform_graphics_texture_image_sampler_uvwrepeat"] ? samplerUniform["uniform_graphics_texture_image_sampler_uvwrepeat"].as<std::vector<bool>>() : std::vector<bool>{true, true, true};
                std::vector<int> outlineColor = font["resource_font_outlinecolor"] ? font["resource_font_outlinecolor"].as<std::vector<int>>() : std::vector<int>{255, 255, 255, 255};
                std::vector<int> textColor = font["resource_font_textcolor"] ? font["resource_font_textcolor"].as<std::vector<int>>() : std::vector<int>{0, 0, 0, 255};
                int fontSize = font["resource_font_size"] ? font["resource_font_size"].as<int>() : 20;
                //std::cout<<"Font name: "<<name<<std::endl;
                textManager.SetFontSize(fontSize);
                textManager.SetSamplerID(samplerid);
                textManager.SetOutlineColor(glm::vec4(outlineColor[0], outlineColor[1], outlineColor[2], outlineColor[3]));
                textManager.SetTextColor(glm::vec4(textColor[0], textColor[1], textColor[2], textColor[3]));
                textManager.p_renderer = &renderer;
                textManager.p_textImageManager = &textImageManager;
                textManager.p_modelManager = &modelManager;

                textManager.CreateTextImage(); //create text atlas image and push to textImageManager
                textManager.CreateGlyphMap(); //create glyph map
                textManager.CreateTextResource(); //loop every textbox[i], create instance data, and create model based on instance data
                //std::cout<<"Font "<<name<<" loaded."<<std::endl;
            }
        }

        if (resource["Models"]) {
            for (const auto& model : resource["Models"]) {
                std::string name = model["resource_model_name"] ? model["resource_model_name"].as<std::string>() : "Default";
                //std::cout<<"model name: "<<name<<std::endl;
                //id is not really useful here, because the model id must be in order
                int id = model["resource_model_id"] ? model["resource_model_id"].as<int>() : 0;

                //appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                if(name == "CUSTOM3D0"){
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
                }else if(name == "TEXTBOXIMAGE"){
                    renderer.CreateVertexBuffer<Vertex3D>(modelManager.textboxImageModels[0].vertices);
                    renderer.CreateIndexBuffer(modelManager.textboxImageModels[0].indices);
                    
                    modelManager.modelLengths.push_back(modelManager.textboxImageModels[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.textboxImageModels[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.textboxImageModels[0].lengthMax);
                }else if(name == "TEXTQUAD"){ //TODO: vertexBuffer and indexBuffer has the same index# of CUSTOM3D#, but instance buffer is 0
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
                }else if(name == "CUSTOM2D0"){
                    //appInfo.VertexBufferType = VertexStructureTypes::TwoDimension;
                    renderer.CreateVertexBuffer<Vertex2D>(modelManager.customModels2D[0].vertices); 

                    modelManager.modelLengths.push_back(modelManager.customModels2D[0].length);
                    modelManager.modelLengthsMin.push_back(modelManager.customModels2D[0].lengthMin);
                    modelManager.modelLengthsMax.push_back(modelManager.customModels2D[0].lengthMax);
                }else{
                    //appInfo.VertexBufferType = VertexStructureTypes::ThreeDimension;
                    std::vector<Vertex3D> modelVertices3D;
                    std::vector<uint32_t> modelIndices3D;
                    modelManager.LoadObjModel(name, modelVertices3D, modelIndices3D);
                    renderer.CreateVertexBuffer<Vertex3D>(modelVertices3D); 
                    renderer.CreateIndexBuffer(modelIndices3D);
                }
            }
        }

        //std::cout<<"Application: Total Texture Images before creating new ones: "<<textureManager.textureImages.size()<<std::endl;

        if (resource["Textures"]) {
            //texture id is allocated by engine, instead of user, in order
            for (const auto& texture : resource["Textures"]) {
                std::string name = texture["resource_texture_name"].as<std::string>();
                //int id = texture["resource_texture_id"].as<int>();
                int miplevel = texture["resource_texture_miplevels"].as<int>();
                bool enableCubemap = texture["resource_texture_cubmap"].as<bool>();
                int samplerid = texture["uniform_sampler_id"].as<int>();

                VkImageUsageFlags usage;// = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                //VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                //for(int i = 0; i < textureAttributes->size(); i++){
                    //auto startTextureTime = std::chrono::high_resolution_clock::now();

                if(miplevel > 1) //mipmap
                    usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                else 
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE) usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
                    else usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                
                if(!appInfo.Feature.b_feature_graphics_48pbt){ //24bpt
                    if(CComputeDescriptorManager::computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN) textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, swapchain.swapChainImageFormat);
                    else textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R8G8B8A8_SRGB, 8, enableCubemap);  
                }else{ //48bpt
                    //textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_UNORM, 16, enableCubemap); 
                    textureManager.CreateTextureImage(name, usage, renderer.commandPool, miplevel, samplerid, VK_FORMAT_R16G16B16A16_SFLOAT, 16, enableCubemap); 
                }
                
                if(appInfo.Feature.b_feature_graphics_rainbow_mipmap){
                    VkImageUsageFlags usage_mipmap = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps("checkerboard", usage_mipmap);
                }else if(miplevel > 1) textureManager.textureImages[textureManager.textureImages.size()-1].generateMipmaps();
                
                    //auto endTextureTime = std::chrono::high_resolution_clock::now();
                    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
                    //std::cout<<"Load Texture '"<< (*textureNames)[i].first <<"' cost: "<<durationTime<<" milliseconds"<<std::endl;
                //}
            }
        }

        //std::cout<<"Application: Total Texture Images after creating new ones: "<<textureManager.textureImages.size()<<std::endl;

        //shaders id is allocated by engine, not user, in order
        if (resource["Pipelines"]) {
            //Must initialize smart pointers, otherwise will crash
            appInfo.VertexShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            appInfo.FragmentShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            //appInfo.EnableSamplerCountOne =  std::make_unique<std::vector<bool>>(std::vector<bool>());
            //appInfo.EnableDepthBias =  std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.RenderPassShadowmap = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.Subpass =  std::make_unique<std::vector<int>>(std::vector<int>());
            appInfo.VertexDatatype = std::make_unique<std::vector<int>>(std::vector<int>());
            appInfo.BlendEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.DepthTestEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.DepthWriteEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.SkyboxEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());

            for (const auto& pipeline : resource["Pipelines"]) {
                //std::cout<<"Application: Read Pipeline."<<std::endl;
                std::string name = pipeline["resource_graphics_pipeline_name"].as<std::string>();
                std::string vertexShaderName = pipeline["resource_graphics_pipeline_vertexshader_name"].as<std::string>();
                std::string fragmentShaderName = pipeline["resource_graphics_pipeline_fragmentshader_name"].as<std::string>();
                //bool bEnableSamplerCountOne = pipeline["resource_graphics_pipeline_enable_sampler_count_one"] ? pipeline["resource_graphics_pipeline_enable_sampler_count_one"].as<bool>() : false;
                //bool bEnableDepthBias = pipeline["resource_graphics_pipeline_enable_depth_bias"] ? pipeline["resource_graphics_pipeline_enable_depth_bias"].as<bool>() : false;
                bool bRenderPassShadowmap = pipeline["renderpasses_shadowmap"] ? pipeline["renderpasses_shadowmap"].as<bool>() : false;
                int subpassId = pipeline["subpasses_subpass_id"] ? pipeline["subpasses_subpass_id"].as<int>() : 0;
                int vertexDatatype = pipeline["resource_graphics_pipeline_vertexdatatype"] ? pipeline["resource_graphics_pipeline_vertexdatatype"].as<int>() : 2; //2 is normal 3d vertex

                bool blendEnable = pipeline["resource_graphics_pipeline_blend_enable"] ? pipeline["resource_graphics_pipeline_blend_enable"].as<bool>() : false;
                bool depthTestEnable = pipeline["resource_graphics_pipeline_depth_test_enable"] ? pipeline["resource_graphics_pipeline_depth_test_enable"].as<bool>() : true;
                bool depthWriteEnable = pipeline["resource_graphics_pipeline_depth_write_enable"] ? pipeline["resource_graphics_pipeline_depth_write_enable"].as<bool>() : true;
                bool skyboxEnable = pipeline["resource_graphics_pipeline_skybox"] ? pipeline["resource_graphics_pipeline_skybox"].as<bool>() : false;

                //std::cout<<"Pipeline Name: "<<name<<std::endl;
                appInfo.VertexShader->push_back(vertexShaderName);
                appInfo.FragmentShader->push_back(fragmentShaderName);
                //appInfo.EnableSamplerCountOne->push_back(bEnableSamplerCountOne);
               // appInfo.EnableDepthBias->push_back(bEnableDepthBias);
                appInfo.RenderPassShadowmap->push_back(bRenderPassShadowmap);
                appInfo.Subpass->push_back(subpassId);
                appInfo.VertexDatatype->push_back(vertexDatatype);
                appInfo.BlendEnable->push_back(blendEnable);
                appInfo.DepthTestEnable->push_back(depthTestEnable);
                appInfo.DepthWriteEnable->push_back(depthWriteEnable);
                appInfo.SkyboxEnable->push_back(skyboxEnable);
            }

            // if (resource["VertexShaders"]) {
            //     auto vertexShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            //     for (const auto& vertexShader : resource["VertexShaders"]) {
            //         vertexShaderList->push_back(vertexShader["resource_vertexshader_name"].as<std::string>());
            //     }
            //     appInfo.VertexShader = std::move(vertexShaderList);
            // }

            // if (resource["FragmentShaders"]) {
            //     auto fragmentShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            //     for (const auto& fragmentShader : resource["FragmentShaders"]) {
            //         fragmentShaderList->push_back(fragmentShader["resource_fragmentshader_name"].as<std::string>());
            //     }
            //     appInfo.FragmentShader = std::move(fragmentShaderList);
            // }
        }

        if (resource["ComputeShaders"]) {
            auto computeShaderList = std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& computeShader : resource["ComputeShaders"]) {
                computeShaderList->push_back(computeShader["resource_computeshader_name"].as<std::string>());
            }
            appInfo.ComputeShader = std::move(computeShaderList);
        }
    }

    //std::cout<<"Application: Read Resources Done."<<std::endl;
}

void Application::ReadAttachments(){
    bool bShadowmapAttachmentDepthLight = config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"] ? config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"].as<bool>() : false;
    bool bMainSceneAttachmentDepthLight = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"].as<bool>() : false;
    bool bMainSceneAttachmentDepthCamera = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"].as<bool>()  : false;
    bool bMainSceneAttachmentColorResovle = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"].as<bool>()  : false;
    bool bMainSceneAttachmentColorPresent = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"].as<bool>()  : true; //need al least one subpass with at least one color attachment

    renderProcess.iShadowmapAttachmentDepthLight = bShadowmapAttachmentDepthLight ? 0 : -1; //shadowmap renderpass attachment depth light, only one attachment, so id is 0

    int AttachmentCount = 0;
    renderProcess.iMainSceneAttachmentDepthLight = bMainSceneAttachmentDepthLight ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentDepthCamera = bMainSceneAttachmentDepthCamera ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorResovle = bMainSceneAttachmentColorResovle ? AttachmentCount++ : -1;
    renderProcess.iMainSceneAttachmentColorPresent = bMainSceneAttachmentColorPresent ? AttachmentCount++ : -1;

    swapchain.iShadowmapAttachmentDepthLight = renderProcess.iShadowmapAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthLight = renderProcess.iMainSceneAttachmentDepthLight;
    swapchain.iMainSceneAttachmentDepthCamera = renderProcess.iMainSceneAttachmentDepthCamera;
    swapchain.iMainSceneAttachmentColorResovle = renderProcess.iMainSceneAttachmentColorResovle;
    swapchain.iMainSceneAttachmentColorPresent = renderProcess.iMainSceneAttachmentColorPresent;

    std::cout<<"Shadowmap attachments: "<<swapchain.iShadowmapAttachmentDepthLight<<std::endl;
    std::cout<<"Mainscene attachments: "<<swapchain.iMainSceneAttachmentDepthLight<<","<<swapchain.iMainSceneAttachmentDepthCamera<<","<<swapchain.iMainSceneAttachmentColorResovle<<","<<swapchain.iMainSceneAttachmentColorPresent<<std::endl;

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

    //std::cout<<"Application: Read Attachments Done."<<std::endl;
}

void Application::ReadSubpasses(){
    renderProcess.bEnableShadowmapRenderpassSubpassShadowmap = config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"] ? config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"].as<bool>() : false;
    renderProcess.bEnableMainSceneRenderpassSubpassShadowmap = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"].as<bool>() : false;
    renderProcess.bEnableMainSceneRenderpassSubpassDraw = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"].as<bool>() : true; //need at least one subpass, even for compute sample
    renderProcess.bEnableMainSceneRenderpassSubpassObserve = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"].as<bool>() : false;

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
    //create renderpass
    //std::cout<<"Application: Create MainScene Render Pass."<<std::endl;
    renderProcess.createSubpass_mainscene(appInfo.Feature.feature_graphics_observe_attachment_id);
    renderProcess.createDependency_mainscene();
    renderProcess.createRenderPass_mainscene();

    //create framebuffer
    //std::cout<<"Application: Create MainScene Framebuffer."<<std::endl;
    swapchain.CreateFramebuffer_mainscene(renderProcess.renderPass_mainscene);

    //std::cout<<"Application: Read Subpasses Done."<<std::endl;
}

void Application::CreateUniformDescriptors(bool b_uniform_graphics, bool b_uniform_compute){
    //UNIFORM STEP 1/3 (Pool)
    CGraphicsDescriptorManager::createDescriptorPool(objects.size()+textManager.m_textBoxes.size());//need size of both objects and textboxes, because each need a sampler
    CComputeDescriptorManager::createDescriptorPool();

    //UNIFORM STEP 2/3 (Layer)
    if(b_uniform_graphics){
        if(appInfo.Uniform.b_uniform_graphics_custom) 
             CGraphicsDescriptorManager::createDescriptorSetLayout_General(&appInfo.Uniform.GraphicsCustom.Binding); 
        else CGraphicsDescriptorManager::createDescriptorSetLayout_General(); 
        if(CGraphicsDescriptorManager::textureImageSamplers.size()>0) CGraphicsDescriptorManager::createDescriptorSetLayout_TextureImageSampler(); 
    }
    if(b_uniform_compute){
        if(appInfo.Uniform.b_uniform_compute_custom) CComputeDescriptorManager::createDescriptorSetLayout(&appInfo.Uniform.ComputeCustom.Binding);
        else CComputeDescriptorManager::createDescriptorSetLayout();
    }

    //UNIFORM STEP 3/3 (Set)
    if(b_uniform_graphics){
        graphicsDescriptorManager.createDescriptorSets_General(swapchain.buffer_depthcamera.view, swapchain.buffer_depthlight);
        //if(appInfo.Feature.feature_graphics_observe_attachment_id == 0) //assume 0 is light Depth Image Buffer
            //graphicsDescriptorManager.createDescriptorSets_General(swapchain.buffer_depthcamera.view, swapchain.buffer_depthlight[0].view, swapchain.buffer_depthlight[1].view); //TODO
           // graphicsDescriptorManager.createDescriptorSets_General(swapchain.depthImageBuffer.view, swapchain.lightDepthImageBuffer.view); //TODO
        //else// if(appInfo.Feature.feature_graphics_observe_attachment_id == 1)
            //graphicsDescriptorManager.createDescriptorSets_General(swapchain.depthImageBuffer.view);//TODO: what if no depthImageBuffer is not enable 
    }
    if(b_uniform_compute){
        if(appInfo.Uniform.b_uniform_compute_swapchain_storage) {
            if(appInfo.Uniform.b_uniform_compute_texture_storage)
                computeDescriptorManager.createDescriptorSets(&(textureManager.textureImages), &(swapchain.swapchain_views));//this must be called after texture resource is loaded
            else computeDescriptorManager.createDescriptorSets(NULL, &(swapchain.swapchain_views));
        }else computeDescriptorManager.createDescriptorSets();
    }
}

void Application::CreatePipelines(){
    bool bPipelineVerbose = false;

    /****************************
    * Command Buffer
    ****************************/
    if(appInfo.VertexShader != NULL) renderer.CreateGraphicsCommandBuffer();
    if(appInfo.ComputeShader != NULL) renderer.CreateComputeCommandBuffer();
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Command Buffer"<<std::endl;

    /****************************
    * Frame Buffer (legacy)
    ****************************/
    //if(appInfo.VertexShader != NULL){
        // VkImageLayout imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // renderProcess.enableColorAttachmentDescriptionColorPresent(swapchain.swapChainImageFormat);//assume when vertex is non-null, need a color attachment for presentation(must be single sampled)
        // if(swapchain.bEnableDepthTest) {
        //     renderProcess.enableAttachmentDescriptionDepth(swapchain.depthFormat, swapchain.msaaSamples);
        //     if(swapchain.bEnableMSAA) //if enable MSAA, must also enable depthTest
        //         renderProcess.enableAttachmentDescriptionColorMultiSample(swapchain.swapChainImageFormat, swapchain.msaaSamples, imageLayout); 
        // }
        // //renderProcess.createSubpass();
        // if(swapchain.bEnableDepthTest){
        //     VkPipelineStageFlags srcPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //     VkPipelineStageFlags dstPipelineStageFlag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //     renderProcess.createDependency(srcPipelineStageFlag, dstPipelineStageFlag);
        // }else renderProcess.createDependency();
        // renderProcess.createRenderPass();

        //swapchain.CreateFramebuffers(renderProcess.renderPass);
    //}
    //if(bVerbose) std::cout<<"CreatePipeline: Done Frame Buffer"<<std::endl;
    
    /****************************
    * Create Shaders
    ****************************/
    if(appInfo.VertexShader != NULL){
        for(int i = 0; i < appInfo.VertexShader->size(); i++){
            shaderManager.CreateShader((*appInfo.VertexShader)[i], shaderManager.VERT);
            shaderManager.CreateShader((*appInfo.FragmentShader)[i], shaderManager.FRAG);
        }
    }
    if(appInfo.ComputeShader != NULL)
        for(int i = 0; i < appInfo.ComputeShader->size(); i++)
            shaderManager.CreateShader((*appInfo.ComputeShader)[i], shaderManager.COMP);
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Shaders"<<std::endl;

    /****************************
    * Create Pipelines
    ****************************/
    if(appInfo.VertexShader != NULL){
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
        
        //std::cout<<"Begin create graphics pipeline"<<std::endl;
        for(int i = 0; i < appInfo.VertexShader->size(); i++){
            //std::cout<<"test create pipeline"<<std::endl;
            //! All graphics pipelines use the same dsLayouts
            if(shaderManager.bEnablePushConstant){
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create Push Constant Layout"<<std::endl;
                renderProcess.createGraphicsPipelineLayout(dsLayouts,  shaderManager.pushConstantRange, true, i);
                if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Push Constant Layout"<<std::endl;
            }
            else renderProcess.createGraphicsPipelineLayout(dsLayouts, i);

            
            int vertexDatatype = appInfo.VertexDatatype ? (*appInfo.VertexDatatype)[i] : 0;
            if(bPipelineVerbose) std::cout<<"CreatePipeline: Try Create graphics pipeline: "<<i<<", VertexStructureType="<<vertexDatatype<<std::endl;

            switch(vertexDatatype){
                case VertexStructureTypes::NoType:
                    renderProcess.createGraphicsPipeline(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], i,
                        (*appInfo.Subpass)[i], false, renderProcess.renderPass_mainscene,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);  
                break;
                case VertexStructureTypes::ThreeDimension:
                    //for 2-renderpass case, each pipeline for different renderpass
                    if((*appInfo.RenderPassShadowmap)[i]) {
                        renderProcess.createGraphicsPipeline<Vertex3D>(
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                            shaderManager.vertShaderModules[i], 
                            shaderManager.fragShaderModules[i], true, false, i,
                            (*appInfo.Subpass)[i], (*appInfo.RenderPassShadowmap)[i], renderProcess.renderPass_shadowmap,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);  
                    }else{
                        renderProcess.createGraphicsPipeline<Vertex3D>(
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                            shaderManager.vertShaderModules[i], 
                            shaderManager.fragShaderModules[i], true, false, i,
                            (*appInfo.Subpass)[i], (*appInfo.RenderPassShadowmap)[i], renderProcess.renderPass_mainscene,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);   
                    }   
                break;
                case VertexStructureTypes::TwoDimension:
                    std::cout<<"CreatePipeline: Create 2D pipeline"<<std::endl;
                    renderProcess.createGraphicsPipeline<Vertex2D>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, false, i,
                        (*appInfo.Subpass)[i], false, renderProcess.renderPass_mainscene,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);  
                    std::cout<<"CreatePipeline: Done Create 2D pipeline"<<std::endl;
                break;
                case VertexStructureTypes::ParticleType:
                    renderProcess.createGraphicsPipeline<Particle>(
                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, false, i,
                        (*appInfo.Subpass)[i], false, renderProcess.renderPass_mainscene,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);  
                break;
                case VertexStructureTypes::TextQuad:
                    renderProcess.createGraphicsPipeline<TextQuadVertex>(
                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                        shaderManager.vertShaderModules[i], 
                        shaderManager.fragShaderModules[i], true, true, i,
                        (*appInfo.Subpass)[i], (*appInfo.RenderPassShadowmap)[i], renderProcess.renderPass_mainscene,
                        (*appInfo.BlendEnable)[i],  (*appInfo.DepthTestEnable)[i], (*appInfo.DepthWriteEnable)[i], (*appInfo.SkyboxEnable)[i]);   
                break;
                default:
                break;
            }
        }
        //std::cout<<"Done create graphics pipeline"<<std::endl;
    }
    if(appInfo.ComputeShader != NULL){ //for now assume only one compute pipeline
        //! only support one compute pipeline
        renderProcess.createComputePipelineLayout(CComputeDescriptorManager::descriptorSetLayout);
        renderProcess.createComputePipeline(shaderManager.compShaderModules[0]);
    }
    if(bPipelineVerbose) std::cout<<"CreatePipeline: Done Create Pipelines"<<std::endl;
}

void Application::ReadRegisterObjects(){
    if (config["Objects"]) {
        //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
        for (const auto& obj : config["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            int resource_model_id = obj["resource_model_id"] ? obj["resource_model_id"].as<int>() : 0;
            auto resource_texture_id_list = obj["resource_texture_id_list"] ? obj["resource_texture_id_list"].as<std::vector<int>>() : std::vector<int>(1, 0);
            int resource_default_graphics_pipeline_id = obj["resource_default_graphics_pipeline_id"] ? obj["resource_default_graphics_pipeline_id"].as<int>() : 0;
            std::string name = obj["object_name"] ? obj["object_name"].as<std::string>() : "Default";
            bool bSticker = obj["object_sticker"] ? obj["object_sticker"].as<bool>() : false;
            float object_scale = obj["object_scale"] ? obj["object_scale"].as<float>() : 1.0f;
            auto object_scale_3 = obj["object_scale_3"] ? obj["object_scale_3"].as<std::vector<float>>() : std::vector<float>(3, object_scale);
            auto position = obj["object_position"] ? obj["object_position"].as<std::vector<float>>(): std::vector<float>(3, 0);
            auto rotation = obj["object_rotation"] ? obj["object_rotation"].as<std::vector<float>>(): std::vector<float>(3, 0);
            auto velocity = obj["object_velocity"] ? obj["object_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            auto angular_velocity = obj["object_angular_velocity"] ? obj["object_angular_velocity"].as<std::vector<float>>(): std::vector<float>(3, 0);
            //bool isSkybox = obj["object_skybox"] ? obj["object_skybox"].as<bool>() : false;

            objects[object_id].m_object_id = object_id;
            objects[object_id].m_model_id = resource_model_id;
            objects[object_id].m_texture_ids = resource_texture_id_list;
            objects[object_id].m_default_graphics_pipeline_id = resource_default_graphics_pipeline_id;
            objects[object_id].Name = name;
            objects[object_id].bSticker = bSticker;
            objects[object_id].SetPosition(position[0], position[1], position[2]);
            objects[object_id].SetRotation(rotation[0], rotation[1], rotation[2]);
            objects[object_id].SetVelocity(velocity[0], velocity[1], velocity[2]);
            objects[object_id].SetAngularVelocity(angular_velocity[0], angular_velocity[1], angular_velocity[2]);
            //objects[object_id].bSkybox = isSkybox;

            //must load resources before object register
            if(objects[object_id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Object id("<<object_id<<")!"<<std::endl;
                continue;
            }
            objects[object_id].Register((Application*)this);
            objects[object_id].SetScale(object_scale_3[0], object_scale_3[1], object_scale_3[2]);//set scale after model is registered, otherwise the length will not be computed correctly

            //std::cout<<"ObjectId:("<<object_id<<") Name:("<<objects[object_id].Name<<") Length:("<<objects[object_id].Length.x<<","<<objects[object_id].Length.y<<","<<objects[object_id].Length.z<<")"
            //    <<" Position:("<<objects[object_id].Position.x<<","<<objects[object_id].Position.y<<","<<objects[object_id].Position.z<<")"<<std::endl;
        }
    }

    //register objects for controls
    if(appInfo.Feature.feature_graphics_enable_controls){
        int indexOffset = customObjectSize;
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
    

}

void Application::ReadRegisterTextboxes(){
    //std::cout<<"Begin Read Textboxes"<<std::endl;
    if (config["Textboxes"]) {
        for (const auto& tb : config["Textboxes"]) {
            int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
            std::string name = tb["textbox_name"] ? tb["textbox_name"].as<std::string>() : "Default";
            auto position = tb["textbox_position"] ? tb["textbox_position"].as<std::vector<float>>(): std::vector<float>(3,0);
            glm::vec3 glm_position(position[0], position[1], position[2]);
            auto rotation = tb["textbox_rotation"] ? tb["textbox_rotation"] .as<std::vector<float>>(): std::vector<float>(3,0);
            glm::vec3 glm_rotation(rotation[0],rotation[1],rotation[2]);
            bool bSticker = tb["textbox_sticker"] ? tb["textbox_sticker"].as<bool>() : false;
            auto scale = tb["textbox_scale"] ? tb["textbox_scale"].as<float>() : 1.0f;
            auto color = tb["textbox_color"] ? tb["textbox_color"].as<std::vector<float>>(): std::vector<float>(4,1.0f);
            glm::vec4 glm_boxColor(color[0], color[1], color[2], color[3]);
            int resource_model_id = tb["resource_model_id"] ? tb["resource_model_id"].as<int>() : 0;
            //auto resource_text_id_list = tb["resource_text_id_list"] ? tb["resource_text_id_list"].as<std::vector<int>>() : std::vector<int>(1, 0); //recover later
            std::string text_content = tb["textbox_text_content"] ? tb["textbox_text_content"].as<std::string>() : "";
            auto text_color = tb["textbox_text_color"] ? tb["textbox_text_color"].as<std::vector<float>>() : std::vector<float>(4,1.0f);
            glm::vec4 glm_textColor(text_color[0], text_color[1], text_color[2], text_color[3]);
            int resource_default_graphics_pipeline_id = tb["resource_default_graphics_pipeline_id"] ? tb["resource_default_graphics_pipeline_id"].as<int>() : 0;

            textManager.m_textBoxes[textbox_id].Name = name;
            textManager.m_textBoxes[textbox_id].m_textBoxID = textbox_id;
            textManager.m_textBoxes[textbox_id].SetPosition(glm_position);
            textManager.m_textBoxes[textbox_id].SetRotation(glm_rotation);
            textManager.m_textBoxes[textbox_id].bSticker = bSticker;
            textManager.m_textBoxes[textbox_id].SetScale(scale);
            textManager.m_textBoxes[textbox_id].SetBoxColor(glm_boxColor);
            textManager.m_textBoxes[textbox_id].m_model_id = resource_model_id;
            textManager.m_textBoxes[textbox_id].m_text_content = text_content;
            textManager.m_textBoxes[textbox_id].SetTextColor(glm_textColor);
            textManager.m_textBoxes[textbox_id].m_default_graphics_pipeline_id = resource_default_graphics_pipeline_id;

            if(textManager.m_textBoxes[textbox_id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Textbox id("<<textbox_id<<")!"<<std::endl;
                continue;
            }
            textManager.m_textBoxes[textbox_id].Register((Application*)this);

            //std::cout<<"TextboxId:("<<id<<") Name:("<<textBoxes[id].GetName()<<") Position:("<<textBoxes[id].GetPosition().x<<","<<textBoxes[id].GetPosition().y<<","<<textBoxes[id].GetPosition().z<<")"<<std::endl;
        }
    }

    //register textbox for controls
    if(appInfo.Feature.feature_graphics_enable_controls){
        int indexOffset = customTextboxSize;
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
    
}

void Application::ReadLightings(){
    if (config["Lights"]) {
        for (const auto& light : config["Lights"]) {
            int id = light["light_id"] ? light["light_id"].as<int>() : 0;
            if(lights[id].bRegistered) {
                std::cout<<"WARNING: Trying to register a registered Light id("<<id<<")!"<<std::endl;
                continue;
            }
            
            std::string name = light["light_name"] ? light["light_name"].as<std::string>() : "Default";

            auto position = light["light_position"] ? light["light_position"].as<std::vector<float>>(): std::vector<float>(3,0);
            glm::vec3 glm_position(position[0], position[1], position[2]);

            auto intensity = light["light_intensity"] ? light["light_intensity"].as<std::vector<float>>(): std::vector<float>(4,0);

            auto color = light["light_color"] ? light["light_color"].as<std::vector<float>>(): std::vector<float>(3,1.0f);
            glm::vec3 glm_color(color[0], color[1], color[2]);

            auto spotAngle = light["light_spot"] ? light["light_spot"].as<std::vector<float>>() : std::vector<float>(2, 180.0f); //the default value is [180,180] degrees which sets the light to point light instead of spot light
            float spotInnerAngle = spotAngle[0];
            float spotOuterAngle = spotAngle[1];

            lights[id].Register(name, id, glm_position, intensity, glm_color, spotInnerAngle, spotOuterAngle);

            std::cout<<"LightId:("<<id<<") Name:("<<lights[id].GetLightName()<<") Intensity:("<<lights[id].GetIntensity(0)<<","<<lights[id].GetIntensity(1)<<","<<lights[id].GetIntensity(2)<<","<<lights[id].GetIntensity(3)<<")"
                <<" Position:("<<lights[id].GetLightPosition().x<<","<<lights[id].GetLightPosition().y<<","<<lights[id].GetLightPosition().z<<")"<<std::endl;
 
        }
        for(int i = 0; i < lights.size(); i++)
            if(!lights[i].bRegistered) std::cout<<"WARNING: Light id("<<i<<") is not registered!"<<std::endl;
    }
}

void Application::ReadCameras(){
    mainCamera.cameraType = (CameraType)(config["MainCamera"]["camera_mode"] ? config["MainCamera"]["camera_mode"].as<int>() : 0);
    mainCamera.SetPosition(
        config["MainCamera"]["camera_position"][0].as<float>(), 
        config["MainCamera"]["camera_position"][1].as<float>(), 
        config["MainCamera"]["camera_position"][2].as<float>());
    mainCamera.SetRotation(
        config["MainCamera"]["camera_rotation"][0].as<float>(), 
        config["MainCamera"]["camera_rotation"][1].as<float>(), 
        config["MainCamera"]["camera_rotation"][2].as<float>());
        
    mainCamera.focusObjectId = config["MainCamera"]["object_id_target"] ? config["MainCamera"]["object_id_target"].as<int>() : 0;

    mainCamera.bEnableOrthographic = config["MainCamera"]["camera_projection_enable_orthographic"] ? config["MainCamera"]["camera_projection_enable_orthographic"].as<bool>() : false;
    float nearPlane = config["MainCamera"]["camera_z"][0].as<float>();
    float farPlane = config["MainCamera"]["camera_z"][1].as<float>();

    if(!mainCamera.bEnableOrthographic){
        float fov = config["MainCamera"]["camera_projection_perspective_fov"] ? config["MainCamera"]["camera_projection_perspective_fov"].as<float>() : 90.0f;
        mainCamera.setPerspective(fov, 1.0f, nearPlane, farPlane);
    }else{
        float orthoWidth = config["MainCamera"]["camera_projection_orthographic_width"] ? config["MainCamera"]["camera_projection_orthographic_width"].as<float>() : 20.0f;
        float orthoHeight = config["MainCamera"]["camera_projection_orthographic_height"] ? config["MainCamera"]["camera_projection_orthographic_height"].as<float>() : 20.0f;
        mainCamera.setOrthographic(
            -orthoWidth / 2.0f, orthoWidth / 2.0f,
            -orthoHeight / 2.0f, orthoHeight / 2.0f,
            nearPlane, farPlane);
    }
    mainCamera.SetRotationSensitivity(200.0f);


    // mainCamera.SetTargetPosition(
    //     config["MainCamera"]["camera_target_position"][0].as<float>(), 
    //     config["MainCamera"]["camera_target_position"][1].as<float>(), 
    //     config["MainCamera"]["camera_target_position"][2].as<float>());

#ifdef SDL
    //sdlManager.keyboard_sensitive = config["MainCamera"]["camera_keyboard_sensitive"] ? config["MainCamera"]["camera_keyboard_sensitive"].as<float>() : 3;
    //sdlManager.mouse_sensitive = config["MainCamera"]["camera_mouse_sensitive"] ? config["MainCamera"]["camera_mouse_sensitive"].as<float>() : 60;
#else    
    glfwManager.keyboard_sensitive = config["MainCamera"]["camera_keyboard_sensitive"] ? config["MainCamera"]["camera_keyboard_sensitive"].as<float>() : 3;
    glfwManager.mouse_sensitive = config["MainCamera"]["camera_mouse_sensitive"] ? config["MainCamera"]["camera_mouse_sensitive"].as<float>() : 60;
#endif

    if (config["LightCamera"]) {
        lightCameras[0].cameraType = (CameraType)(config["LightCamera"]["camera_mode"] ? config["LightCamera"]["camera_mode"].as<int>() : 0);
        lightCameras[0].SetPosition(
            config["LightCamera"]["camera_position"][0].as<float>(), 
            config["LightCamera"]["camera_position"][1].as<float>(), 
            config["LightCamera"]["camera_position"][2].as<float>());
        lightCameras[0].SetRotation(
            config["LightCamera"]["camera_rotation"][0].as<float>(), 
            config["LightCamera"]["camera_rotation"][1].as<float>(), 
            config["LightCamera"]["camera_rotation"][2].as<float>());
        lightCameras[0].focusObjectId = config["LightCamera"]["object_id_target"] ? config["LightCamera"]["object_id_target"].as<int>() : 0;

        lightCameras[0].bEnableOrthographic = config["LightCamera"]["camera_projection_enable_orthographic"] ? config["LightCamera"]["camera_projection_enable_orthographic"].as<bool>() : false;
        float nearPlane = config["LightCamera"]["camera_z"][0].as<float>();
        float farPlane = config["LightCamera"]["camera_z"][1].as<float>();

        if(!lightCameras[0].bEnableOrthographic){
            float fov = config["LightCamera"]["camera_projection_perspective_fov"] ? config["LightCamera"]["camera_projection_perspective_fov"].as<float>() : 90.0f;
            lightCameras[0].setPerspective(fov, 1.0f, nearPlane, farPlane);
        }else{
            float orthoWidth = config["LightCamera"]["camera_projection_orthographic_width"] ? config["LightCamera"]["camera_projection_orthographic_width"].as<float>() : 20.0f;
            float orthoHeight = config["LightCamera"]["camera_projection_orthographic_height"] ? config["LightCamera"]["camera_projection_orthographic_height"].as<float>() : 20.0f;
            lightCameras[0].setOrthographic(
                -orthoWidth / 2.0f, orthoWidth / 2.0f,
                -orthoHeight / 2.0f, orthoHeight / 2.0f,
                nearPlane, farPlane);
        }
    }else{
        lightCameras[0].cameraType = mainCamera.cameraType; //default to main camera type
        lightCameras[0].SetPosition(mainCamera.Position);
        lightCameras[0].SetRotation(mainCamera.Rotation);
        lightCameras[0].setPerspective(mainCamera.fov,  (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, mainCamera.znear, mainCamera.zfar);
        lightCameras[0].focusObjectId = mainCamera.focusObjectId; //default to main camera focus object id
        lightCameras[0].bEnableOrthographic = mainCamera.bEnableOrthographic; //default to main camera orthographic mode
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

}

void Application::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    //CSupervisor::Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(computeDescriptorManager.descriptorSets);

    renderer.BindComputeDescriptorSets(renderProcess.computePipelineLayout, dsSets);

    //std::cout<<"Record Compute command buffer. "<<std::endl;
    renderer.Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}


void Application::LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName){
    handle = LoadLibraryA(moduleName.c_str()); 
    if(!handle) { 
        std::cerr << "Module load failed! Module Name = " << moduleName << std::endl; 
        return; 
    }

    using CreateInstanceFunc = void*(*)();
    auto CreateInstance =  (CreateInstanceFunc)GetProcAddress(handle, "CreateInstance");
    if(!CreateInstance) { 
        std::cerr << "GetProcAddress failed! (CreateInstance_Module) Module Name = " << moduleName << std::endl;
        FreeLibrary(handle);
        instance = nullptr;
        return;
    }
    
    instance = CreateInstance();
    if (!instance) {
        std::cerr << "CreateInstance failed!" << std::endl;
        FreeLibrary(handle);
        handle = nullptr;
        return;
    }

}

void Application::DestroyInstance(HMODULE handle, void* instance){
    if (instance) {
        using DestroyInstanceFunc = void(*)(void*);
        auto DestroyInstance =  (DestroyInstanceFunc)GetProcAddress(handle, "DestroyInstance");
        if(!DestroyInstance) { 
            std::cerr << "GetProcAddress failed! (DestroyInstance)" << std::endl;
            FreeLibrary(handle);
            return;
        }
        DestroyInstance(instance);
        instance = nullptr;
    }
}

Application::~Application(){
    CleanUp();

    if (handle_module_sdlcore) {
        //std::cout<<"- FreeLibrary: handle_module_sdlcore. (~Application())"<<std::endl;
        FreeLibrary(handle_module_sdlcore);
        handle_module_sdlcore = nullptr;
    }

    if (handle_module_game) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
        FreeLibrary(handle_module_game);
        handle_module_game = nullptr;
    }
}

    extern "C" void* CreateInstance(){ return new Application();}
    extern "C" void DestroyInstance(void *p){ 
        if(p) {
            static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_sdlcore,static_cast<Application*>(p)->instance_sdlcore);
            static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_game,static_cast<Application*>(p)->instance_game);
            delete static_cast<Application*>(p);
            //std::cout<<"- Destroy Instance Application."<<std::endl;
        } 
    }
}