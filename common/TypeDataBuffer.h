#pragma once
#include <vulkan/vulkan.h>
#include "Config.h"
#include <stdexcept>

class CWxjBuffer final{
public:
    CWxjBuffer(): m_size(0){}
    ~CWxjBuffer(){}

    VkResult init(IN VkDeviceSize requiredSize, VkBufferUsageFlags usage, VkDevice logicalDevice, VkPhysicalDevice physicalDevice, bool needDeviceAddress = false) {
        //HERE_I_AM("Init05DataBuffer");
        //Step1:Create Buffer(create buffer)
        VkResult result = VK_SUCCESS;

        VkBufferCreateInfo  vbci;
        vbci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vbci.pNext = nullptr;
        vbci.flags = 0;
        vbci.size = requiredSize;
        vbci.usage = usage;
        vbci.queueFamilyIndexCount = 0;
        vbci.pQueueFamilyIndices = (const uint32_t *)nullptr;
        vbci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	// can only use CONCURRENT if .queueFamilyIndexCount > 0
        result = vkCreateBuffer(logicalDevice, IN &vbci, PALLOCATOR, OUT &buffer); //CContext::GetHandle().GetLogicalDevice()
        //REPORT("vkCreateBuffer");

        //Step 2:allocate memory(create deviceMemory in gpu)
        VkMemoryRequirements			vmr;
        vkGetBufferMemoryRequirements(logicalDevice, IN buffer, OUT &vmr);		// fills vmr
        //if (Verbose){
        //fprintf(debugger->FpDebug, "Buffer vmr.size = %lld\n", vmr.size);
        //fprintf(debugger->FpDebug, "Buffer vmr.alignment = %lld\n", vmr.alignment);
        //fprintf(debugger->FpDebug, "Buffer vmr.memoryTypeBits = 0x%08x\n", vmr.memoryTypeBits);
        //fflush(debugger->FpDebug);
        //}
        m_size = vmr.size;//vmr.size is different than the input requiredSize, because of alignment reason, vmr.size can be larger

        //add for ray tracing pipeline
        VkMemoryAllocateFlagsInfo flagsInfo{};
        flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfo.flags = needDeviceAddress ? VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT : 0;

        VkMemoryAllocateInfo			vmai;
        vmai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vmai.pNext = needDeviceAddress ? &flagsInfo : nullptr; //change for ray tracing pipeline
        vmai.allocationSize = vmr.size; 
        vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits, physicalDevice);
        //VkDeviceMemory				vdm;
        result = vkAllocateMemory(logicalDevice, IN &vmai, PALLOCATOR, OUT &deviceMemory);
       
        //REPORT("vkAllocateMemory");
        //deviceMemory = vdm;

        //Step 3: bind memory(bind buffer and deviceMemory)
        result = vkBindBufferMemory(logicalDevice, buffer, IN deviceMemory, 0);		// 0 is the offset
        //REPORT("vkBindBufferMemory");

        return result;
    }

    VkResult fill(IN void * data, VkDevice logicalDevice) {
        //Step 4:copy memory(copy data into deviceMemory)
        void * pGpuMemory;
        vkMapMemory(logicalDevice, IN deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
        memcpy(pGpuMemory, data, (size_t)m_size);
        vkUnmapMemory(logicalDevice, IN deviceMemory);
        return VK_SUCCESS;
    }

    void DestroyAndFree(VkDevice logicalDevice){
        if(m_size != 0){
            vkDestroyBuffer(logicalDevice, buffer, nullptr);
            vkFreeMemory(logicalDevice, deviceMemory, nullptr);
        }
    }

    VkBuffer		buffer;
    VkDeviceMemory		deviceMemory;

private:
	VkDeviceSize		m_size;

    int FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits, VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties	vpdmp;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, OUT &vpdmp);//instance->pickedPhysicalDevice->get()->getHandle() ， CContext::GetHandle().GetPhysicalDevice()
        for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
            VkMemoryType vmt = vpdmp.memoryTypes[i];
            VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
            if ((memoryTypeBits & (1 << i)) != 0) {
                if (((vmpf & memoryFlagBits) && (vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0){
                    //fprintf(debugger->FpDebug, "Found given memory flag (0x%08x) and type (0x%08x): i = %d\n", memoryFlagBits, memoryTypeBits, i);
                    return i;
                }
            }
        }

        //fprintf(debugger->FpDebug, "Could not find given memory flag (0x%08x) and type (0x%08x)\n", memoryFlagBits, memoryTypeBits);
        throw  std::runtime_error("Could not find given memory flag and type");
    }

    int FindMemoryThatIsHostVisible(uint32_t memoryTypeBits, VkPhysicalDevice physicalDevice) {
        return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits, physicalDevice);
    }


};