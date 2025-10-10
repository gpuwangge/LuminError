#ifndef H_SWAPCHAIN
#define H_SWAPCHAIN

#include "vulkan\vulkan.h"
#include "physicalDevice.h"
#include "context.h"
#include "imageBuffer.h"
#include "logManager.h"

//Swapchain has all attachment resources(images, imageViews) for rendering
//frambuffer wraps the swapchain imageViews, so that it can be used in renderPass

class CSwapchain final{
    VkSwapchainKHR handle{VK_NULL_HANDLE};
public:
    CSwapchain();
    ~CSwapchain();
    void CleanUp();

    /**********************
    * Attachment IDs
    **********************/
    int iShadowmapAttachmentDepthLight = -1; //this is for shadowmap renderpass, not main scene renderpass
    int iMainSceneAttachmentDepthLight = -1; //this is for shadowmap in main scene renderpass
    int iMainSceneAttachmentDepthCamera = -1;
    int iMainSceneAttachmentColorResovle = -1;
    int iMainSceneAttachmentColorPresent = -1;

    /**********************
    * Resources: Which attachment will use this resource
    * Total 4 resources
    **********************/
    //Resource#1.buffer_depthlight
    //for iShadowmapAttachmentDepthLight/iMainSceneAttachmentDepthLight to use
    std::vector<CWxjImageBuffer> buffer_depthlight; //for different lights each need a separate buffer
    void create_attachment_resource_depthlight(VkSampleCountFlagBits msaaSamples);

    //Resource#2.buffer_depthcamera
    //for iMainSceneAttachmentDepthCamera to use
    VkFormat depthFormat;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) ;
    VkFormat findDepthFormat();
    CWxjImageBuffer buffer_depthcamera;
    void create_attachment_resource_depthcamera();

    //Resource#3.buffer_colorresolve
    //for iMainSceneAttachmentColorResovle to use
    VkSampleCountFlagBits msaaSamples;
    CWxjImageBuffer  buffer_colorresolve;
    void create_attachment_resource_colorresolve();

    //Resource#4.swapchain_images and swapchain_views
    //for iMainSceneAttachmentColorPresent to use
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_views;
    uint32_t swapchainImageSize = 0;
    void createSwapchainImages(VkSurfaceKHR surface, int width, int height);
    void createSwapchainViews(VkImageAspectFlags aspectFlags);


    /************************
    * Framebuffers
    ************************/
    std::vector<std::vector<VkFramebuffer>> framebuffers_shadowmap;//a 2D vector, framebuffers_shadowmap[i][j] is for the ith shadowmap, which has j attachment (j is normally 1)
    void CreateFramebuffer_shadowmap(VkRenderPass &renderPass, int shadowmapIndex); //will use Resource#1

    std::vector<VkFramebuffer> framebuffers_mainscene;
    void CreateFramebuffer_mainscene(VkRenderPass &renderPass);  //will use Resource#1/2/3/4



    /************************
    * Helper Variable and Functions
    ************************/
    VkFormat swapChainImageFormat;//08
	VkExtent2D swapChainExtent;//08
    CLogManager logManager;

    VkSwapchainKHR getHandle() const{ return handle;}
    bool bComputeSwapChainImage = false; //added VK_IMAGE_USAGE_STORAGE_BIT for image storage

    void GetMaxUsableSampleCount();
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
    void displaySwapchainInfo(SwapChainSupportDetails swapChainSupport);
    //bool CheckFormatSupport(VkPhysicalDevice gpu, VkFormat format, VkFormatFeatureFlags requestedSupport);

    //CDebugger * debugger;
};

#endif