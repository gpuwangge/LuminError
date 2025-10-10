#ifndef H_INSTANCE
#define H_INSTANCE

#include "common.h"
#include "physicalDevice.h"
#include "context.h"

class CInstance{
public:
    CInstance(const std::vector<const char*> &requiredValidationLayers, std::vector<const char*> &requiredExtensions);

    ~CInstance();

    VkInstance handle{VK_NULL_HANDLE};
    VkInstance getHandle() const{ return handle;}

    //CDebugger * debugger;

    void DisplayLayers(std::vector<VkLayerProperties> &availableLayers);
    void DisplayExtensions(std::vector<VkExtensionProperties> &availableExtensions);
    void DisplayExtensions(std::vector<const char*> &availableExtensions);
    //void DisplayRequiredExtensions();

#ifndef ANDROID
    VkDebugUtilsMessengerEXT debugMessenger;//02
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    //std::vector<const char*> getRequiredExtensions();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void Init02SetupDebugMessenger();
#endif

    std::vector<std::unique_ptr<CPhysicalDevice>> physicalDevices;
    //std::unique_ptr<CPhysicalDevice>* pickedPhysicalDevice;
    void findAllPhysicalDevices();
    std::unique_ptr<CPhysicalDevice>* pickSuitablePhysicalDevice(VkSurfaceKHR surface, const std::vector<const char*>  requireDeviceExtensions, VkQueueFlagBits requiredQueueFamilies);
};


#endif