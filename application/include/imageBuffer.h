#ifndef H_IMAGEMANAGER
#define H_IMAGEMANAGER

#include "common.h"
#include "context.h"
#include "dataBuffer.hpp"

//3 places use image buffer: depth, MSAA color, texture. 
//swap chain has vkimage and vkimageview but not acutally create image(but will create imageView), so it does't use image buffer
//image is always used together with device memory and imageView, so combine here
//image must call createImage; imageView must call createImageView(); swapchain imageview will also call createImageView()
class CWxjImageBuffer final{
public:
	VkImage	image;
	VkDeviceMemory deviceMemory;
	VkDeviceSize size;
    VkImageView view;

    CWxjImageBuffer();
    ~CWxjImageBuffer();

    void destroy();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) ;
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool bCubeMap,
        VkImageLayout layout);
    
    //Create 2 versions of this function. The void version is for texture, msaaImage, depthImage. VkImageView version is for swapchain imageview
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool bCubeMap);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool bCubeMap);

    //void createImageView_swapchain(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkImageView createImageView_swapchain(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    //CDebugger * debugger;//can not use debugger in ImageBuffer, because multiple image buffer is created at the same time for swapchain
};

#endif
