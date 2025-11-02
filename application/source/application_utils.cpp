#include "../include/application.h"
#include <windows.h>

namespace LEApplication{
/**************
 * Helper Functions
 ************/
Application::Application(){
    //lightCameras.resize(2); //work
    lightCameras.resize(LIGHT_MAX); //TODO: for test purpose, create more cameras than needed

    //logManager.setLogFile("application.log");
}

std::string Application::GetPureName(const std::string& path) {
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

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
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
    //std::cout<<"Application::DestroyInstance()"<<std::endl;
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

void Application::CleanUp(){
    logger->CloseLogFile();
    //std::cout<<"Application Begin Cleanup()..."<<std::endl;

    //std::cout<<"Application: swapchain.CleanUp()"<<std::endl;
    renderer->SwapchainCleanup();
    //std::cout<<"Application: renderProcess.CleanUp()"<<std::endl;
    renderer->RenderProcessCleanup();

    //std::cout<<"Application: graphicsDescriptorManager.Destroy()"<<std::endl;
    //graphicsDescriptorManager.DestroyAndFree();
    renderer->GraphicsDescriptorManagerDestroyAndFree();
    //std::cout<<"Application: computeDescriptorManager.DestroyAndFree()"<<std::endl;
    renderer->ComputeDescriptorManagerDestroyAndFree();

    //std::cout<<"Application: textureManager.Destroy()"<<std::endl;
    textureManager.Destroy();
    textImageManager.Destroy();
    textManager.Destroy();

    //std::cout<<"Application: renderer begin Destroy()"<<std::endl;
    renderer->Destroy();
    //std::cout<<"Application: renderer end Destroy()"<<std::endl;

    //std::cout<<"Application: vkDestroyDevice()"<<std::endl;
    vkDestroyDevice(renderer->GetLogicalDevice(), nullptr);

    if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(renderer->GetInstance(), renderer->GetDebugMessenger(), nullptr);

    vkDestroySurfaceKHR(renderer->GetInstance(), renderer->GetSurface(), nullptr);
    vkDestroyInstance(renderer->GetInstance(), nullptr);
    
    renderer->ContextQuit();

    //std::cout<<"Application End Cleanup()."<<std::endl;
}

Application::~Application(){
    //std::cout<<"Application::~Application()"<<std::endl;

    if (handle_module_yamlcore) {
        //std::cout<<"- FreeLibrary: handle_module_yamlcore. (~Application())"<<std::endl;
        FreeLibrary(handle_module_yamlcore);
        handle_module_yamlcore = nullptr;
    }

    if (handle_module_sdlcore) {
        //std::cout<<"- FreeLibrary: handle_module_sdlcore. (~Application())"<<std::endl;
        FreeLibrary(handle_module_sdlcore);
        handle_module_sdlcore = nullptr;
    }

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

    if (handle_module_renderercore) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
        FreeLibrary(handle_module_renderercore);
        handle_module_renderercore = nullptr;
    }

    if (handle_module_logcore) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
        FreeLibrary(handle_module_logcore);
        handle_module_logcore = nullptr;
    }

    if (handle_module_resourcecore) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
        FreeLibrary(handle_module_resourcecore);
        handle_module_resourcecore = nullptr;
    }
}

extern "C" void* CreateInstance(){ return new Application();}
extern "C" void DestroyInstance(void *p){ 
    if(p) {
        static_cast<Application*>(p)->CleanUp();
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_yamlcore,static_cast<Application*>(p)->yamler);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_sdlcore,static_cast<Application*>(p)->sdler);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_game,static_cast<Application*>(p)->gamer);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_renderercore,static_cast<Application*>(p)->renderer);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_logcore, static_cast<Application*>(p)->logger);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_resourcecore, static_cast<Application*>(p)->resourcer);
        delete static_cast<Application*>(p);
        //std::cout<<"- Destroy Instance Application."<<std::endl;
    } 
}



bool Application::Get_feature_graphics_enable_controls()  {return yamler->GetAppInfo().Feature.feature_graphics_enable_controls;}
bool Application::Get_feature_graphics_show_all_metric_controls()  {return appInfo->Feature.feature_graphics_show_all_metric_controls;}
bool Application::Get_feature_graphics_show_performance_control()  {return appInfo->Feature.feature_graphics_show_performance_control;}
void Application::Set_feature_graphics_enable_controls(bool value)  {appInfo->Feature.feature_graphics_enable_controls = value;}
void Application::Set_feature_graphics_show_all_metric_controls(bool value) {appInfo->Feature.feature_graphics_show_all_metric_controls = value;}
void Application::Set_feature_graphics_show_performance_control(bool value) {appInfo->Feature.feature_graphics_show_performance_control = value;}

int Application::GetControlNodeSize() { return controlNodes.size();}
void Application::SetControlNodeVisible(int nodeId, bool value) { controlNodes[nodeId]->bVisible = value;}
void* Application::GetInstanceHandle() {return renderer->GetInstance();}// instance->getHandle();}

int Application::GetObjectSize() { return objects.size(); }
int Application::GetCustomObjectSize() { return appInfo->Objects.size(); }
void Application::SetObjectVelocity(int objectId, float vx, float vy, float vz) {objects[objectId].SetVelocity(vx, vy, vz);}
void Application::SetObjectVelocity(int objectId, glm::vec3 v) {objects[objectId].SetVelocity(v);}
void Application::SetObjectAngularVelocity(int objectId, float vx, float vy, float vz) {objects[objectId].SetAngularVelocity(vx, vy, vz); }
void Application::SetObjectPosition(int objectId, float px, float py, float pz) { objects[objectId].SetPosition(px, py, pz); }
void Application::SetObjectPosition(int objectId, glm::vec3 p) { objects[objectId].SetPosition(p); }
void Application::SetObjectScaleRectangleXY(int objectId, float x0, float y0, float x1, float y1) { objects[objectId].SetScaleRectangleXY(x0, y0, x1, y1); }
glm::vec3 Application::GetObjectPosition(int objectId) { return objects[objectId].Position; }

int Application::GetLightSize() { return lights.size(); }
glm::vec3 Application::GetLightPosition(int lightId) { return lights[lightId].GetLightPosition(); }
void Application::SetLightPosition(int lightId, float px, float py, float pz) { lights[lightId].SetLightPosition(glm::vec3(px, py, pz)); }
void Application::SetLightPosition(int lightId, glm::vec3 p) { lights[lightId].SetLightPosition(p); }

void Application::CreateCustomModel2D(std::vector<Vertex2D> &vertices2D) {modelManager.CreateCustomModel2D(vertices2D);}
void Application::CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage) {
    modelManager.CreateCustomModel3D(vertices3D, indices3D, isTextboxImage);
}

void Application::DrawObject(int objectId) { objects[objectId].Draw(); }
void Application::DrawTexts() { textManager.Draw(); }
void Application::DrawObjects() { for(int i = 0; i < objects.size(); i++) objects[i].Draw(); }
void Application::DrawObjects(int startObjectId, int endObjectId) { 
    for(int i = startObjectId; i <= endObjectId && i < objects.size(); i++)  objects[i].Draw(); 
}
void Application::DrawObject(int objectId, int pipelineId) { objects[objectId].Draw(pipelineId); }
void Application::DrawObject(int objectId, int pipelineId, int numVertex) { objects[objectId].Draw_NoIndexNoSet(pipelineId, numVertex); }
void Application::DrawParticlesFromStorageBuffer(int objectId, uint32_t particleCount) {
    objects[objectId].Draw(renderer->GetStorageBuffers(), -1, particleCount);
}

void Application::ComputeDispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
    Dispatch(numWorkGroupsX,numWorkGroupsY,numWorkGroupsZ);
}

void Application::SetComputeCustomSize(int size) { appInfo->Uniform.ComputeCustom.Size = size; }
void Application::SetComputeCustomBinding(void* binding) {
    VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
    if (bindingPtr) appInfo->Uniform.ComputeCustom.Binding = *bindingPtr;
}
void Application::UploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) {
    renderer->uploadComputeCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
}
void Application::SetComputeStorageBufferSize(int size) { appInfo->Uniform.ComputeStorageBuffer.Size = size; }
void Application::SetComputeStorageBufferUsage(int usage) {appInfo->Uniform.ComputeStorageBuffer.Usage = usage; }
void Application::UploadComputeStorageBuffer(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) {
    renderer->uploadStorageBuffer(currentFrame, storageBufferObject, dataSize);
}
void Application::DownloadComputeStorageBuffer(uint32_t currentFrame, void* storageBufferObject, int dataSize) {
    renderer->downloadStorageBuffer(currentFrame, storageBufferObject, dataSize);
}

void Application::SetGraphicsCustomSize(int size) { appInfo->Uniform.GraphicsCustom.Size = size; }
void Application::SetGraphicsCustomBinding(void* binding) {
    VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
    if (bindingPtr) appInfo->Uniform.GraphicsCustom.Binding = *bindingPtr;
}
void Application::UploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) {
    //graphicsDescriptorManager.uploadCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
    renderer->uploadGraphicsCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
}

void Application::SetMainCameraVelocityX(float value) { mainCamera.Velocity.x = value; }
void Application::SetMainCameraVelocityY(float value) { mainCamera.Velocity.y = value; }
void Application::SetMainCameraVelocityZ(float value) { mainCamera.Velocity.z = value; }
void Application::SetMainCameraAngularVelocityX(float value) { mainCamera.AngularVelocity.x = value; }
void Application::SetMainCameraAngularVelocityY(float value) { mainCamera.AngularVelocity.y = value; }
void Application::SetMainCameraAngularVelocityZ(float value) { mainCamera.AngularVelocity.z = value; }
void Application::SetMainCameraType(int type) { mainCamera.cameraType = (CameraType)type; }
int Application::GetMainCameraType() { return mainCamera.cameraType; }
void Application::SetMainCameraFocusObjectId(int objectId) { mainCamera.focusObjectId = objectId; }
int Application::GetMainCameraFocusObjectId() { return mainCamera.focusObjectId; }
void Application::MoveMainCameraLeft(float distance, float speed) { mainCamera.MoveLeft(distance, speed); }
void Application::MoveMainCameraRight(float distance, float speed) { mainCamera.MoveRight(distance, speed); }
void Application::MoveMainCameraForward(float distance, float speed) { mainCamera.MoveForward(distance, speed); }
void Application::MoveMainCameraBackward(float distance, float speed) { mainCamera.MoveBackward(distance, speed); }
glm::vec3 Application::GetMainCameraPosition() { return mainCamera.Position; }
void Application::SetMainCameraSensitivity(float sensitivity) { mainCamera.SetRotationSensitivity(sensitivity); }

void Application::SetLightCameraPosition(int lightCameraId, glm::vec3 p) { lightCameras[lightCameraId].SetPosition(p); }
void Application::SetLightCameraFocusObjectId(int lightCameraId, int objectId) { lightCameras[lightCameraId].focusObjectId = objectId; }
int Application::GetLightCameraFocusObjectId(int lightCameraId) { return lightCameras[lightCameraId].focusObjectId; }

void Application::LogContext(std::string s, float *n, int size) { logger->LogArray(s, n, size); }
void Application::LogContext(std::string s) { logger->Print(s); }
void Application::LogContext(std::string s, float n) { logger->Print(s, n); }
void Application::LogContext(std::string s, int n1, int n2) { logger->Log(s, n1, n2); }

void Application::SetRenderMode(int mode) { appInfo->RenderMode = (RenderModes)mode; }
void Application::SetPause(bool value) { NeedToPause = value; }
int Application::GetWindowWidth() { return windowWidth; }
int Application::GetWindowHeight() { return windowHeight; }
int Application::GetCurrentFrame() { return renderer->GetCurrentFrame();}
double Application::GetElapseTime() { return elapseTime;}
double Application::GetDeltaTime() { return deltaTime; }

void Application::CmdNextSubpass() { vkCmdNextSubpass(renderer->GetGraphicsCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE); }
void Application::SetSwapchainImageSize(int size) { renderer->SetSwapchain_ImageSize(size); }
void Application::EnableComputeSwapChainImage(bool enable) { renderer->SetSwapchain_Compute_Image(enable); }
void Application::DeviceWaitIdle() { vkDeviceWaitIdle(renderer->GetLogicalDevice()); }

void Application::PushConstantToCommand(void* pcData, int pipelineId) {
    renderer->PushConstantToCommand(pcData, renderer->GetGraphicsPipelineLayout(pipelineId), shaderManager.pushConstantRange);
}
void Application::CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    vkCmdSetDepthBias(renderer->GetGraphicsCommandBuffer(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void Application::CreateComputeCommandBuffers_DispatchForSwapchainImage(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
    std::vector<VkCommandBuffer> &commandBuffers = renderer->GetComputeCommandBuffers();// renderer.commandBuffers[renderer.computeCmdId];
    std::vector<VkImage> &swapChainImages = renderer->GetSwapchain_Images();

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        //renderer.currentFrame = i;
        renderer->SetCurrentFrame(i);
        //std::cout<<"commandbuffer i: "<<i<<std::endl;
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        //if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to begin recording command buffer!");
        //}
        //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
        renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());

        renderer->RecordImageBarrier(commandBuffers[i], swapChainImages[i],
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, //before write, expect layout to be VK_IMAGE_LAYOUT_GENERAL
            VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

        Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);

        renderer->RecordImageBarrier(commandBuffers[i], swapChainImages[i],
            VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, //before present, expect layout to be VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

        //if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to record command buffer!");
        //}
        renderer->EndRecordComputeCommandBuffer();
    }
    //renderer.currentFrame = 0;
    renderer->SetCurrentFrame(0);
}

}

