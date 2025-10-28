#ifndef H_IMAGEMANAGER
#define H_IMAGEMANAGER

#include "context.h"
#include <vulkan/vulkan.h>

//3 places use image buffer: depth, MSAA color, texture. 
//swap chain has vkimage and vkimageview but not acutally create image(but will create imageView), so it does't use image buffer
//image is always used together with device memory and imageView, so combine here
//image must call createImage; imageView must call createImageView(); swapchain imageview will also call createImageView()
class CWxjImageBuffer final{
public:
    CWxjImageBuffer() {}
    ~CWxjImageBuffer() {}

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
            VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool bCubeMap,
            VkImageLayout layout){
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        if(bCubeMap){
            //imageInfo.extent.width = width / 6; //horizontal skybox
            //imageInfo.extent.height = height;
            imageInfo.extent.width = width / 4; //standard skybox
            imageInfo.extent.height = height / 3;
            imageInfo.arrayLayers = 6; //for cubemap
            imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;//for cubemap
        }else{
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.arrayLayers = 1;
        }
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = layout;//VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        

        if (vkCreateImage(CContext::GetHandle().GetLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }
        //printf("Created VkImage: %p\n", image);

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), image, &memRequirements);//this is different from buffer allocation

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        size = allocInfo.allocationSize;

        vkBindImageMemory(CContext::GetHandle().GetLogicalDevice(), image, deviceMemory, 0);
    }
    
    //Create 2 versions of this function. The void version is for texture, msaaImage, depthImage. VkImageView version is for swapchain imageview
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool bCubeMap){
        view = createImageView(image, format, aspectFlags, mipLevels, bCubeMap);
    }

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool bCubeMap){
        //Concept of imageView that something can change how we present the image, but no need to change the image itself
        //Imageviews are commonly used in Vulkan, all images should have their imageviews.

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        if(bCubeMap){
            viewInfo.subresourceRange.layerCount = 6; //for cubemap
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE; //for cubmap
        }else{
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        }

        //Components change texture color for any channel
        //VK_COMPONENT_SWIZZLE_IDENTITY is default value
        //VK_COMPONENT_SWIZZLE_ZERO will set channel value to 0
        //VK_COMPONENT_SWIZZLE_R will set the channel value to r channel's value
        //VK_COMPONENT_SWIZZLE_ONE will set the channel value to maximum
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;

        VkImageView imageView;
        if (vkCreateImageView(CContext::GetHandle().GetLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    //void createImageView_swapchain(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkImageView createImageView_swapchain(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels){
        //Concept of imageView that something can change how we present the image, but no need to change the image itself
        //Imageviews are commonly used in Vulkan, all images should have their imageviews.

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ///viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE; //for cubmap
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.layerCount = 1;
        ///viewInfo.subresourceRange.layerCount = 6; //for cubemap

        //Components change texture color for any channel
        //VK_COMPONENT_SWIZZLE_IDENTITY is default value
        //VK_COMPONENT_SWIZZLE_ZERO will set channel value to 0
        //VK_COMPONENT_SWIZZLE_R will set the channel value to r channel's value
        //VK_COMPONENT_SWIZZLE_ONE will set the channel value to maximum
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;

        VkImageView imageView;
        if (vkCreateImageView(CContext::GetHandle().GetLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    //CDebugger * debugger;//can not use debugger in ImageBuffer, because multiple image buffer is created at the same time for swapchain

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void destroy(){
        if(size != (VkDeviceSize)0){
        vkDestroyImage(CContext::GetHandle().GetLogicalDevice(), image, nullptr);
        vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), deviceMemory, nullptr);
        if(view != NULL)
            vkDestroyImageView(CContext::GetHandle().GetLogicalDevice(), view, nullptr);
        }
    }

    VkImage	image;
	VkDeviceMemory deviceMemory;
	VkDeviceSize size = 0;
    VkImageView view = NULL;
};

#endif
