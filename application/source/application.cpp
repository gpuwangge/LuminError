#include "application.h"
#include "Foundation.h"
#include <locale>
#include <iomanip>
#include <iostream>
//#include "shaderManager.h"

namespace LEApplication{

void Application::Run(std::string exampleName){ //Entrance Function
    /**************** 
    * Module Related
    *****************/
    void* pVoid = nullptr;

    //Load YAML Core Module
    LoadModuleAndInstance(handle_module_yamlcore, pVoid, "yamlcore.dll");
    instance_yamlcore = static_cast<LEYAML::IYAMLCore*>(pVoid);
    appInfo = &instance_yamlcore->GetAppInfo();
    
    //Load SDL Core Module
    LoadModuleAndInstance(handle_module_sdlcore, pVoid, "sdlcore.dll");
    instance_sdlcore = static_cast<LESDL::ISDLCore*>(pVoid);
    instance_sdlcore->SetApplication(this);

    //Load Game(Example) Module
    LoadModuleAndInstance(handle_module_game, pVoid, exampleName);
    instance_game = static_cast<LuminError::IGame*>(pVoid);
    instance_game->SetApplication(this);

    //Load Renderer Core Module
    LoadModuleAndInstance(handle_module_renderercore, pVoid, "renderercore.dll");
    instance_renderercore = static_cast<LERenderer::IRendererCore*>(pVoid);
    instance_renderercore->SetApplication(this);

    CContext::Init();
    instance_game->PreInitialize();

    /**************** 
    * Five steps with third-party(GLFW or SDL) initialization
    * Step 1: Create Window
    *****************/
    m_sampleName = GetPureName(exampleName);
    instance_sdlcore->createWindow(OUT windowWidth, OUT windowHeight, m_sampleName);
	PRINT("run: Created Window. Window width = %d,  height = %d.", windowWidth, windowHeight);

    /**************** 
    * Step 2: Select required layers
    *****************/
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    
    /**************** 
    * Step 3: Select required instance extensions
    *****************/
    std::vector<const char*> requiredInstanceExtensions;
    instance_sdlcore->queryRequiredInstanceExtensions(OUT requiredInstanceExtensions);
    if(enableValidationLayers) requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    /**************** 
    * Step 4: create instance
    *****************/
    instance = std::make_unique<CInstance>(requiredValidationLayers, requiredInstanceExtensions);

    /**************** 
    * Step 5: create surface
    * Surface is to store view format information for creating swapchain. 
    * Only third party(glfw or sdl) knows what kind of surface can be attached to its window.
    *****************/
    instance_sdlcore->createSurface(IN instance, OUT surface);

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

    //query basic capabilities of surface
    //VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities;
    //std::cout<<vkGetPhysicalDeviceSurfaceCapabilitiesKHR(CContext::GetHandle().GetPhysicalDevice(), surface, pSurfaceCapabilities)<<std::endl;
    //std::cout<<"Surface min extent: width="<<pSurfaceCapabilities->minImageExtent.width<<", Surface min extent: height="<<pSurfaceCapabilities->minImageExtent.height<<std::endl;
    //std::cout<<"Surface max extent: width="<<pSurfaceCapabilities->maxImageExtent.width<<", Surface max extent: height="<<pSurfaceCapabilities->maxImageExtent.height<<std::endl;
    swapchain.createSwapchainImages(surface, windowWidth, windowHeight);
	swapchain.createSwapchainViews(VK_IMAGE_ASPECT_COLOR_BIT);

    instance_renderercore->CreateCommandPool(surface);

    std::cout<<"======================================="<<std::endl;
    std::cout<<"======Welcome to Vulkan Platform======="<<std::endl;
    std::cout<<"======================================="<<std::endl;

    TimePoint T0 = now();
    instance_game->Initialize();
    instance_yamlcore->ReadYAMLFile(m_sampleName);
    Initialize();
    instance_game->PostInitialize();

    TimePoint T1 = now();
    totalInitTime = printElapsed("Application: Total Initialization cost", T0, T1);

    while(instance_sdlcore->IsRunning()){
        instance_sdlcore->eventHandle();
        if(!NeedToPause) {
            instance_game->Update();
            Update();
            Record_Present();
            instance_game->PostUpdate();
            instance_renderercore->Update(); //update currentFrame
        }
        if(NeedToExit) break;
    }

	vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());//Wait GPU to complete all jobs before CPU destroy resources
}

void Application::Update(){
    static TimePoint startTimePoint = now();
    static TimePoint lastTimePoint = now();
    TimePoint currentTimePoint = now();
    elapseTime = secondsBetween(startTimePoint, currentTimePoint);
    deltaTime = secondsBetween(lastTimePoint, currentTimePoint);
    lastTimePoint = currentTimePoint;

    if(objects.size() > 0 && mainCamera.focusObjectId < objects.size())
        mainCamera.SetTargetPosition(objects[mainCamera.focusObjectId].Position);
    mainCamera.update(deltaTime);

    for(int i = 0; i < lights.size(); i++){//lightCameras.size()
        if(lights.size() > 0 && lightCameras[i].focusObjectId < objects.size())
            lightCameras[i].SetTargetPosition(objects[lightCameras[i].focusObjectId].Position);
        lightCameras[i].update(deltaTime);
    }

    for(int i = 0; i < objects.size(); i++) objects[i].Update(deltaTime, instance_renderercore->GetCurrentFrame(), mainCamera); 
    textManager.Update(deltaTime, instance_renderercore->GetCurrentFrame(), mainCamera);
    for(int i = 0; i < lights.size(); i++) lights[i].Update(deltaTime, instance_renderercore->GetCurrentFrame(), mainCamera, lightCameras[i]);
    if(appInfo->Feature.feature_graphics_enable_controls)
        for(int i = 0; i < controlNodes.size(); i++) controlNodes[i]->Update();

    frameCount++;
}

void Application::Record_Present(){
    /**************************
     * Universial Render Functions
     *************************/
    switch(instance_renderercore->GetRenderMode()){
        case RenderModes::GRAPHICS:
        //case renderer.RENDER_GRAPHICS_Mode:
            //std::cout<<"RENDER_GRAPHICS_Mode"<<std::endl;

            //must wait for fence before record command buffer
            instance_renderercore->WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            instance_renderercore->AquireSwapchainImage(swapchain.getHandle()); 

            vkResetCommandBuffer(instance_renderercore->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            instance_renderercore->StartRecordGraphicsCommandBuffer(
                instance_renderercore->GetRenderpass_mainscene(),
                swapchain.framebuffers_mainscene,swapchain.swapChainExtent, 
                instance_renderercore->GetClearValues());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            instance_renderercore->EndRecordGraphicsCommandBuffer();

            instance_renderercore->SubmitGraphics();

            instance_renderercore->PresentSwapchainImage(swapchain.getHandle());
        break;
        case RenderModes::GRAPHICS_SHADOWMAP:
            //must wait for fence before record command buffer
            instance_renderercore->WaitForGraphicsFence();
            //must aquire swap image before record command buffer
            instance_renderercore->AquireSwapchainImage(swapchain.getHandle()); 

            vkResetCommandBuffer(instance_renderercore->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            //renderer.BeginCommandBuffer(renderer.graphicsCmdId);
            instance_renderercore->BeginGraphicsCommandBuffer();

            for(int i = 0; i < swapchain.framebuffers_shadowmap.size(); i++){
                //std::cout<<"Application: Begin Shadowmap"<<i<<" Render Pass."<<std::endl;
                instance_renderercore->BeginRenderPass(instance_renderercore->GetRenderpass_shadowmap(), swapchain.framebuffers_shadowmap[i], swapchain.swapChainExtent, instance_renderercore->GetClearValues_shadowmap(), true);
                instance_renderercore->SetViewport(swapchain.swapChainExtent);
                instance_renderercore->SetScissor(swapchain.swapChainExtent);
                //RecordGraphicsCommandBuffer_RenderpassShadowmap(i);
                instance_game->RecordGraphicsCommandBuffer_RenderpassShadowmap(i);
                //renderer.EndRenderPass();
                instance_renderercore->EndGraphicsRenderPass();
            }

            //std::cout<<"Application: Begin Mainscene Render Pass."<<std::endl;
            instance_renderercore->BeginRenderPass(instance_renderercore->GetRenderpass_mainscene(), swapchain.framebuffers_mainscene, swapchain.swapChainExtent, instance_renderercore->GetClearValues(), false);
            instance_renderercore->SetViewport(swapchain.swapChainExtent);
            instance_renderercore->SetScissor(swapchain.swapChainExtent);
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            //renderer.EndRenderPass();
            instance_renderercore->EndGraphicsRenderPass();

	        //renderer.EndCommandBuffer(renderer.graphicsCmdId);
            instance_renderercore->EndGraphicsCommandBuffer();

            instance_renderercore->SubmitGraphics();

            instance_renderercore->PresentSwapchainImage(swapchain.getHandle());


        break;
        case RenderModes::COMPUTE:
        //case renderer.RENDER_COMPUTE_Mode:
            //std::cout<<"Application: RENDER_COMPUTE_Mode."<<std::endl;
            instance_renderercore->WaitForComputeFence();//must wait for fence before record
            //std::cout<<"Application: renderer.WaitForComputeFence()"<<std::endl;

            vkResetCommandBuffer(instance_renderercore->GetComputeCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            //std::cout<<"Application: vkResetCommandBuffer"<<std::endl;

            instance_renderercore->StartRecordComputeCommandBuffer(instance_renderercore->GetComputePipeline(), instance_renderercore->GetComputePipelineLayout());
            instance_game->RecordComputeCommandBuffer();
            instance_renderercore->EndRecordComputeCommandBuffer();
            //std::cout<<"Application: recordComputeCommandBuffer()"<<std::endl;

            instance_renderercore->SubmitCompute();
            //std::cout<<"Application: renderer.SubmitCompute()"<<std::endl;

           // renderer.PresentSwapchainImage(swapchain); //???
        break;
        case RenderModes::COMPUTE_SWAPCHAIN:
        //case renderer.RENDER_COMPUTE_SWAPCHAIN_Mode:
            //must wait for fence before record
            instance_renderercore->WaitForComputeFence();
            //must aquire swap image before record command buffer
            instance_renderercore->AquireSwapchainImage(swapchain.getHandle());
            //std::cout<<"Application: renderer.imageIndex = "<<renderer.imageIndex<< std::endl;
            //std::cout<<"Application: renderer.currentFrame = "<<renderer.currentFrame<< std::endl;

            //vkResetCommandBuffer(renderer.commandBuffers[renderer.computeCmdId][renderer.currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

            //in this mode, nothing is recorded(all commands are pre-recorded), for NOW. But still, swapchain will be presented.
            //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
            //recordComputeCommandBuffer();
            //renderer.EndRecordComputeCommandBuffer();

            instance_renderercore->SubmitCompute(); 

            instance_renderercore->PresentSwapchainImage(swapchain.getHandle()); 
        break;
        case RenderModes::COMPUTE_GRAPHICS:
        //case renderer.RENDER_COMPUTE_GRAPHICS_Mode:
            instance_renderercore->WaitForComputeFence();//must wait for fence before record
            instance_renderercore->WaitForGraphicsFence();//must wait for fence before record
            instance_renderercore->AquireSwapchainImage(swapchain.getHandle());//must aquire swap image before record command buffer

            vkResetCommandBuffer(instance_renderercore->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            vkResetCommandBuffer(instance_renderercore->GetComputeCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);
            
            instance_renderercore->StartRecordComputeCommandBuffer(instance_renderercore->GetComputePipeline(), instance_renderercore->GetComputePipelineLayout());
            instance_game->RecordComputeCommandBuffer();
            instance_renderercore->EndRecordComputeCommandBuffer();

            instance_renderercore->StartRecordGraphicsCommandBuffer(
                instance_renderercore->GetRenderpass_mainscene(),
                swapchain.framebuffers_mainscene, swapchain.swapChainExtent,
                instance_renderercore->GetClearValues());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            instance_renderercore->EndRecordGraphicsCommandBuffer();
            
            instance_renderercore->SubmitCompute(); 
            instance_renderercore->SubmitGraphics(); 

            instance_renderercore->PresentSwapchainImage(swapchain.getHandle()); 
        break;
        default:
        break;
    }
}

void Application::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(computeDescriptorManager.descriptorSets);
    instance_renderercore->BindComputeDescriptorSets(instance_renderercore->GetComputePipelineLayout(), dsSets);
    instance_renderercore->Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}

}