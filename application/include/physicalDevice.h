#ifndef H_PHYSICALDEVICE
#define H_PHYSICALDEVICE

//#include "instance.h"
#include "logicalDevice.h"
//#include "context.h"
#include "logManager.h"
#include "Config.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <locale>

//#define PRINT logManager.print

//class CInstance;//forward declaration

class CPhysicalDevice{
public:
    //CInstance *m_instance;
    CPhysicalDevice(VkPhysicalDevice physical_device);
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device);

    ~CPhysicalDevice();

    VkPhysicalDevice getHandle() const{ return handle;}

    //CDebugger * debugger;
    CLogManager logManager;

    //void setInstance(CInstance *instance);

    QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, std::string s);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface);

    bool checkDeviceExtensionSupport(const std::vector<const char*>  requireDeviceExtensions);

    std::vector<std::unique_ptr<CLogicalDevice>> logicalDevices;
    void createLogicalDevices(VkSurfaceKHR surface, const std::vector<const char*> requiredValidationLayers, const std::vector<const char*>  requireDeviceExtensions);
    VkDevice getLogicalDevice() {return logicalDevices.back().get()->logicalDevice; }
    VkQueue getGraphicsQueue()  {return logicalDevices.back().get()->graphicsQueue; }
    VkQueue getPresentQueue()   {return logicalDevices.back().get()->presentQueue;  }
    VkQueue getComputeQueue()   {return logicalDevices.back().get()->computeQueue;  }

    

    void displayPhysicalDevices();
    
private:
    VkPhysicalDevice handle{VK_NULL_HANDLE};
};

#endif
