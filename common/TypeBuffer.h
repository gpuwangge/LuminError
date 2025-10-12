#pragma once
#include <vulkan/vulkan.h>
#include "Config.h"
#include "../application/include/context.h"

class CWxjBuffer final{
public:
    CWxjBuffer(): m_size(0){}
    ~CWxjBuffer(){}

    VkResult init(IN VkDeviceSize requiredSize, VkBufferUsageFlags usage) {
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
        result = vkCreateBuffer(CContext::GetHandle().GetLogicalDevice(), IN &vbci, PALLOCATOR, OUT &buffer);
        //REPORT("vkCreateBuffer");

        //Step 2:allocate memory(create deviceMemory in gpu)
        VkMemoryRequirements			vmr;
        vkGetBufferMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), IN buffer, OUT &vmr);		// fills vmr
        //if (Verbose){
        //fprintf(debugger->FpDebug, "Buffer vmr.size = %lld\n", vmr.size);
        //fprintf(debugger->FpDebug, "Buffer vmr.alignment = %lld\n", vmr.alignment);
        //fprintf(debugger->FpDebug, "Buffer vmr.memoryTypeBits = 0x%08x\n", vmr.memoryTypeBits);
        //fflush(debugger->FpDebug);
        //}
         m_size = vmr.size;//vmr.size is different than the input requiredSize, because of alignment reason, vmr.size can be larger

        VkMemoryAllocateInfo			vmai;
        vmai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vmai.pNext = nullptr;
        vmai.allocationSize = vmr.size; 
        vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits);
        //VkDeviceMemory				vdm;
        result = vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), IN &vmai, PALLOCATOR, OUT &deviceMemory);
       
        //REPORT("vkAllocateMemory");
        //deviceMemory = vdm;

        //Step 3: bind memory(bind buffer and deviceMemory)
        result = vkBindBufferMemory(CContext::GetHandle().GetLogicalDevice(), buffer, IN deviceMemory, 0);		// 0 is the offset
        //REPORT("vkBindBufferMemory");

        return result;
    }

    VkResult fill(IN void * data) {
        //Step 4:copy memory(copy data into deviceMemory)
        void * pGpuMemory;
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
        memcpy(pGpuMemory, data, (size_t)m_size);
        vkUnmapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory);
        return VK_SUCCESS;
    }

    void DestroyAndFree(){
        if(m_size != 0){
            vkDestroyBuffer(CContext::GetHandle().GetLogicalDevice(), buffer, nullptr);
            vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), deviceMemory, nullptr);
        }
    }

    VkBuffer		buffer;
    VkDeviceMemory		deviceMemory;

private:
	VkDeviceSize		m_size;

    int FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
        VkPhysicalDeviceMemoryProperties	vpdmp;
        vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), OUT &vpdmp);//instance->pickedPhysicalDevice->get()->getHandle()
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

    int FindMemoryThatIsHostVisible(uint32_t memoryTypeBits) {
        return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits);
    }


};