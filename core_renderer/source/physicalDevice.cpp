#include "physicalDevice.h"
#include <set>

CPhysicalDevice::CPhysicalDevice(VkPhysicalDevice physical_device, LELog::ILogCore *logger_) : handle(physical_device), logger(logger_) {
    //CPhysicalDevice(CInstance *instance, VkPhysicalDevice physical_device){
    //debugger = new CDebugger("../logs/physicalDevice.log");
// #ifndef ANDROID
//     logManager.setLogFile("physicalDevice.log");
// #endif
}

 //void CPhysicalDevice::setInstance(CInstance *instance){
    //m_instance = instance;
 //}

CPhysicalDevice::~CPhysicalDevice(){
    //if (!debugger) delete debugger;
}

QueueFamilyIndices CPhysicalDevice::findQueueFamilies(VkSurfaceKHR surface, std::string s) {
    //HERE_I_AM(s);

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, nullptr);
    //debugger->writeMSG("Found %d Queue Families:\n", queueFamilyCount);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyCount, queueFamilyProperties.data());

    /*
    int i = 0;
    if(debugger->getVerbose()){
        for (const auto& queueFamilyProperty : queueFamilyProperties) {
            fprintf(debugger->FpDebug, "\t%d: Queue Family Count = %2d  ;   ", i, queueFamilyProperty.queueCount);
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)	fprintf(debugger->FpDebug, " Graphics");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)	fprintf(debugger->FpDebug, " Compute");
            if ((queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)	fprintf(debugger->FpDebug, " Transfer");
            fprintf(debugger->FpDebug, "\n");
            i++;
        }
    }*/

    int i = 0;
    bool selected = false;
    for (const auto& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.computeFamily = i;
        }

        if ((queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
            indices.graphicsAndComputeFamily = i;
        }

        VkBool32 presentSupport = false;
        VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, surface, OUT &presentSupport);
        //REPORT("vkGetPhysicalDeviceSurfaceSupportKHR");

        if (presentSupport) {
            indices.presentFamily = i;
            //debugger->writeMSG("This Surface is supported by the Present Queue\n");
        }
        //else debugger->writeMSG("This Surface is not supported by the Present Queue\n");

        if (indices.isComplete()) {
            selected = true;
            //debugger->writeMSG("Complete: selected queue family %d that support Graphics, Present and Compute command.\n", i);
            break; //select the first suitable indices (which has graphics and present family)
        }
        i++;
    }
    
    //if(!selected) debugger->writeMSG("Can NOT find proper queue family!\n");
    return indices;
}

bool CPhysicalDevice::checkDeviceExtensionSupport(const std::vector<const char*>  requireDeviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(handle, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(requireDeviceExtensions.begin(), requireDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails CPhysicalDevice::querySwapChainSupport(VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, OUT &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formatCount, details.formats.data());
    }


    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

void CPhysicalDevice::createLogicalDevices(VkSurfaceKHR surface, const std::vector<const char*> requiredValidationLayers, const std::vector<const char*>  requireDeviceExtensions){
    //HERE_I_AM("createLogicalDevices");
    
    //physicalDevices.push_back(std::make_unique<CPhysicalDevice>(physical_device));
    logicalDevices.push_back(std::make_unique<CLogicalDevice>());//create one logicalDevice for each invocation of this function

    VkResult result = VK_SUCCESS;

    QueueFamilyIndices indices = findQueueFamilies(surface, "Find Queue Families when creating logical device");

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requireDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = requireDeviceExtensions.data();

#ifndef ANDROID
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    }
    else
#endif 
        createInfo.enabledLayerCount = 0;
    

    //create logicalDevice
    result = vkCreateDevice(handle, &createInfo, nullptr, &(logicalDevices.back().get()->logicalDevice));
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create logical device!");
    //REPORT("vkCreateLogicalDevice");

    //set PhysicalDevice's Queue Familily property to logicalDevice's queue
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsFamily.value(), 0, &(logicalDevices.back().get()->graphicsQueue)); //graphics queue use physical device's family 0 
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.presentFamily.value(), 0, &(logicalDevices.back().get()->presentQueue)); //present queue use the same family
    vkGetDeviceQueue(logicalDevices.back().get()->logicalDevice, indices.graphicsAndComputeFamily.value(), 0, &(logicalDevices.back().get()->computeQueue));//A physical device has several family, queue is pointing to one of the families
    
}

//MSAA related functions 
VkSampleCountFlagBits CPhysicalDevice::getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(handle, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void CPhysicalDevice::displayPhysicalDevices(){
    logger->Log("Physical Device DisplayPhysicalDevices: ");

    VkPhysicalDeviceProperties	PhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(IN handle, OUT &PhysicalDeviceProperties);

    logger->Log("\tlimits.timestampPeriod(ns): {}", (float)PhysicalDeviceProperties.limits.timestampPeriod);

    logger->Log("\tAPI version: {}", (int)PhysicalDeviceProperties.apiVersion);
    logger->Log("\tDriver version: {}", (int)PhysicalDeviceProperties.apiVersion);
    logger->Log("\tVendor ID: {:#06x}", (int)PhysicalDeviceProperties.vendorID);
    logger->Log("\tDevice ID: {:#06x}", (int)PhysicalDeviceProperties.deviceID);
    logger->Log("\tPhysical Device Type: {} =", (int)PhysicalDeviceProperties.deviceType);
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)	logger->Log("\t\t(Discrete GPU)");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)	logger->Log("\t\t(Integrated GPU)");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)	logger->Log("\t\t(Virtual GPU)");
    if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)		logger->Log("\t\t(CPU)");
    logger->Log("\tDevice Name: {}", PhysicalDeviceProperties.deviceName);
    logger->Log("\tPipeline Cache Size: {}", (int)PhysicalDeviceProperties.pipelineCacheUUID[0]);


    //Display PhysicalDevice Features
    VkPhysicalDeviceFeatures	PhysicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(IN handle, OUT &PhysicalDeviceFeatures);

    logger->Log("\n\tPhysical Device Features:");
    logger->Log("\t\tgeometryShader = {:2d}", (int)PhysicalDeviceFeatures.geometryShader);
    logger->Log("\t\ttessellationShader = {:2d}", (int)PhysicalDeviceFeatures.tessellationShader);
    logger->Log("\t\tmultiDrawIndirect = {:2d}", (int)PhysicalDeviceFeatures.multiDrawIndirect);
    logger->Log("\t\twideLines = {:2d}", (int)PhysicalDeviceFeatures.wideLines);
    logger->Log("\t\tlargePoints = {:2d}", (int)PhysicalDeviceFeatures.largePoints);
    logger->Log("\t\tmultiViewport = {:2d}", (int)PhysicalDeviceFeatures.multiViewport);
    logger->Log("\t\tocclusionQueryPrecise = {:2d}", (int)PhysicalDeviceFeatures.occlusionQueryPrecise);
    logger->Log("\t\tpipelineStatisticsQuery = {:2d}", (int)PhysicalDeviceFeatures.pipelineStatisticsQuery);
    logger->Log("\t\tshaderFloat64 = {:2d}", (int)PhysicalDeviceFeatures.shaderFloat64);
    logger->Log("\t\tshaderInt64 = {:2d}", (int)PhysicalDeviceFeatures.shaderInt64);
    logger->Log("\t\tshaderInt16 = {:2d}", (int)PhysicalDeviceFeatures.shaderInt16);
    logger->Log("\t\tsamplerAnisotropy = {:2d}", (int)PhysicalDeviceFeatures.samplerAnisotropy);
    

    VkFormatProperties					vfp;
    logger->Log("\n\tImage Formats Checked:");
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_R32G32B32A32_SFLOAT, &vfp);
    logger->Log("\t\tFormat VK_FORMAT_R32G32B32A32_SFLOAT: {:#10x}, {:#10x}, {:#10x}", (int)vfp.linearTilingFeatures, (int)vfp.optimalTilingFeatures, (int)vfp.bufferFeatures);

    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_R8G8B8A8_UNORM, &vfp);
    logger->Log("\t\tFormat VK_FORMAT_R8G8B8A8_UNORM: {:#10x}, {:#10x}, {:#10x}", (int)vfp.linearTilingFeatures, (int)vfp.optimalTilingFeatures, (int)vfp.bufferFeatures);
        
    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_B8G8R8A8_UNORM, &vfp);
    logger->Log("\t\tFormat VK_FORMAT_B8G8R8A8_UNORM: {:#10x}, {:#10x}, {:#10x}", (int)vfp.linearTilingFeatures, (int)vfp.optimalTilingFeatures, (int)vfp.bufferFeatures);

    vkGetPhysicalDeviceFormatProperties(handle, IN VK_FORMAT_B8G8R8A8_SRGB, &vfp);
    logger->Log("\t\tFormat VK_FORMAT_B8G8R8A8_SRGB: {:#10x}, {:#10x}, {:#10x}", (int)vfp.linearTilingFeatures, (int)vfp.optimalTilingFeatures, (int)vfp.bufferFeatures);

    VkPhysicalDeviceMemoryProperties			vpdmp;
    vkGetPhysicalDeviceMemoryProperties(handle, OUT &vpdmp);
    logger->Log("\n\t{} Memory Types:", (int)vpdmp.memoryTypeCount);
    for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
        VkMemoryType vmt = vpdmp.memoryTypes[i];
        VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
        logger->Log("\t\tMemory {:2d}: ", (int)i);
        if ((vmpf & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)	logger->Log("\t\t\tDeviceLocal");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)	logger->Log("\t\t\tHostVisible");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)	logger->Log("\t\t\tHostCoherent");
        if ((vmpf & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)	logger->Log("\t\t\tHostCached");
        if ((vmpf & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)	logger->Log(" LazilyAllocated");
        //logger->Log("");
    }

    logger->Log("\n\t{} Memory Heaps:", (int)vpdmp.memoryHeapCount);
    for (unsigned int i = 0; i < vpdmp.memoryHeapCount; i++) {
        logger->Log("\t\tHeap {}: ", (int)i);
        VkMemoryHeap vmh = vpdmp.memoryHeaps[i];
        logger->Log("\t\t\tsize = {:#10x}", (unsigned long int)vmh.size);
        if ((vmh.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)	logger->Log("\t\t\tDeviceLocal");
        if ((vmh.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT) != 0)	logger->Log("\t\t\tMultiInstance");
        //logger->Log("");
    }
    logger->Log("");

    // see what device layers are available:
    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(handle, &layerCount, (VkLayerProperties *)nullptr);
    VkLayerProperties * deviceLayers = new VkLayerProperties[layerCount];
    VkResult result = vkEnumerateDeviceLayerProperties(handle, &layerCount, deviceLayers);
    logger->Log("\n\tvkEnumerateDeviceLayerProperties");
    logger->Log("\t{} physical device layers enumerated:", (int)layerCount);
    for (unsigned int i = 0; i < layerCount; i++) {
        logger->Log("\t\t{}", deviceLayers[i].layerName);
        logger->Log("\t\t{}", deviceLayers[i].description);
        logger->Log("\t\t{:#10x}", (int)deviceLayers[i].specVersion);
        logger->Log("\t\t{:2d}", (int)deviceLayers[i].implementationVersion);
        

        // see what device extensions are available:
        //show layer related extension
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(handle, deviceLayers[i].layerName, &extensionCount, (VkExtensionProperties *)nullptr);
        VkExtensionProperties * deviceExtensions = new VkExtensionProperties[extensionCount];
        result = vkEnumerateDeviceExtensionProperties(handle, deviceLayers[i].layerName, &extensionCount, deviceExtensions);
        logger->Log("\t\tvkEnumerateDeviceExtensionProperties");

        logger->Log("\t\t{} device extensions enumerated", (int)extensionCount);
        //logger->Log("\t\tfor '%s':", deviceLayers[i].layerName);
        for (unsigned int ii = 0; ii < extensionCount; ii++) {
            logger->Log("\t\t\t{:#10x}", (int)deviceExtensions[ii].specVersion);
            logger->Log("\t\t\t{}", deviceExtensions[ii].extensionName);
        }
        logger->Log("");
    }
    delete[] deviceLayers;
    //debugger->flush();

    logger->Log("\n\tMax Uniform Buffer Range: {} bytes", (unsigned int)PhysicalDeviceProperties.limits.maxUniformBufferRange);
    logger->Log("  \tMax Storage Buffer Range: {} bytes", (unsigned int)PhysicalDeviceProperties.limits.maxStorageBufferRange);
    logger->Log("  \tMax Descriptor Set Uniform Buffers: {}", (unsigned int)PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
    logger->Log("  \tMax Descriptor Set Uniform Buffers Dynamic: {}", (unsigned int)PhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
    logger->Log("  \tMax Per Stage Descriptor Uniform Buffers: {}", (unsigned int)PhysicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers);
    logger->Log("  \tMin Uniform Buffer Offset Alignment: {}", (unsigned int)PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment);

    logger->Log("");
}
    
