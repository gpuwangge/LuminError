#include "../include/instance.h"

#ifdef ANDROID
#include "..\\..\\androidFramework\\include\\androidFileManager.h"
#endif

CInstance::CInstance(const std::vector<const char*> &requiredValidationLayers, std::vector<const char*> &requiredExtensions){
    //debugger = new CDebugger("../logs/instance.log");

    VkResult result = VK_SUCCESS;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VULKAN PLATFORM";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

#ifndef ANDROID
    //First make sure required layer(s) are available
    if (enableValidationLayers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // see what layers are actually available:

        std::vector<VkLayerProperties> availableLayers(layerCount);
        result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        PRINT("vkEnumerateInstanceLayerProperties");
        DisplayLayers(availableLayers);

        PRINT("Required Layers:");
        for (const char* layerName : requiredValidationLayers) {// to find out validation layer in available layers
            PRINT("\t" + std::string(layerName));
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    //debugger->writeMSG("validation layers available!(%s)\n\n", layerName);
                    break;
                }
            }
        }

        if (result != VK_SUCCESS) throw std::runtime_error("validation layers requested, but not available!");
    }
#endif

    //Second set required extension(s)
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, (VkExtensionProperties *)nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties((char *)nullptr, &extensionCount, availableExtensions.data());
    PRINT("vkEnumerateInstanceExtensionProperties");
    DisplayExtensions(availableExtensions);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#ifndef ANDROID    
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
#endif
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#ifndef ANDROID        
    }
#endif

    //auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    PRINT("Required Extensions");
    DisplayExtensions(requiredExtensions);

    //Third create instance
    result = vkCreateInstance(&createInfo, nullptr, &handle);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance!");

    //REPORT("vkCreateInstance");

#ifndef ANDROID
    Init02SetupDebugMessenger();
#endif
}

CInstance::~CInstance(){
    //if (!debugger) delete debugger;
    //if (handle != VK_NULL_HANDLE) vkDestroyInstance(handle, nullptr);
}

#ifndef ANDROID  
void CInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}
#endif

#ifndef ANDROID  
VkResult CInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
#endif

#ifndef ANDROID
void CInstance::Init02SetupDebugMessenger() {
    //HERE_I_AM("Init02CreateDebugMessenger");

    VkResult result = VK_SUCCESS;

    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    result = CreateDebugUtilsMessengerEXT(handle, &createInfo, nullptr, &debugMessenger);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to set up debug messenger!");

    //REPORT("CreateDebugUtilsMessengerEXT");
}
#endif

void CInstance::findAllPhysicalDevices(){
    //HERE_I_AM("findAllPhysicalDevices");

    VkResult result = VK_SUCCESS;

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, (VkPhysicalDevice *)nullptr);
    
    if (result != VK_SUCCESS || deviceCount == 0) {
        //debugger->writeMSG("Could not count the physical devices\n");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    //debugger->writeMSG("%d physical devices found.\n", deviceCount);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    result = vkEnumeratePhysicalDevices(handle, OUT &deviceCount, OUT devices.data());
    //REPORT("vkEnumeratePhysicalDevices");

    if (result != VK_SUCCESS) {
        //debugger->writeMSG("Could not enumerate the %d physical devices\n", deviceCount);
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (auto &physical_device : devices){
        //gpus.push_back(std::make_unique<CPhysicalDevice>(*this, physical_device));
        physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
        physicalDevices.back().get()->displayPhysicalDevices();
    }

}

std::unique_ptr<CPhysicalDevice>* CInstance::pickSuitablePhysicalDevice(VkSurfaceKHR surface, const std::vector<const char*>  requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies){
    //HERE_I_AM("pickSuitablePhysicalDevice");

    if(physicalDevices.empty()){
        //debugger->writeMSG("Error: No physical devices!\n");
        return nullptr;
    }

    for ( auto& phy_device : physicalDevices) {
        
        //Test Queue Families
        QueueFamilyIndices indices = phy_device->findQueueFamilies(surface, "Find Queue Families when picking physical device");

        //Test Extension
        bool extensionsSupported = phy_device->checkDeviceExtensionSupport(requireDeviceExtensions);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            //Test SwapChain
            SwapChainSupportDetails swapChainSupport = phy_device->querySwapChainSupport(surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        if (indices.isComplete() && extensionsSupported && swapChainAdequate) {
            if(requiredQueueFamilies & VK_QUEUE_GRAPHICS_BIT) { //VK_QUEUE_GRAPHICS_BIT can do everything, including Compute
                PRINT("Require VK_QUEUE_GRAPHICS_BIT");
                if(!indices.graphicsFamily.has_value()) return nullptr;
                //debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsFamily.value());debugger->flush();
                //PRINT("Picked physical device index: %d", (int)indices.computeFamily.value());
            }
            if(requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) {
                PRINT("Require VK_QUEUE_COMPUTE_BIT");
                if(!indices.computeFamily.has_value()) return nullptr;
                //PRINT("Picked physical device index: %d", (int)indices.computeFamily.value());
            }
            if((requiredQueueFamilies & VK_QUEUE_COMPUTE_BIT) & VK_QUEUE_GRAPHICS_BIT){
                //debugger->writeMSG("Require VK_QUEUE_COMPUTE_BIT & VK_QUEUE_GRAPHICS_BIT\n");debugger->flush();
                if(!indices.graphicsAndComputeFamily.has_value()) return nullptr;
                //debugger->writeMSG("Picked physical device index: %d\n", indices.graphicsAndComputeFamily.value());debugger->flush();
            }

            //pickedPhysicalDevice = &phy_device;
            VkPhysicalDeviceProperties	PhysicalDeviceProperties;
            vkGetPhysicalDeviceProperties(IN phy_device.get()->getHandle(), OUT &PhysicalDeviceProperties);
            PRINT("Picked physical device: %s\n", PhysicalDeviceProperties.deviceName);

            return &phy_device;  
        }
    }

    //if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
    //}
    return nullptr;
}

void CInstance::DisplayLayers(std::vector<VkLayerProperties> &availableLayers){
    for (const auto& layerProperties : availableLayers) 
        PRINT("\t" + std::string(layerProperties.layerName) + " (" + std::string(layerProperties.description) + "): 0x%08x, %5d", (int)layerProperties.specVersion, layerProperties.implementationVersion);
}

void CInstance::DisplayExtensions(std::vector<VkExtensionProperties> &availableExtensions){
    for (const auto& extensionsProperties : availableExtensions) 
        PRINT("\t" + std::string(extensionsProperties.extensionName) + ": 0x%08x", (int)extensionsProperties.specVersion);
}

void CInstance::DisplayExtensions(std::vector<const char*> &availableExtensions){
    for (const auto& extensionsProperties : availableExtensions) 
        PRINT("\t" + std::string(extensionsProperties));
}


    
