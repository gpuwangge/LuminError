#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleComputeStorageImage
class TEST_CLASS_NAME: public CApplication{
//This test draws on the swapchain images, with the use of storage image
//The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
//Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
//Thus no graphics pipeline is needed here
//When run, a purple rectangle will show on the screen

public:
	std::vector<VkClearValue> clearValues{ {  0.0f, 1.0f, 0.0f, 1.0f  } };

	CSimpleComputeStorageImage(){
		swapchain.swapchainImageSize = MAX_FRAMES_IN_FLIGHT;
		swapchain.bComputeSwapChainImage = true;
	}

	void initialize(){
        appInfo.RenderMode = renderer.COMPUTE_SWAPCHAIN;
		CApplication::initialize();
		createComputeCommandBuffers(renderer.commandBuffers[renderer.computeCmdId], swapchain.swapchain_images);
	}

	void update(){
		CApplication::update();
	}

	void recordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind) {
        VkImageMemoryBarrier barrier{};
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcAccessMask = scrAccess;
        barrier.dstAccessMask = dstAccess;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        VkImageSubresourceRange sub{};
        sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        sub.baseArrayLayer = 0;
        sub.baseMipLevel = 0;
        sub.layerCount = VK_REMAINING_MIP_LEVELS;
        sub.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.subresourceRange = sub;

        vkCmdPipelineBarrier(buffer, srcBind, dstBind,
            0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

	void createComputeCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, std::vector<VkImage> &swapChainImages) {
        for (size_t i = 0; i < commandBuffers.size(); i++) {
			renderer.currentFrame = i;
			//std::cout<<"commandbuffer i: "<<i<<std::endl;
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            //if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to begin recording command buffer!");
            //}
			renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);


            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            
			
			Dispatch(200,300,1);
         
            recordImageBarrier(commandBuffers[i], swapChainImages[i],
                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

            //if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            //    throw std::runtime_error("failed to record command buffer!");
            //}
			renderer.EndRecordComputeCommandBuffer();
        }
		renderer.currentFrame = 0;
    }
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

