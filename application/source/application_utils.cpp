#include "../include/application.h"

namespace LEApplication{
/*************
 * Helper Functions
 ************/
Application::Application(){
    //lightCameras.resize(2); //work
    lightCameras.resize(LIGHT_MAX); //TODO: for test purpose, create more cameras than needed

    logManager.setLogFile("application.log");
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

    if (enableValidationLayers) DestroyDebugUtilsMessengerEXT(instance->getHandle(), instance->debugMessenger, nullptr);

    vkDestroySurfaceKHR(instance->getHandle(), surface, nullptr);
    vkDestroyInstance(instance->getHandle(), nullptr);
    
    CContext::Quit();

    //std::cout<<"End Cleanup()."<<std::endl;
}

Application::~Application(){
    CleanUp();

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
}

extern "C" void* CreateInstance(){ return new Application();}
extern "C" void DestroyInstance(void *p){ 
    if(p) {
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_yamlcore,static_cast<Application*>(p)->instance_yamlcore);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_sdlcore,static_cast<Application*>(p)->instance_sdlcore);
        static_cast<Application*>(p)->DestroyInstance(static_cast<Application*>(p)->handle_module_game,static_cast<Application*>(p)->instance_game);
        delete static_cast<Application*>(p);
        //std::cout<<"- Destroy Instance Application."<<std::endl;
} 
}

}

