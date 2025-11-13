#include "gameEngine.h"
#include "Foundation.h"
#include <locale>
#include <iomanip>
#include <iostream>
//#include "shaderManager.h"

namespace LEGameEngine{

void GameEngine::Run(std::string exampleName){ //Entrance Function
    //std::cout<<"Run "<<exampleName<<std::endl;
    std::cout<<"======================================="<<std::endl;
    std::cout<<"======Welcome to Vulkan Platform======="<<std::endl;
    std::cout<<"======================================="<<std::endl;

    /**************** 
    * Module Related
    *****************/
    void* pVoid = nullptr;

    //Load YAML Core Module
    LoadModuleAndInstance(handle_module_yamlcore, pVoid, "yamlcore.dll");
    yamler = static_cast<LEYAML::IYAMLCore*>(pVoid);
    appInfo = &yamler->GetAppInfo();
    
    //Load SDL Core Module
    LoadModuleAndInstance(handle_module_sdlcore, pVoid, "sdlcore.dll");
    sdler = static_cast<LESDL::ISDLCore*>(pVoid);
    sdler->SetApplication(this);

    //Load Renderer Core Module
    LoadModuleAndInstance(handle_module_renderercore, pVoid, "core_renderer.dll");
    renderer = static_cast<LERenderer::IRendererCore*>(pVoid);
    renderer->SetApplication(this); //also load log dll and create logger here

    //Load Log Core Module
    LoadModuleAndInstance(handle_module_logcore, pVoid, "logcore.dll");
    //logger = std::shared_ptr<LELog::ILogCore>(static_cast<LELog::ILogCore*>(pVoid));
    logger = static_cast<LELog::ILogCore*>(pVoid);

    //Load Resource Core Module
    LoadModuleAndInstance(handle_module_resourcecore, pVoid, "resourcecore.dll");
    resourcer = static_cast<LEResource::IResourceCore*>(pVoid);
    resourcer->SetApplication(this, logger);

    m_sampleName = GetPureName(exampleName);
    //std::cout<<"exampleName: "<<exampleName<<std::endl;
    std::string logName = logger->GetLogFileName(m_sampleName);
    std::string folderPath = logger->CreateDateFolder(LOG_PATH);
    std::string fullLogName = folderPath + "/" + logName;
    //std::cout<<"fullLogName: "<<fullLogName<<std::endl;
    //mkdir(LOG_PATH);
    logger->SetLogFile(fullLogName);
    // logger->Print("Application started");// 演示所有用例
    // logger->Print("Integer: {}", 42); // 基本类型
    // logger->Print("Float: {}", 3.14f);
    // logger->Print("String: {}", "Hello");
    // logger->Print("Multiple: {}, {}, {}", 1, "test", 2.5f);// 多个参数
    //glm::vec3 vec(1.0f, 2.0f, 3.0f);// glm 向量
    //logger->Print("Vector: ({}, {}, {})", vec.x, vec.y, vec.z);
    //int numbers[] = {10, 20, 30};// 数组
    //logger->Print("Array: {}, {}, {}", numbers[0], numbers[1], numbers[2]);
    // logger->Log("Log Application started");
    // logger->Log("Log Integer: {}", 42); // 基本类型
    // logger->Log("Log Float: {}", 3.14f);
    // logger->Log("Log String: {}", "Hello");
    // logger->Log("Log Multiple: {}, {}, {}", 1, "test", 2.5f);// 多个参数
    // logger->Log("Log Vector: ({}, {}, {})", vec.x, vec.y, vec.z);
    // logger->Log("Log Array: {}, {}, {}", numbers[0], numbers[1], numbers[2]);

    gamer->PreInitialize();

    /**************** 
    * Five steps with third-party(GLFW or SDL) initialization
    * Step 1: Create Window
    *****************/
    sdler->createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
	//PRINT("run: Created Window. Window width = %d,  height = %d.", windowWidth, windowHeight);

    /**************** 
    * Step 2: Select required layers
    *****************/
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    
    /**************** 
    * Step 3: Select required instance extensions
    *****************/
    std::vector<const char*> requiredInstanceExtensions;
    sdler->queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
    if(enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    /**************** 
    * Step 4: create instance
    *****************/
    //instance = std::make_unique<CInstance>(requiredValidationLayers, requiredInstanceExtensions, logger);
    renderer->CreateInstance(requiredValidationLayers, requiredInstanceExtensions);

    /**************** 
    * Step 5: create surface
    * Surface is to store view format information for creating swapchain. 
    * Only third party(glfw or sdl) knows what kind of surface can be attached to its window.
    *****************/
    sdler->createSurface(IN renderer->GetInstance(), OUT renderer->GetSurface());

    /**************** 
    * General initialization begins
    * Select required queue families
    * Select required device extensions
    *****************/
    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //& VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // instance->findAllPhysicalDevices();

    // CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(renderer->GetSurface(), requireDeviceExtensions, requiredQueueFamilies);
    // //App dev can only query properties from physical device, but can not directly operate it
    // //App dev operates logical device, can logical device communicate with physical device by command queues
    // //App dev will fill command buffer with commands later
    // //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    // CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    renderer->CreatePhysicalDevice(requireDeviceExtensions, requiredQueueFamilies, requiredValidationLayers);
    

    // textureManager.m_logicalDevice = renderer->GetLogicalDevice();
    // textureManager.m_physicalDevice = renderer->GetPhysicalDevice();
    // textureManager.m_graphicsQueue = renderer->GetGraphicsQueue();
    //textureManager.SetLogger(logger);
    textManager.m_logicalDevice = renderer->GetLogicalDevice();
    textManager.m_physicalDevice = renderer->GetPhysicalDevice();
    textManager.m_graphicsQueue = renderer->GetGraphicsQueue();
    //shaderManager.m_logicalDevice = renderer->GetLogicalDevice();
    resourcer->SetDevice(renderer->GetLogicalDevice(), renderer->GetPhysicalDevice(), renderer->GetGraphicsQueue());

    //query basic capabilities of surface
    //VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities;
    //std::cout<<vkGetPhysicalDeviceSurfaceCapabilitiesKHR(CContext::GetHandle().GetPhysicalDevice(), surface, pSurfaceCapabilities)<<std::endl;
    //std::cout<<"Surface min extent: width="<<pSurfaceCapabilities->minImageExtent.width<<", Surface min extent: height="<<pSurfaceCapabilities->minImageExtent.height<<std::endl;
    //std::cout<<"Surface max extent: width="<<pSurfaceCapabilities->maxImageExtent.width<<", Surface max extent: height="<<pSurfaceCapabilities->maxImageExtent.height<<std::endl;
    renderer->CreateSwapchainImages(renderer->GetSurface(), windowWidth, windowHeight);
    renderer->CreateSwapchainViews(VK_IMAGE_ASPECT_COLOR_BIT);

    renderer->CreateCommandPool(renderer->GetSurface());

    std::cout<<"======Begin Initialization======="<<std::endl;

    TimePoint T0 = now();
    gamer->Initialize();
    yamler->ReadYAMLFile(m_sampleName);
    Initialize();
    gamer->PostInitialize();

    TimePoint T1 = now();
    totalInitTime = printElapsed("Application: Total Initialization cost", T0, T1);

    while(sdler->IsRunning()){
        sdler->eventHandle();
        if(!NeedToPause) {
            gamer->Update();
            Update();
            Record_Present();
            gamer->PostUpdate();
            renderer->Update(); //update currentFrame
        }
        if(NeedToExit) break;
    }

	vkDeviceWaitIdle(renderer->GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}

void GameEngine::Update(){
    static TimePoint startTimePoint = now();
    static TimePoint lastTimePoint = now();
    TimePoint currentTimePoint = now();
    elapseTime = secondsBetween(startTimePoint, currentTimePoint);
    deltaTime = secondsBetween(lastTimePoint, currentTimePoint);
    lastTimePoint = currentTimePoint;

    if(objects.size() > 0 && mainCamera.focusObjectId < objects.size())
        mainCamera.SetTargetPosition(objects[mainCamera.focusObjectId].Position);
    mainCamera.update(deltaTime);

    //global ubo must be uploaded after camera, and before object
    StructGraphicsGlobalUniformBuffer graphicsGlobalUniformBufferObject{};
    graphicsGlobalUniformBufferObject.mainCameraView = mainCamera.matrices.view;
    graphicsGlobalUniformBufferObject.mainCameraViewInverse = glm::inverse(mainCamera.matrices.view);
    graphicsGlobalUniformBufferObject.mainCameraProj = mainCamera.matrices.projection;
    graphicsGlobalUniformBufferObject.mainCameraProjInverse = glm::inverse(mainCamera.matrices.projection);
    graphicsGlobalUniformBufferObject.mainCameraPos = mainCamera.Position;
    graphicsGlobalUniformBufferObject.aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    float fovY = glm::radians(mainCamera.fov);
    graphicsGlobalUniformBufferObject.tanHalfFovY = tan(fovY / 2.0f);
    renderer->uploadGraphicsGlobalUniformBuffer(renderer->GetCurrentFrame(), &graphicsGlobalUniformBufferObject, sizeof(StructGraphicsGlobalUniformBuffer));

    StructComputeGlobalUniformBuffer computeGlobalUniformBufferObject{};
    computeGlobalUniformBufferObject.mainCameraView = mainCamera.matrices.view;
    computeGlobalUniformBufferObject.mainCameraViewInverse = glm::inverse(mainCamera.matrices.view);
    computeGlobalUniformBufferObject.mainCameraProj = mainCamera.matrices.projection;
    computeGlobalUniformBufferObject.mainCameraProjInverse = glm::inverse(mainCamera.matrices.projection);
    computeGlobalUniformBufferObject.mainCameraPos = mainCamera.Position;
    computeGlobalUniformBufferObject.aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    fovY = glm::radians(mainCamera.fov);
    computeGlobalUniformBufferObject.tanHalfFovY = tan(fovY / 2.0f);
    renderer->uploadComputeGlobalUniformBuffer(renderer->GetCurrentFrame(), &computeGlobalUniformBufferObject, sizeof(StructComputeGlobalUniformBuffer));


    for(int i = 0; i < lights.size(); i++){//lightCameras.size()
        if(lights.size() > 0 && lightCameras[i].focusObjectId < objects.size())
            lightCameras[i].SetTargetPosition(objects[lightCameras[i].focusObjectId].Position);
        lightCameras[i].update(deltaTime);
    }

    for(int i = 0; i < objects.size(); i++) objects[i].Update(deltaTime, renderer->GetCurrentFrame(), mainCamera); 
    textManager.Update(deltaTime, renderer->GetCurrentFrame(), mainCamera);
    for(int i = 0; i < lights.size(); i++) lights[i].Update(deltaTime, renderer->GetCurrentFrame(), mainCamera, lightCameras[i]);
    if(appInfo->Feature.feature_graphics_enable_controls)
        for(int i = 0; i < controlNodes.size(); i++) controlNodes[i]->Update();

    frameCount++;
}

void GameEngine::Record_Present(){
    /**************************
     * Universial Render Functions
     *************************/
    switch(renderer->GetRenderMode()){
        case RenderModes::GRAPHICS:
        //case renderer.RENDER_GRAPHICS_Mode:
            //std::cout<<"RENDER_GRAPHICS_Mode"<<std::endl;

            //must wait for fence before record command buffer
            renderer->WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle()); 

            vkResetCommandBuffer(renderer->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            renderer->StartRecordGraphicsCommandBuffer(
                renderer->GetRenderpass_mainscene(),
                renderer->GetSwapchain_FrameBuffers_Mainscene(),
                renderer->GetSwapchainExtent(),
                renderer->GetClearValues());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer->EndRecordGraphicsCommandBuffer();

            renderer->SubmitGraphics();

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle());
        break;
        case RenderModes::GRAPHICS_SHADOWMAP:
            //must wait for fence before record command buffer
            renderer->WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle()); 

            vkResetCommandBuffer(renderer->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            //renderer.BeginCommandBuffer(renderer.graphicsCmdId);
            renderer->BeginGraphicsCommandBuffer();

            for(int i = 0; i < renderer->GetSwapchain_FrameBuffersSize_Shadowmap(); i++){
                //std::cout<<"Application: Begin Shadowmap"<<i<<" Render Pass."<<std::endl;
                renderer->BeginRenderPass(renderer->GetRenderpass_shadowmap(), renderer->GetSwapchain_FrameBuffer_Shadowmap(i), renderer->GetSwapchainExtent(), renderer->GetClearValues_shadowmap(), true);
                renderer->SetViewport(renderer->GetSwapchainExtent());
                renderer->SetScissor(renderer->GetSwapchainExtent());
                //RecordGraphicsCommandBuffer_RenderpassShadowmap(i);
                gamer->RecordGraphicsCommandBuffer_RenderpassShadowmap(i);
                //renderer.EndRenderPass();
                renderer->EndGraphicsRenderPass();
            }

            //std::cout<<"Application: Begin Mainscene Render Pass."<<std::endl;
            renderer->BeginRenderPass(renderer->GetRenderpass_mainscene(), renderer->GetSwapchain_FrameBuffers_Mainscene(), renderer->GetSwapchainExtent(), renderer->GetClearValues(), false);
            renderer->SetViewport(renderer->GetSwapchainExtent());
            renderer->SetScissor(renderer->GetSwapchainExtent());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            //renderer.EndRenderPass();
            renderer->EndGraphicsRenderPass();

	        //renderer.EndCommandBuffer(renderer.graphicsCmdId);
            renderer->EndGraphicsCommandBuffer();

            renderer->SubmitGraphics();

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle());


        break;
        case RenderModes::COMPUTE:
        //case renderer.RENDER_COMPUTE_Mode:
            //std::cout<<"Application: RENDER_COMPUTE_Mode."<<std::endl;
            renderer->WaitForComputeFence();//must wait for fence before record
            //std::cout<<"Application: renderer.WaitForComputeFence()"<<std::endl;

            vkResetCommandBuffer(renderer->GetComputeCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            //std::cout<<"Application: vkResetCommandBuffer"<<std::endl;

            renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());
            gamer->RecordComputeCommandBuffer();
            renderer->EndRecordComputeCommandBuffer();
            //std::cout<<"Application: recordComputeCommandBuffer()"<<std::endl;

            renderer->SubmitCompute();
            //std::cout<<"Application: renderer.SubmitCompute()"<<std::endl;

           // renderer.PresentSwapchainImage(swapchain); //???
        break;
        case RenderModes::COMPUTE_SWAPCHAIN:
        //case renderer.RENDER_COMPUTE_SWAPCHAIN_Mode:
            //must wait for fence before record
            renderer->WaitForComputeFence();
            //must aquire swap image before record command buffer
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle());
            //std::cout<<"Application: renderer.imageIndex = "<<renderer.imageIndex<< std::endl;
            //std::cout<<"Application: renderer.currentFrame = "<<renderer.currentFrame<< std::endl;

            //vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
            vkResetCommandBuffer(renderer->GetComputeCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            //in this mode, nothing is recorded(all commands are pre-recorded), for NOW. But still, swapchain will be presented.
            //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            //recordComputeCommandBuffer();
            //renderer.EndRecordComputeCommandBuffer();

            //!For swapchain, need convert layout before write stuff in swapchain images
            renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());
            renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentFrame()],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, //before write, expect layout to be VK_IMAGE_LAYOUT_GENERAL
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            gamer->RecordComputeCommandBuffer();
            renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentFrame()],
                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, //before present, expect layout to be VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
            renderer->EndRecordComputeCommandBuffer();

            renderer->SubmitCompute(); 

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle()); 
        break;
        case RenderModes::COMPUTE_GRAPHICS:
        //case renderer.RENDER_COMPUTE_GRAPHICS_Mode:
            renderer->WaitForComputeFence();//must wait for fence before record
            renderer->WaitForGraphicsFence();//must wait for fence before record
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle());//must aquire swap image before record command buffer

            vkResetCommandBuffer(renderer->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            vkResetCommandBuffer(renderer->GetComputeCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            
            renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());
            gamer->RecordComputeCommandBuffer();
            renderer->EndRecordComputeCommandBuffer();

            renderer->StartRecordGraphicsCommandBuffer(
                renderer->GetRenderpass_mainscene(),
                renderer->GetSwapchain_FrameBuffers_Mainscene(), renderer->GetSwapchainExtent(),
                renderer->GetClearValues());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer->EndRecordGraphicsCommandBuffer();
            
            renderer->SubmitCompute(); 
            renderer->SubmitGraphics(); 

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle()); 
        break;
        default:
        break;
    }
}

void GameEngine::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(renderer->GetDescriptorSets());
    renderer->BindComputeDescriptorSets(renderer->GetComputePipelineLayout(), dsSets);
    renderer->Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}

}