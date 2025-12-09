#pragma once

#define IN
#define OUT
#define INOUT

const int MAX_FRAMES_IN_FLIGHT = 2; //决定 CPU 最多提前提交几帧命令,必须少于等于 swapchain 图像数量
//举例说明
//假设：
//swapchainImageCount = 3
//MAX_FRAMES_IN_FLIGHT = 2
//Frame0: acquire image 0 -> submit -> GPU 渲染 image0
//Frame1: acquire image 1 -> submit -> GPU 渲染 image1
//Frame2: CPU 等待 fence0 -> acquire image 2 -> submit -> GPU 渲染 image2
//核心理解
//swapchain image → GPU resource
//frame in flight → CPU resource
//CPU 可以少于 GPU，因为 GPU 可以异步等待 semaphore 或 fence

//Vulkan 1.4以前， semaphore的数量跟MAX_FRAMES_IN_FLIGHT一致即可；1.4以后，semaphore的数量跟swapchain image数量一致更好

const int MIPMAP_TEXTURE_COUNT = 7;

//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr