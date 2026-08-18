#include "gameEngine.h"
#include <windows.h>

namespace LEGameEngine{
/**************
 * Helper Functions
 ************/
GameEngine::GameEngine(){
    //lightCameras.resize(2); //work
    lightCameras.resize(LIGHT_MAX); //TODO: for test purpose, create more cameras than needed

    //logManager.setLogFile("application.log");
}

std::string GameEngine::GetPureName(const std::string& path) {
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

void GameEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void GameEngine::LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName){
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

void GameEngine::DestroyInstance(HMODULE handle, void* instance){
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

void GameEngine::CleanUp(){
    logger->CloseLogFile();

    for(int i = 0; i < rtMeshes.size(); i++){
        rtMeshes[i].vertexBuffer.DestroyAndFree(renderer->GetLogicalDevice());
        rtMeshes[i].indexBuffer.DestroyAndFree(renderer->GetLogicalDevice());
        rtMeshes[i].blasBuffer.DestroyAndFree(renderer->GetLogicalDevice());
    }
    //for(int i = 0; i < rtSpheres.size(); i++){
        //rtSpheres[i].vertexBuffer.DestroyAndFree(renderer->GetLogicalDevice());
        //rtSpheres[i].indexBuffer.DestroyAndFree(renderer->GetLogicalDevice());
    //    rtSpheres[i].blasBuffer.DestroyAndFree(renderer->GetLogicalDevice());
    //}
    //raytracing_vertex_buffer.DestroyAndFree(renderer->GetLogicalDevice());
    //raytracing_index_buffer.DestroyAndFree(renderer->GetLogicalDevice());

    /*********************
    *1 VkDevice Resources
    ********************/

    /*********************
    *1.1 Swapchain
    * For Framebuffer(shadowmap, mainscene)
    *   vkDestroyFramebuffer
    * For swapchain imageviews
    *   vkDestroyImageView
    * 
    *   vkDestroySwapchainKHR
    *
    * For buffer(buffer_depthlight, buffer_depthcamera, buffer_colorresolve, textureimages, textimage textureimages):
    *   vkDestroyImage
    *   vkFreeMemory
    *   vkDestroyImageView
    * 
    * For buffer(instanceDataBuffer)
    *   vkDestroyBuffer
    *   vkFreeMemory
    ********************/
    //std::cout<<"Application: swapchain.CleanUp()"<<std::endl;
    renderer->SwapchainCleanup();
    //std::cout<<"Application: renderProcess.CleanUp()"<<std::endl;

    //std::cout<<"Application: textureManager.Destroy()"<<std::endl;
    resourcer->DestroyTextureManager();
    resourcer->DestroyTextImageManager();
    textManager.Destroy();

    /*********************
    *1.2 Pipeline
    * For Renderpass(Shadowmap, mainScene)
    *   vkDestroyRenderPass
    * For Pipelines (Graphics, Compute)
    *   vkDestroyPipeline
    *   vkDestroyPipelineLayout
    ********************/
    renderer->RenderProcessCleanup();

    /*********************
    *1.3 Descriptor
    *   vkDestroySampler
    *   vkDestroyDescriptorPool
    *   vkDestroyDescriptorSetLayout
    * 
    * For buffers(objectUniformBuffers, textUniformBuffers, customUniformBuffers, m_lightingUniformBuffers)
    *   vkDestroyBuffer
    *   vkFreeMemory 
    ********************/
    renderer->GraphicsDescriptorManagerDestroyAndFree();
    renderer->ComputeDescriptorManagerDestroyAndFree();
    renderer->RaytracingDescriptorManagerDestroyAndFree();

    /*********************
    *1.4 Command Pool
    * For buffers(vertexDataBuffer, indexDataBuffer)
    *   vkDestroyBuffer
    *   vkFreeMemory
    * 
    *   vkDestroySemaphore
    *   vkDestroyFence
    *   vkDestroyCommandPool
    ********************/
    renderer->Destroy();

    /*********************
    *2 VkDevice
    ********************/
    vkDestroyDevice(renderer->GetLogicalDevice(), nullptr); 

    /*********************
    *3 Surface
    ********************/
    vkDestroySurfaceKHR(renderer->GetInstance(), renderer->GetSurface(), nullptr);

    /*********************
    *4 Debug Messenger
    ********************/
    if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(renderer->GetInstance(), renderer->GetDebugMessenger(), nullptr);

    /*********************
    *5 VkInstance
    ********************/
    vkDestroyInstance(renderer->GetInstance(), nullptr);
    
    /*********************
    *6 Context
    ********************/
    renderer->ContextQuit();


}

GameEngine::~GameEngine(){
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

    //if (handle_module_game) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
    //    FreeLibrary(handle_module_game);
    //    handle_module_game = nullptr;
    //}

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

extern "C" void* CreateInstance(){ return new GameEngine();}
extern "C" void DestroyInstance(void *p){ 
    if(p) {
        static_cast<GameEngine*>(p)->CleanUp();
        static_cast<GameEngine*>(p)->DestroyInstance(static_cast<GameEngine*>(p)->handle_module_yamlcore,static_cast<GameEngine*>(p)->yamler);
        static_cast<GameEngine*>(p)->DestroyInstance(static_cast<GameEngine*>(p)->handle_module_sdlcore,static_cast<GameEngine*>(p)->sdler);
        //static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_game,static_cast<Application*>(p)->gamer);
        static_cast<GameEngine*>(p)->DestroyInstance(static_cast<GameEngine*>(p)->handle_module_renderercore,static_cast<GameEngine*>(p)->renderer);
        static_cast<GameEngine*>(p)->DestroyInstance(static_cast<GameEngine*>(p)->handle_module_logcore, static_cast<GameEngine*>(p)->logger);
        static_cast<GameEngine*>(p)->DestroyInstance(static_cast<GameEngine*>(p)->handle_module_resourcecore, static_cast<GameEngine*>(p)->resourcer);
        delete static_cast<GameEngine*>(p);
        //std::cout<<"- Destroy Instance Application."<<std::endl;
    } 
}



bool GameEngine::Get_feature_graphics_enable_controls()  {return yamler->GetAppInfo().Feature.feature_graphics_enable_controls;}
bool GameEngine::Get_feature_graphics_show_all_metric_controls()  {return appInfo->Feature.feature_graphics_show_all_metric_controls;}
bool GameEngine::Get_feature_graphics_show_performance_control()  {return appInfo->Feature.feature_graphics_show_performance_control;}
void GameEngine::Set_feature_graphics_enable_controls(bool value)  {appInfo->Feature.feature_graphics_enable_controls = value;}
void GameEngine::Set_feature_graphics_show_all_metric_controls(bool value) {appInfo->Feature.feature_graphics_show_all_metric_controls = value;}
void GameEngine::Set_feature_graphics_show_performance_control(bool value) {appInfo->Feature.feature_graphics_show_performance_control = value;}

int GameEngine::Get_feature_raytracing_pipeline_render_mode() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_render_mode;}
int GameEngine::Get_feature_raytracing_pipeline_interactive_move_mode() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_interactive_move_mode;}
int GameEngine::Get_feature_raytracing_pipeline_interactive_still_mode() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_interactive_still_mode;}
int GameEngine::Get_feature_raytracing_pipeline_sampler_per_pixel() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_sampler_per_pixel;}
int GameEngine::Get_feature_raytracing_pipeline_maximum_bounce() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_maximum_bounce;}
int GameEngine::Get_feature_raytracing_pipeline_maximum_path() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_maximum_path;}
bool GameEngine::Get_feature_raytracing_pipeline_accumulate() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_accumulate;}
bool GameEngine::Get_feature_raytracing_pipeline_enableNEE() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_enableNEE;}
bool GameEngine::Get_feature_raytracing_pipeline_use_sky() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_use_sky;}
float GameEngine::Get_feature_raytracing_pipeline_maximum_Radiance() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_maximum_Radiance;}
int GameEngine::Get_feature_raytracing_pipeline_debug_mode() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_debug_mode;}
bool GameEngine::Get_feature_raytracing_pipeline_softShadowEnable() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_softShadowEnable; }
int GameEngine::Get_feature_raytracing_pipeline_softShadowSampleNumber() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_softShadowSampleNumber; }
int GameEngine::Get_feature_raytracing_pipeline_maxReflectionDepth() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_maxReflectionDepth;}
int GameEngine::Get_feature_raytracing_pipeline_maxRefractionDepth() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_maxRefractionDepth;}
bool GameEngine::Get_feature_raytracing_pipeline_shadowRayIgnoreSphere() {return yamler->GetAppInfo().Feature.feature_raytracing_pipeline_shadow_ray_ignore_sphere;}

int GameEngine::GetControlNodeSize() { return controlNodes.size();}
void GameEngine::SetControlNodeVisible(int nodeId, bool value) { controlNodes[nodeId]->bVisible = value;}
void* GameEngine::GetInstanceHandle() {return renderer->GetInstance();}// instance->getHandle();}

int GameEngine::GetObjectSize() { return objects.size(); }
int GameEngine::GetCustomObjectSize() { return appInfo->Objects.size(); }
void GameEngine::SetObjectVelocity(int objectId, float vx, float vy, float vz) {objects[objectId].SetVelocity(vx, vy, vz);}
void GameEngine::SetObjectVelocity(int objectId, glm::vec3 v) {objects[objectId].SetVelocity(v);}
void GameEngine::SetObjectAngularVelocity(int objectId, float vx, float vy, float vz) {objects[objectId].SetAngularVelocity(vx, vy, vz); }
void GameEngine::SetObjectPosition(int objectId, float px, float py, float pz) { objects[objectId].SetPosition(px, py, pz); }
void GameEngine::SetObjectPosition(int objectId, glm::vec3 p) { objects[objectId].SetPosition(p); }
void GameEngine::SetObjectScaleRectangleXY(int objectId, float x0, float y0, float x1, float y1) { objects[objectId].SetScaleRectangleXY(x0, y0, x1, y1); }
glm::vec3 GameEngine::GetObjectPosition(int objectId) { return objects[objectId].Position; }

glm::vec3 GameEngine::GetObjectScale(int objectId) { return objects[objectId].Scale; }
int GameEngine::GetObjectModelID(int objectId) { return objects[objectId].m_model_id; }
int GameEngine::GetObjectMaterialID(int objectId) { return objects[objectId].m_material_id; }
glm::vec3 GameEngine::GetObjectRotation(int objectId) { return objects[objectId].Rotation; }
int GameEngine::GetObjectTextureID_BaseColor(int objectId) { return objects[objectId].m_texture_ids[0];};

int GameEngine::GetLightSize() { return lights.size(); }
glm::vec3 GameEngine::GetLightPosition(int lightId) { return lights[lightId].GetLightPosition(); }
void GameEngine::SetLightPosition(int lightId, float px, float py, float pz) { lights[lightId].SetLightPosition(glm::vec3(px, py, pz)); }
void GameEngine::SetLightPosition(int lightId, glm::vec3 p) { lights[lightId].SetLightPosition(p); }

void GameEngine::CreateCustomModel2D(std::vector<Vertex2D> &vertices2D) { resourcer->CreateModelCustomModel2D(vertices2D);}
void GameEngine::CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage) {
    resourcer->CreateModelCustomModel3D(vertices3D, indices3D, isTextboxImage);
}

void GameEngine::DrawObject(int objectId) { objects[objectId].Draw(); }
void GameEngine::DrawTexts() { textManager.Draw(); }
void GameEngine::DrawObjects() { for(int i = 0; i < objects.size(); i++) objects[i].Draw(); }
void GameEngine::DrawObjects(int startObjectId, int endObjectId) { 
    for(int i = startObjectId; i <= endObjectId && i < objects.size(); i++)  objects[i].Draw(); 
}
void GameEngine::DrawObject(int objectId, int pipelineId) { objects[objectId].Draw(pipelineId); }
void GameEngine::DrawObject(int objectId, int pipelineId, int numVertex) { objects[objectId].Draw_NoIndexNoSet(pipelineId, numVertex); }
void GameEngine::DrawParticlesFromStorageBuffer(int objectId, uint32_t particleCount) {
    objects[objectId].Draw(renderer->GetStorageBuffers(), -1, particleCount);
}

void GameEngine::ComputeDispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
    Dispatch(numWorkGroupsX,numWorkGroupsY,numWorkGroupsZ);
}

void GameEngine::RayTrace(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
    Trace(numWorkGroupsX,numWorkGroupsY,numWorkGroupsZ);
}


void GameEngine::SetComputeCustomSize(int size) { appInfo->Uniform.ComputeCustom.Size = size; }
void GameEngine::SetComputeCustomBinding(void* binding) {
    VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
    if (bindingPtr) appInfo->Uniform.ComputeCustom.Binding = *bindingPtr;
}
void GameEngine::UploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) {
    renderer->uploadComputeCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
}

void GameEngine::SetRaytracingCustomSize(int size) { appInfo->Uniform.RaytracingCustom.Size = size; }
void GameEngine::SetRaytracingCustomBinding(void* binding) {
    VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
    if (bindingPtr) appInfo->Uniform.RaytracingCustom.Binding = *bindingPtr;
}
void GameEngine::UploadRaytracingCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) {
    renderer->uploadRaytracingStorageBuffer_custom(currentFrame, customUniformBufferObject, dataSize);
}

// void GameEngine::SetComputeStorageBufferSize_WindowSwap(int size) { appInfo->Uniform.ComputeStorageBuffer.Size = size; }
// void GameEngine::SetComputeStorageBufferUsage_WindowSwap(int usage) {appInfo->Uniform.ComputeStorageBuffer.Usage = usage; }
void GameEngine::UploadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) {
    renderer->uploadStorageBuffer_windowswap(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::DownloadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) {
    renderer->downloadStorageBuffer_windowswap(currentFrame, storageBufferObject, dataSize);
}

void GameEngine::UploadComputeUniformBuffer_Material(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadComputeUniformBuffer_material(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::UploadComputeStorageBuffer_TriangleVertexAttribute(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadStorageBuffer_triangleVertexAttribute(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::UploadComputeStorageBuffer_TriangleVertexIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadStorageBuffer_triangleVertexIndex(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::UploadComputeStorageBuffer_TriangleReorderIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadStorageBuffer_triangleReorderIndex(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::UploadComputeStorageBuffer_BVHNode(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadStorageBuffer_bvhNode(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::UploadComputeStorageBuffer_Sphere(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadStorageBuffer_sphere(currentFrame, storageBufferObject, dataSize);
}

// void GameEngine::UploadRaytracingStorageBuffer_TriangleVertexAttribute(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
//     renderer->uploadRaytracingStorageBuffer_triangleVertexAttribute(currentFrame, storageBufferObject, dataSize);
// }
// void GameEngine::UploadRaytracingStorageBuffer_TriangleVertexIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
//     renderer->uploadRaytracingStorageBuffer_triangleVertexIndex(currentFrame, storageBufferObject, dataSize);
// }
void GameEngine::UploadRaytracingStorageBuffer_GeometryInfo(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize){
    renderer->uploadRaytracingStorageBuffer_geometryInfo(currentFrame, storageBufferObject, dataSize);
}


void GameEngine::SetComputeStorageBufferSize_CustomSwap(int size) { appInfo->Uniform.ComputeStorageBufferInfo_CustomSwap.Size = size; }
void GameEngine::SetComputeStorageBufferUsage_CustomSwap(int usage) {appInfo->Uniform.ComputeStorageBufferInfo_CustomSwap.Usage = usage; }
void GameEngine::UploadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) {
    renderer->uploadStorageBuffer_customswap(currentFrame, storageBufferObject, dataSize);
}
void GameEngine::DownloadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) {
    renderer->downloadStorageBuffer_customswap(currentFrame, storageBufferObject, dataSize);
}

void GameEngine::SetGraphicsCustomSize(int size) { appInfo->Uniform.GraphicsCustom.Size = size; }
void GameEngine::SetGraphicsCustomBinding(void* binding) {
    VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
    if (bindingPtr) appInfo->Uniform.GraphicsCustom.Binding = *bindingPtr;
}
void GameEngine::UploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) {
    //graphicsDescriptorManager.uploadCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
    renderer->uploadGraphicsCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
}

void GameEngine::SetMainCameraVelocityX(float value) { if(!bRaytracingRenderMode) mainCamera.Velocity.x = value; }
void GameEngine::SetMainCameraVelocityY(float value) { if(!bRaytracingRenderMode) mainCamera.Velocity.y = value; }
void GameEngine::SetMainCameraVelocityZ(float value) { if(!bRaytracingRenderMode) mainCamera.Velocity.z = value; }
void GameEngine::SetMainCameraAngularVelocityX(float value) { if(!bRaytracingRenderMode) mainCamera.AngularVelocity.x = value; }
void GameEngine::SetMainCameraAngularVelocityY(float value) { if(!bRaytracingRenderMode) mainCamera.AngularVelocity.y = value; }
void GameEngine::SetMainCameraAngularVelocityZ(float value) { if(!bRaytracingRenderMode) mainCamera.AngularVelocity.z = value; }
void GameEngine::SetMainCameraType(int type) { if(!bRaytracingRenderMode) mainCamera.cameraType = (CameraType)type; }
int GameEngine::GetMainCameraType() { return mainCamera.cameraType; }
void GameEngine::SetMainCameraFocusObjectId(int objectId) { if(!bRaytracingRenderMode) mainCamera.focusObjectId = objectId; }
int GameEngine::GetMainCameraFocusObjectId() { return mainCamera.focusObjectId; }
void GameEngine::MoveMainCameraLeft(float distance, float speed) { if(!bRaytracingRenderMode) mainCamera.MoveLeft(distance, speed); }
void GameEngine::MoveMainCameraRight(float distance, float speed) { if(!bRaytracingRenderMode) mainCamera.MoveRight(distance, speed); }
void GameEngine::MoveMainCameraForward(float distance, float speed) { if(!bRaytracingRenderMode) mainCamera.MoveForward(distance, speed); }
void GameEngine::MoveMainCameraBackward(float distance, float speed) { if(!bRaytracingRenderMode) mainCamera.MoveBackward(distance, speed); }
glm::vec3 GameEngine::GetMainCameraPosition() { return mainCamera.Position; }
void GameEngine::SetMainCameraSensitivity(float sensitivity) { mainCamera.SetRotationSensitivity(sensitivity); }

void GameEngine::ToggleRaytracingRenderMode(){bRaytracingRenderMode = !bRaytracingRenderMode; }
bool GameEngine::GetRaytracingRenderMode() {return bRaytracingRenderMode; }

void GameEngine::SetLightCameraPosition(int lightCameraId, glm::vec3 p) { lightCameras[lightCameraId].SetPosition(p); }
void GameEngine::SetLightCameraFocusObjectId(int lightCameraId, int objectId) { lightCameras[lightCameraId].focusObjectId = objectId; }
int GameEngine::GetLightCameraFocusObjectId(int lightCameraId) { return lightCameras[lightCameraId].focusObjectId; }

void GameEngine::LogContext(std::string s, float *n, int size) { logger->LogArray(s, n, size); }
void GameEngine::LogContext(std::string s) { logger->Print(s); }
void GameEngine::LogContext(std::string s, float n) { logger->Print(s, n); }
void GameEngine::LogContext(std::string s, int n1, int n2) { logger->Log(s, n1, n2); }

//void GameEngine::SetRenderMode(int mode) { appInfo->RenderMode = (RenderModes)mode; }
void GameEngine::SetPause(bool value) { NeedToPause = value; }
int GameEngine::GetWindowWidth() { return windowWidth; }
int GameEngine::GetWindowHeight() { return windowHeight; }
int GameEngine::GetCurrentFrame() { return renderer->GetCurrentFrame();}
double GameEngine::GetElapseTime() { return elapseTime;}
double GameEngine::GetDeltaTime() { return deltaTime; }

void GameEngine::CmdNextSubpass() { vkCmdNextSubpass(renderer->GetGraphicsCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE); }
void GameEngine::SetSwapchainImageSize(int size) { renderer->SetSwapchain_ImageSize(size); }
//void GameEngine::EnableComputeSwapChainImage(bool enable) { renderer->SetSwapchain_Compute_Image(enable); }
void GameEngine::DeviceWaitIdle() { vkDeviceWaitIdle(renderer->GetLogicalDevice()); }

void GameEngine::PushConstantToCommand(void* pcData, int pipelineId) {
    renderer->PushConstantToCommand(pcData, renderer->GetGraphicsPipelineLayout(pipelineId), resourcer->GetShaderPushConstantRange());
}
void GameEngine::CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    vkCmdSetDepthBias(renderer->GetGraphicsCommandBuffer(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void GameEngine::ConvertStorageImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout){
    //for RenderModes::COMPUTE_SWAPCHAIN, need convert intermediaColor_Image to VK_IMAGE_LAYOUT_GENERAL
    //intermediaColor_Image can only be created as VK_IMAGE_LAYOUT_UNDEFINED
    if(renderer->GetRenderMode() == RenderModes::COMPUTE_SWAPCHAIN){
        //std::cout<<"Convert intermedia color image layout to VK_IMAGE_LAYOUT_GENERAL for Render Mode COMPUTE_SWAPCHAIN."<<std::endl;

        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            renderer->SetCurrentFrame(i);

            vkResetCommandBuffer(renderer->GetComputeCommandBuffer(), 0);//VkCommandBufferResetFlagBits

            renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());
            renderer->RecordImageBarrier(
                renderer->GetComputeCommandBuffer(),
                renderer->GetIntermediaColor_Image(renderer->GetCurrentFrame(), 0),
                oldLayout,
                newLayout,
                0,
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
            );
            renderer->EndRecordComputeCommandBuffer();

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &renderer->GetComputeCommandBuffer();

            if (vkQueueSubmit(renderer->GetComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit compute queue (convert)!");
            }
        }

        renderer->SetCurrentFrame(0);
        vkDeviceWaitIdle(renderer->GetLogicalDevice());
    }

    if(renderer->GetRenderMode() == RenderModes::RAYTRACING_SWAPCHAIN){
        //std::cout<<"Convert intermedia color image layout to VK_IMAGE_LAYOUT_GENERAL for Render Mode RAYTRACING_SWAPCHAIN."<<std::endl;

        for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            renderer->SetCurrentFrame(i);

            vkResetCommandBuffer(renderer->GetRaytracingCommandBuffer(), /*VkCommandBufferResetFlagBits*/ 0);

            renderer->StartRecordRaytracingCommandBuffer(renderer->GetRaytracingPipeline(), renderer->GetRaytracingPipelineLayout());
            renderer->RecordImageBarrier(
                renderer->GetRaytracingCommandBuffer(),
                renderer->GetIntermediaColor_Image(renderer->GetCurrentFrame(), 0),
                oldLayout,
                newLayout,
                0,
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
            );
            renderer->RecordImageBarrier(
                renderer->GetRaytracingCommandBuffer(),
                renderer->GetIntermediaColor_Image(renderer->GetCurrentFrame(), 1),
                oldLayout,
                newLayout,
                0,
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
            );
            renderer->EndRecordRaytracingCommandBuffer();

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &renderer->GetRaytracingCommandBuffer();

            if (vkQueueSubmit(renderer->GetComputeQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
                throw std::runtime_error("failed to submit compute queue (convert for ray tracing)!");
            }
        }


        renderer->SetCurrentFrame(0);
        vkDeviceWaitIdle(renderer->GetLogicalDevice());
        //std::cout<<"Finish Convert intermedia color image layout to VK_IMAGE_LAYOUT_GENERAL for Render Mode RAYTRACING_SWAPCHAIN."<<std::endl;
    }
}

/*
void GameEngine::CreateComputeCommandBuffers_DispatchForSwapchainImage(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
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
void GameEngine::CreateComputeCommandBuffers_DispatchForSwapchainImage_(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) {
    std::vector<VkCommandBuffer> &commandBuffers = renderer->GetComputeCommandBuffers();// renderer.commandBuffers[renderer.computeCmdId];
    std::vector<VkImage> &swapChainImages = renderer->GetSwapchain_Images();
    //renderer->SetCurrentFrame(i);
    //std::cout<<"commandbuffer i: "<<i<<std::endl;
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to begin recording command buffer!");
    //}
    //renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);
    renderer->StartRecordComputeCommandBuffer(renderer->GetComputePipeline(), renderer->GetComputePipelineLayout());

    renderer->RecordImageBarrier(commandBuffers[renderer->GetCurrentFrame()], swapChainImages[renderer->GetCurrentFrame()],
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, //before write, expect layout to be VK_IMAGE_LAYOUT_GENERAL
        VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

    Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);

    renderer->RecordImageBarrier(commandBuffers[renderer->GetCurrentFrame()], swapChainImages[renderer->GetCurrentFrame()],
        VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, //before present, expect layout to be VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    //if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
    //    throw std::runtime_error("failed to record command buffer!");
    //}
    renderer->EndRecordComputeCommandBuffer();
}*/

}//namespace

