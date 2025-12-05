#pragma once

#include "logicalDevice.h"
#include "Config.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <locale>
#include "ILogCore.h"
#include "Utility.h"
#include <memory>

class CPhysicalDevice{
public:
    //CInstance *m_instance;
    CPhysicalDevice(VkPhysicalDevice physical_device, LELog::ILogCore *logger_);
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device);

    ~CPhysicalDevice();

    VkPhysicalDevice getHandle() const{ return handle;}

    //CDebugger * debugger;
    //CLogManager logManager;
    LELog::ILogCore *logger = NULL;

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

