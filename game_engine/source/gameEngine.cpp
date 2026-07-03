#include "gameEngine.h"
#include "Foundation.h"
#include <locale>
#include <iomanip>
#include <iostream>
#include "bvhBuilder.h"

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
    LoadModuleAndInstance(handle_module_yamlcore, pVoid, "core_yaml.dll");
    yamler = static_cast<LEYAML::IYAMLCore*>(pVoid);
    appInfo = &yamler->GetAppInfo();
    
    //Load SDL Core Module
    LoadModuleAndInstance(handle_module_sdlcore, pVoid, "core_sdl.dll");
    sdler = static_cast<LESDL::ISDLCore*>(pVoid);
    sdler->SetApplication(this);

    //Load Renderer Core Module
    LoadModuleAndInstance(handle_module_renderercore, pVoid, "core_renderer.dll");
    renderer = static_cast<LERenderer::IRendererCore*>(pVoid);
    renderer->SetApplication(this); //also load log dll and create logger here

    //Load Log Core Module
    LoadModuleAndInstance(handle_module_logcore, pVoid, "core_log.dll");
    //logger = std::shared_ptr<LELog::ILogCore>(static_cast<LELog::ILogCore*>(pVoid));
    logger = static_cast<LELog::ILogCore*>(pVoid);

    //Load Resource Core Module
    LoadModuleAndInstance(handle_module_resourcecore, pVoid, "core_resource.dll");
    resourcer = static_cast<LEResource::IResourceCore*>(pVoid);
    resourcer->SetApplication(this, logger);

    exampleName = GetPureName(exampleName);
    //std::cout<<"exampleName: "<<exampleName<<std::endl;
    std::string logName = logger->GetLogFileName(exampleName);
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

    yamler->ReadExampleYAMLFile(exampleName);
    //bool bEnableGraphicsPipleine_raytarcing;
    bool bEnableComputePipeline_raytracing = appInfo->Uniform.b_storage_compute_bvhnode ? true : false;
    bool bEnableRaytracingPipeline_raytracing = appInfo->Uniform.b_uniform_raytracing_swapchain_storage ? true : false;
    //std::cout<<"bEnableComputePipeline_raytracing: "<<bEnableComputePipeline_raytracing<<std::endl;
    //std::cout<<"bEnableRaytracingPipeline_raytracing: "<<bEnableRaytracingPipeline_raytracing<<std::endl;

    /**************** 
    * Five steps with third-party(GLFW or SDL) initialization
    * Step 1: Create Window
    *****************/
    sdler->createWindow(OUT windowWidth, OUT windowHeight, exampleName);
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
    //VkQueueFlags requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT; //？
    const std::vector<const char*>  requireDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, //ray tracing TODO
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, //ray tracing TODO
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, //ray tracing TODO
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME //ray tracing TODO
    };

    // instance->findAllPhysicalDevices();

    // CContext::GetHandle().physicalDevice = instance->pickSuitablePhysicalDevice(renderer->GetSurface(), requireDeviceExtensions, requiredQueueFamilies);
    // //App dev can only query properties from physical device, but can not directly operate it
    // //App dev operates logical device, can logical device communicate with physical device by command queues
    // //App dev will fill command buffer with commands later
    // //instance->pickedPhysicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    // CContext::GetHandle().physicalDevice->get()->createLogicalDevices(surface, requiredValidationLayers, requireDeviceExtensions);
    renderer->CreatePhysicalDevice(requireDeviceExtensions, requiredQueueFamilies, requiredValidationLayers, bEnableRaytracingPipeline_raytracing); //also create logical device and command queues here
    
    // if (!LoadRayTracingFunctions(renderer->GetLogicalDevice())) {//ray tracing TODO, load function after logical device is created
    //     logger->Log("Failed to load ray tracing functions\n");
    // }
    //QueryRayTracingProperties();

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
    if(bEnableComputePipeline_raytracing) InitializeComputeRayTracing();
    Initialize();
    if(bEnableComputePipeline_raytracing) SetupComputeRayTracing();
    gamer->PostInitialize();

    if(renderer->GetRenderMode() == RenderModes::COMPUTE_SWAPCHAIN || renderer->GetRenderMode() == RenderModes::RAYTRACING_SWAPCHAIN)
        ConvertStorageImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    TimePoint T1 = now();
    totalInitTime = printElapsed("Application: Total Initialization cost", T0, T1);

    while(sdler->IsRunning()){
        sdler->eventHandle();
        if(!NeedToPause) {
            //std::cout<<"Frame "<<frameCount<<std::endl;
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
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle(), false); 

            vkResetCommandBuffer(renderer->GetGraphicsCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            renderer->StartRecordGraphicsCommandBuffer(
                renderer->GetRenderpass_mainscene(),
                renderer->GetSwapchain_FrameBuffers_Mainscene(),
                renderer->GetSwapchainExtent(),
                renderer->GetClearValues());
            RecordGraphicsCommandBuffer_RenderpassMainscene();
            renderer->EndRecordGraphicsCommandBuffer();

            //std::cout<<"DEBUG Application: Finished recording graphics command buffer. currentFrame = "<< renderer->GetCurrentFrame() <<std::endl;
            renderer->SubmitGraphics(false);
            //std::cout<<"DEBUG Application: Submitted graphics command buffer. currentFrame = "<< renderer->GetCurrentFrame() <<std::endl;

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle(), false);
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
        {
            //std::cout<<"COMPUTE_SWAPCHAIN: currentFrame: "<<renderer->GetCurrentFrame()<<std::endl;

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

            //std::cout<<"Application: Start recording compute command buffer for COMPUTE_SWAPCHAIN mode."<<std::endl;

            //!For swapchain, need convert layout before write stuff in swapchain images
            renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());
            
            //std::cout<<"Application: Record compute command buffer for COMPUTE_SWAPCHAIN mode."<<std::endl;

            //The following code was used to write directly into swapchain image(Vulkan 1.4 no longer recommends this)     
            // renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentFrame()],
            //     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, //before write, expect layout to be VK_IMAGE_LAYOUT_GENERAL
            //     VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
            //     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            //gamer->RecordComputeCommandBuffer();
            // renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentFrame()],
            //     VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, //before present, expect layout to be VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            //     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
            //     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

            gamer->RecordComputeCommandBuffer();

            //std::cout<<"Application: Finished recording compute command buffer for COMPUTE_SWAPCHAIN mode."<<std::endl;
            
            VkImageCopy copy{};
            copy.dstOffset = { 0,0,0 };
            copy.extent = { renderer->GetSwapchainExtent().width, renderer->GetSwapchainExtent().height, 1};
            copy.srcOffset = { 0,0,0 };

            VkImageSubresourceLayers subresource{};
            subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource.baseArrayLayer = 0;
            subresource.layerCount = 1;
            subresource.mipLevel = 0;
            copy.srcSubresource = subresource;
            copy.dstSubresource = subresource;

            //这条代码的唯一目的是： 告诉 Vulkan：接下来我要把 swapchain image 当作 TRANSFER 目标来写（vkCmdCopyImage），请保证它是可写的，并且 layout 正确
            //oldLayout = VK_IMAGE_LAYOUT_UNDEFINED: acquire 回来的 image 内容本来就是 undefined, 你马上会完整覆盖它
            //newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 这张 image 马上要作为 transfer 写入目标
            
            //accessMask 决定“等什么操作”, 资源访问类型（what）
            //srcAccessMask = 0: Vulkan 明确规定, 从 UNDEFINED 转换 layout 时，不需要等待任何之前的访问, 也就是说：不存在“之前的读/写”, 所以也就不存在要同步的 access
            //dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT: 后续的 transfer 写操作（vkCmdCopyImage）必须在 barrier 之后才能开始

            //stageMask 决定“等到哪一步”, 时间点（when）
            //srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT: 没有任何之前的 GPU stage 需要等。从时间轴最早点开始即可。这和 srcAccessMask = 0 是成对出现的。TOP_OF_PIPE ≈ “什么都没发生之前”。
            //dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT: Vulkan 会保证：在 transfer stage 执行 copy 之前, layout transition + 可写性已经完成

            //能不能只设置accessMask，不设置stageMask?不能
            //所以如果你只给 access：TRANSFER_WRITE
            //Vulkan 根本不知道：
            //是 copy？
            //是 blit？
            //是 clear？
            //是 resolve？
            //是什么时候发生？
            //必须用 stage 精确定位。

            //精确版一句话总结
            //这个 barrier 的作用是：在任何传输（TRANSFER）写操作开始之前，
            //保证该 image 已经完成 layout transition，并且不存在需要等待的旧访问。

            //它的真实 Vulkan 语义是：
            //把 swapchain image 直接“初始化”为：
            //接下来将在 TRANSFER 阶段被写入的目标。
            //不等待任何之前的访问。

            //barrier 隐含的前提是： swapchain image 的旧内容可以被丢弃

            renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(),  renderer->GetSwapchain_Images()[renderer->GetCurrentImage()],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                0, VK_ACCESS_TRANSFER_WRITE_BIT, //AccessMask
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT); //StageMask

            vkCmdCopyImage(renderer->GetComputeCommandBuffer(), renderer->GetIntermediaColor_Image(renderer->GetCurrentFrame()), VK_IMAGE_LAYOUT_GENERAL,
                renderer->GetSwapchain_Images()[renderer->GetCurrentImage()], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&copy);

            //std::cout<<"Application: Recorded vkCmdCopyImage to copy from intermedia color image to swapchain image."<<std::endl;

            //一句话精准总结（推荐记住这一句）
            //这个 barrier 的作用是：
            //在 swapchain image 的所有 transfer 写完成之后，
            //将它转换为可被呈现引擎读取的状态（PRESENT_SRC_KHR）。

            //oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 这个 image 刚刚作为 transfer 目标被写完
            //newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: 现在我要把它切换成可以 present 的 layout

            //srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT: 关心的是：刚才 transfer 阶段对它的写入。
            //dstAccessMask = 0: 接下来不是 Vulkan pipeline 要访问它，而是交给 presentation engine（WSI），所以不需要指定 Vulkan 的访问类型。

            //srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT: 这些写发生在 transfer 阶段。
            //dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT: 在所有 Vulkan pipeline 阶段结束之后，再把 image 交给 present

            //合起来的完整语义（规范版）
            //保证所有 transfer 阶段对 swapchain image 的写入完成，
            //并在 Vulkan pipeline 结束后，
            //将 image 安全地移交给 presentation engine 使用。

            //!VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT is working but is legacy
            //新的标准建议用VK_PIPELINE_STAGE_NONE，但是需要在device里添加sync2和features2支持

            renderer->RecordImageBarrier(renderer->GetComputeCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentImage()],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_TRANSFER_WRITE_BIT, 0, //AccessMask
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT); //StageMask, 
            

            renderer->EndRecordComputeCommandBuffer();

            renderer->SubmitCompute();

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle());
        break;
        }
        case RenderModes::RAYTRACING_SWAPCHAIN:
        {
            //std::cout<<"RAYTRACING_SWAPCHAIN: currentFrame: "<<renderer->GetCurrentFrame()<<std::endl;

            //must wait for fence before record
            renderer->WaitForRaytracingFence();
            //must aquire swap image before record command buffer
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle());
            
            //std::cout<<"Application: renderer.imageIndex = "<<renderer->GetCurrentImage()<<", currentFrame = "<<renderer->GetCurrentFrame()<<std::endl;
            
            vkResetCommandBuffer(renderer->GetRaytracingCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            //std::cout<<"Application: Start recording raytracing command buffer for RAYTRACING_SWAPCHAIN mode."<<std::endl;

            //!For swapchain, need convert layout before write stuff in swapchain images
            renderer->StartRecordRaytracingCommandBuffer(renderer->GetRaytracingPipeline(), renderer->GetRaytracingPipelineLayout());
            
            //std::cout<<"Application: Record raytracing command buffer for RAYTRACING_SWAPCHAIN mode."<<std::endl;

            gamer->RecordRaytracingCommandBuffer();

            //std::cout<<"Application: Finished recording raytracing command buffer for RAYTRACING_SWAPCHAIN mode."<<std::endl;
            
            VkImageCopy copy{};
            copy.dstOffset = { 0,0,0 };
            copy.extent = { renderer->GetSwapchainExtent().width, renderer->GetSwapchainExtent().height, 1};
            copy.srcOffset = { 0,0,0 };

            VkImageSubresourceLayers subresource{};
            subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresource.baseArrayLayer = 0;
            subresource.layerCount = 1;
            subresource.mipLevel = 0;
            copy.srcSubresource = subresource;
            copy.dstSubresource = subresource;
            
            renderer->RecordImageBarrier(renderer->GetRaytracingCommandBuffer(),  renderer->GetSwapchain_Images()[renderer->GetCurrentImage()],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                0, VK_ACCESS_TRANSFER_WRITE_BIT, //AccessMask
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT); //StageMask

            vkCmdCopyImage(renderer->GetRaytracingCommandBuffer(), renderer->GetIntermediaColor_Image(renderer->GetCurrentFrame()), VK_IMAGE_LAYOUT_GENERAL,
                renderer->GetSwapchain_Images()[renderer->GetCurrentImage()], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&copy);


            renderer->RecordImageBarrier(renderer->GetRaytracingCommandBuffer(), renderer->GetSwapchain_Images()[renderer->GetCurrentImage()],
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_TRANSFER_WRITE_BIT, 0, //AccessMask
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT); //StageMask, 
            

            renderer->EndRecordRaytracingCommandBuffer();

            renderer->SubmitRaytracing();

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle());
            //std::cout<<"Finished Presenting swapchain image for RAYTRACING_SWAPCHAIN mode."<<std::endl;

            //vkDeviceWaitIdle(renderer->GetLogicalDevice());//TODO: this line is only for debug
            //std::cout<<std::endl;
        break;
        }
        case RenderModes::COMPUTE_GRAPHICS:
        //case renderer.RENDER_COMPUTE_GRAPHICS_Mode:
            renderer->WaitForComputeFence();//must wait for fence before record
            renderer->WaitForGraphicsFence();//must wait for fence before record
            renderer->AquireSwapchainImage(renderer->GetSwapchainHandle(), false);//must aquire swap image before record command buffer

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
            
            renderer->SubmitCompute(false); 
            renderer->SubmitGraphics(false); 

            renderer->PresentSwapchainImage(renderer->GetSwapchainHandle(), false); 
        break;
        default:
        break;
    }
}

void GameEngine::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(renderer->GetComputeDescriptorSets());
    renderer->BindComputeDescriptorSets(renderer->GetComputePipelineLayout(), dsSets); 
    renderer->Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
}

}
