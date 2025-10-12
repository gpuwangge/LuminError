#ifndef H_LOGICALDEVICE
#define H_LOGICALDEVICE

#include <vulkan/vulkan.h>

class CLogicalDevice final{

public:
    CLogicalDevice();
    ~CLogicalDevice();

    VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;

    //VkDevice handle{VK_NULL_HANDLE};
    //VkDevice getHandle() const{ return handle;}

    //CDebugger * debugger;
};

#endif
