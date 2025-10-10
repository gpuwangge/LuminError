#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CTextureCompute
class TEST_CLASS_NAME: public CApplication{
//This test is similar to simpleComputeStorageImage, but instead use 2 texture image, one as input, the other as output
//It also render the result to swapchain
//simpleComputeStorageImage: write result to texture image and show to swapchain
//textureCompute: read texture image, make changes(use compute shader to do blur effect), and write to texture image, and show to swapchain
//This test use graphics pipeline but does not draw anything with it, because result is drawn directly to swap chain
//The reason for graphics pipeline is to load texture to texture image
//but why mvp? why color attachment?
//no need bind graphics pipeline?
//TODO: improve this sample, there is still something unknown for this test; something can be simplified
//A texture image is presented on the screen. Image is blured. char is blue/white. background is yellow.
public:
	static const int KernelRunNumber = 1;

	TEST_CLASS_NAME(){//these must be updated before initialization
		swapchain.swapchainImageSize = MAX_FRAMES_IN_FLIGHT;
		swapchain.bComputeSwapChainImage = true;
	}

	void initialize(){
        appInfo.RenderMode = renderer.COMPUTE_SWAPCHAIN;
		CApplication::initialize();
		createComputeCommandBuffers(renderer.commandBuffers[renderer.computeCmdId], swapchain.swapchain_images);
	}

	void update(){
		static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
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
            
			Dispatch(300,600,1);
         
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

