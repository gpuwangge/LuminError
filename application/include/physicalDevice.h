#ifndef H_PHYSICALDEVICE
#define H_PHYSICALDEVICE

//#include "instance.h"
#include "logicalDevice.h"
#include "common.h"
//#include "context.h"
#include "logManager.h"

//#define PRINT logManager.print

//class CInstance;//forward declaration

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> graphicsAndComputeFamily;
	std::optional<uint32_t> presentFamily;
	

	bool isComplete() {
		return graphicsAndComputeFamily.has_value() && graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

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
    bool checkDeviceExtensionSupport(const std::vector<const char*>  requireDeviceExtensions);
    SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface);

    std::vector<std::unique_ptr<CLogicalDevice>> logicalDevices;
    void createLogicalDevices(VkSurfaceKHR surface, const std::vector<const char*> requiredValidationLayers, const std::vector<const char*>  requireDeviceExtensions);
    VkDevice getLogicalDevice() {return logicalDevices.back().get()->logicalDevice; }
    VkQueue getGraphicsQueue()  {return logicalDevices.back().get()->graphicsQueue; }
    VkQueue getPresentQueue()   {return logicalDevices.back().get()->presentQueue;  }
    VkQueue getComputeQueue()   {return logicalDevices.back().get()->computeQueue;  }

    VkSampleCountFlagBits getMaxUsableSampleCount();

    void displayPhysicalDevices();
    
private:
    VkPhysicalDevice handle{VK_NULL_HANDLE};
};

#endif
