#include "rendererCore.h"
#include <iostream>
#include "IGameEngine.h"
#include <windows.h>
#include "Foundation.h"
#include "context.h"
#include <glm/gtx/euler_angles.hpp>

namespace LERenderer{

void RendererCore::Update(){
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RendererCore::CreateVertexBuffer(void* data, size_t elementSize, size_t elementCount) {
    CWxjBuffer vertexDataBuffer;
    VkDeviceSize bufferSize = elementSize * elementCount;

    VkResult result = vertexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());

    vertexDataBuffer.fill(data, CContext::GetHandle().GetLogicalDevice());
    vertexDataBuffers.push_back(vertexDataBuffer);
}

void RendererCore::CreateIndexBuffer(std::vector<uint32_t> &indices3D){
    //Init05CreateIndexBuffer();
    CWxjBuffer indexDataBuffer;

	//HERE_I_AM("wxjCreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexDataBuffer);
    VkResult result = indexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());

	//REPORT("InitIndexDataBuffer");
    //FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
	indexDataBuffer.fill((void *)(indices3D.data()), CContext::GetHandle().GetLogicalDevice());

    indexDataBuffers.push_back(indexDataBuffer);
    indices3Ds.push_back(indices3D);
}


/**************************
 * Command Buffer Functions
 * ***********************/
void RendererCore::CreateCommandPool(VkSurfaceKHR &surface) {
    //HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices queueFamilyIndices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
    //QueueFamilyIndices queueFamilyIndices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool");
    QueueFamilyIndices queueFamilyIndices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool");

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    //poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();//find a queue family that does graphics
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    result = vkCreateCommandPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) throw std::runtime_error("failed to create graphics command pool!");
    //REPORT("vkCreateCommandPool -- Graphics");
}

void RendererCore::CreateGraphicsCommandBuffer(){
    graphicsCmdId = commandBuffers.size();
    //std::cout<<"graphicsCmdId="<<graphicsCmdId<<std::endl;
    CreateCommandBuffers();
}
void RendererCore::CreateComputeCommandBuffer(){
    computeCmdId = commandBuffers.size();
    CreateCommandBuffers();
}
void RendererCore::CreateRaytracingCommandBuffer(){
    raytracingCmdId = commandBuffers.size();
    CreateCommandBuffers();
}
void RendererCore::CreateCommandBuffers() {
    //commandBuffers.resize(size); //if enable both graphics and compute pipelines, set 2 commandBuffers: 0-GRAPHCIS, 1-COMPUTE
    std::vector<VkCommandBuffer> commandBuffer;

    //for(int i = 0; i < size; i++){
    VkResult result = VK_SUCCESS;

    commandBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    //commandBuffer.resize(3);///!!!

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffer.size();

    result = vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, commandBuffer.data());

    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate command buffers!");
    //REPORT("vkAllocateCommandBuffers");
    //}

    commandBuffers.push_back(commandBuffer);
}

/**************************
 * Universial Render Functions
 * ***********************/
//void RendererCore::AquireSwapchainImage(CSwapchain &swapchain){
void RendererCore::AquireSwapchainImage(VkSwapchainKHR swapchainHandle, bool bVerbose){
    if(bVerbose) std::cout<<"--------currentFrame = " << currentFrame <<"--------"<<std::endl;
    if(bVerbose) std::cout<<"vkAcquireNextImageKHR: imageAvailableSemaphores index = "<<semaphoreIndex%swapchain.swapchainImageSize<<", "<< imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize]<<std::endl;
    VkResult result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchainHandle, UINT64_MAX, imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize], VK_NULL_HANDLE, &currentImage);
    
    //VkSemaphore semaphore = availableSemaphores.front();
    //availableSemaphores.pop_front();
    //VkResult result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchainHandle, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);
}

void RendererCore::WaitForComputeFence(){
    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void RendererCore::WaitForRaytracingFence(){
    VkResult r = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &raytracingInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //std::cout << "Wait fence result = " << r << ", currentFrame = " << currentFrame << std::endl;
    VkResult s = vkGetFenceStatus(CContext::GetHandle().GetLogicalDevice(), raytracingInFlightFences[currentFrame]);
    //std::cout << "Fence status = " << s << ", currentFrame = " << currentFrame << std::endl;
}

void RendererCore::SubmitCompute(bool bVerbose){
    //if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    //    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    //}
    //imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    //vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &computeInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);

    //printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //this code handles compute semaphores
    switch(m_renderMode){
        case GRAPHICS:
            //Pure graphics application doesn't use compute pipeline
        break;
        case GRAPHICS_SHADOWMAP:
        break;
        case COMPUTE:
            //Pure compute application doesn't need swap image or present
        break;
        case COMPUTE_SWAPCHAIN:
        {
            //Because this mode use swap image to present, wait swap image to be ready
            VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize] }; //to wait until image is ready
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;

            //Also because this mode need present swap image, need to tell present that compute is finished
            VkSemaphore signalSemaphores[] = { computeFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize] }; 
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
        }
        break;
        case COMPUTE_GRAPHICS:
        {
            //This mode doesn't interact with swap image, this semaphore is to tell graphics that compute is finished
            VkSemaphore signalSemaphores[] = { computeFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize] }; 
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
        }
        break;
        default:
        break;
    }

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][currentFrame];///!!!

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &computeInFlightFences[currentFrame]);

    if (vkQueueSubmit(CContext::GetHandle().GetComputeQueue(), 1, &submitInfo, computeInFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute queue!");
    }
}


void RendererCore::SubmitRaytracing(bool bVerbose){
    VkCommandBuffer cmd = commandBuffers[raytracingCmdId][currentFrame];
    VkFence fence = raytracingInFlightFences[currentFrame];

    vkWaitForFences(GetLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(GetLogicalDevice(), 1, &fence);

    //std::cout<<"SubmitRaytracing: semaphoreIndex = "<<semaphoreIndex%swapchain.swapchainImageSize<<", currentFrame = "<<currentFrame<<std::endl;
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize] }; //currentFrame?
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSemaphore signalSemaphores[] = { raytracingFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize] };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;


    VkResult res = vkQueueSubmit(
        CContext::GetHandle().GetComputeQueue(),
        1,
        &submitInfo,
        fence);

    if (res != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to submit compute queue (raytracing pipeline)! VkResult = " +
            std::to_string(static_cast<int>(res)));
    }
}

void RendererCore::WaitForGraphicsFence(){
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//must call vkWaitForFences before record command buffer
    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
}

void RendererCore::SubmitGraphics(bool bVerbose){
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //this code handles graphics semaphores
    switch(m_renderMode){
        case GRAPHICS:
        {
            //pure graphics pipeline, need wait swap image is ready
            if(bVerbose) std::cout<<"WaitSemaphore: imageAvailableSemaphores index = "<<semaphoreIndex%swapchain.swapchainImageSize<<", currentFrame = "<<currentFrame<<std::endl;
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize] };
            
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }
        break;
        case GRAPHICS_SHADOWMAP:
        {
            //shadowmap pass, need wait for shadowmap to be ready
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }
        break;
        case COMPUTE:
            //pure compute application doen't need graphics pipeline
        break;
        case COMPUTE_SWAPCHAIN:
            //if render direct on swap image, doesn't need graphics pipeline
        break;
        case COMPUTE_GRAPHICS:
        {
            //graphics/compute pipeline hybrid, need wait both swap image and compute are ready
            VkSemaphore waitSemaphores[] = {computeFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize], imageAvailableSemaphores[semaphoreIndex%swapchain.swapchainImageSize] };
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 2;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }
        break;
        default:
        break;
    }
   
    // std::cout << "Frame " << currentFrame 
    //       << " acquired image " << imageIndex
    //       << ", using semaphore " << imageAvailableSemaphores[currentFrame]
    //       << ", image was in flight: " << (inFlightFences[imageIndex] != VK_NULL_HANDLE) //如果这个值是1，表示图像正在飞行中(正在被GPU使用)；如果是0，代表没有被GPU使用，正空闲，之前如果有工作的话已完成，可以被CPU使用
    //       << std::endl;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[graphicsCmdId][currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    //std::cout<<"before graphics submit. "<<std::endl;
    //GPU read recorded command buffer and execute
    if (vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        //debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    //std::cout<<"Done submit. "<<std::endl;

    //after command is submitted, reset command buffer
    //vkResetCommandBuffer(commandBuffers[graphicsCmdId][currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    
    
}

void RendererCore::PresentSwapchainImage(VkSwapchainKHR swapchainHandle, bool bVerbose){
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;


    presentInfo.waitSemaphoreCount = 1;
    VkSemaphore signalSemaphores[1];// = { computeFinishedSemaphores[currentFrame] }; //can not present until compute/graphics signal is ready renderFinishedSemaphores
    switch(m_renderMode){
        case GRAPHICS:
            //present only if render is finished
            if(bVerbose) std::cout<<"signalSemaphores: renderFinishedSemaphores index = "<<semaphoreIndex%swapchain.swapchainImageSize<< renderFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize]<<std::endl;
            signalSemaphores[0] = renderFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize]; 
        break;
        case GRAPHICS_SHADOWMAP:
            signalSemaphores[0] = renderFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize];
        break;
        case COMPUTE:
            //no need to present image for pure compute application
        break;
        case COMPUTE_SWAPCHAIN:
            //present only if compute is finished
            signalSemaphores[0] = computeFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize]; 
        break;
        case COMPUTE_GRAPHICS:
            //present only if render is finished
            signalSemaphores[0] = renderFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize]; 
        break;
        case RAYTRACING_SWAPCHAIN:
            //present only if raytracing is finished
            signalSemaphores[0] = raytracingFinishedSemaphores[semaphoreIndex%swapchain.swapchainImageSize];
            //signalSemaphores[0] = raytracingFinishedSemaphores[currentFrame];
        break;
        default:
        break;
    }
    

    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchainHandle };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &currentImage;

    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);

    semaphoreIndex++;
    if(bVerbose) std::cout<<"semaphoreIndex increase: " << semaphoreIndex<<std::endl;
}

/**************************
 * Graphics Functions
 * ***********************/
void RendererCore::CreateInitSyncObjects(){
    //Init Command Related
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    QueueFamilyIndices queueFamilyIndices = CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool");
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(GetLogicalDevice(), &poolInfo, nullptr, &initCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create rt init command pool!");
    }

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = initCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(GetLogicalDevice(), &allocInfo, &initCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate rt init command buffer!");
    }


    //Int Fence Related
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &rtInitFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create rtInitFence!");
    }
}
void RendererCore::CreateSyncObjects(int swapchainSize, bool bVerbose) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    //availableSemaphores.resize(swapchainSize);
    // for(int i = 0; i < swapchainSize; i++){
    //     if (vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &availableSemaphores[i]) != VK_SUCCESS){
    //             throw std::runtime_error("failed to create synchronization objects for a swapchain image!");
    //         }
    // }

    imageAvailableSemaphores.resize(swapchainSize);
    renderFinishedSemaphores.resize(swapchainSize);
    //computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    computeFinishedSemaphores.resize(swapchainSize);
    raytracingFinishedSemaphores.resize(swapchainSize);

    //computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    //imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    //imagesInFlight.resize(swapchainSize, VK_NULL_HANDLE);
    //imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    //imagesInFlight.resize(3, VK_NULL_HANDLE);///!!!

    
    for(int i = 0; i < swapchainSize; i++){
        if (vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &raytracingFinishedSemaphores[i]) != VK_SUCCESS){
                throw std::runtime_error("failed to create synchronization objects for a swapchain image!");
            }
        if(bVerbose){
            std::cout<<"Created imageAvailableSemaphores["<<i<<"]: "<<imageAvailableSemaphores[i]<<std::endl;
            std::cout<<"Created renderFinishedSemaphores["<<i<<"]: "<<renderFinishedSemaphores[i]<<std::endl;
            std::cout<<"Created computeFinishedSemaphores["<<i<<"]: "<<computeFinishedSemaphores[i]<<std::endl;
            std::cout<<"Created raytracingFinishedSemaphores["<<i<<"]: "<<raytracingFinishedSemaphores[i]<<std::endl;
        }
    }


    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    
    computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    raytracingInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS ||
            vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS ||
            vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &raytracingInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        //if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            //vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            //throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        //}
    }

}


void RendererCore::StartRecordGraphicsCommandBuffer(VkRenderPass &renderPass, 
        std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
        std::vector<VkClearValue> &clearValues){
    BeginCommandBuffer(graphicsCmdId);
    BeginRenderPass(renderPass, swapChainFramebuffers, extent, clearValues, false);
    SetViewport(extent);
    SetScissor(extent);
}
void RendererCore::EndRecordGraphicsCommandBuffer(){
	EndGraphicsRenderPass();
	EndCommandBuffer(graphicsCmdId);
}

void RendererCore::BeginCommandBuffer(int commandBufferIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffers[commandBufferIndex][currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
        std::cout<<"failed to begin recording command buffer!"<<std::endl;
    }
}

void RendererCore::BeginGraphicsCommandBuffer() {BeginCommandBuffer(graphicsCmdId);}
void RendererCore::BeginComputeCommandBuffer() {BeginCommandBuffer(computeCmdId);}

void RendererCore::BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues, bool useSingleFramebuffer){
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    if(useSingleFramebuffer) renderPassInfo.framebuffer = swapChainFramebuffers[0];
    else renderPassInfo.framebuffer = swapChainFramebuffers[currentImage];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;//swapchain.swapChainExtent;

    //std::array<VkClearValue, 2> clearValues{};
    // if (bEnableDepthTest) {
        // clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        // clearValues[1].depthStencil = { 1.0f, 0 };
        // renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        // renderPassInfo.pClearValues = clearValues.data();
    // }
    // else {
    	//clearValues[0].color = { {  0.0f, 0.0f, 0.0f, 1.0f  } };
    	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    	renderPassInfo.pClearValues = clearValues.data();
    //}

    //Step2
    vkCmdBeginRenderPass(commandBuffers[graphicsCmdId][currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void RendererCore::BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex){
	vkCmdBindPipeline(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipeline); //renderProcess.graphicsPipeline
}
void RendererCore::BindGraphicsPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint){
    BindPipeline(pipeline, pipelineBindPoint, graphicsCmdId);
}
void RendererCore::BindComputePipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint){
    BindPipeline(pipeline, pipelineBindPoint, computeCmdId);
}
void RendererCore::SetViewport(VkExtent2D &extent){
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = extent.width; //(float)swapchain.swapChainExtent.width;
	viewport.height = extent.height; //(float)swapchain.swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	//Step4
	vkCmdSetViewport(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &viewport);
}
void RendererCore::SetScissor(VkExtent2D &extent){
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent; //swapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &scissor);
}
void RendererCore::BindVertexBuffer(int modelId){
    //std::cout<<"objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<std::endl;
    if(vertexDataBuffers.size() <= 0) return;
	VkBuffer vertexBuffers[] = {vertexDataBuffers[modelId].buffer};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 1, vertexBuffers, offsets);
}
void RendererCore::BindVertexInstanceBuffer(int modelId, VkBuffer *pBuffer){    
    //std::cout<<"objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<", instanceDataBuffers.size()="<<instanceDataBuffers.size()<<std::endl;
    if(vertexDataBuffers.size() <= 0) return;
	VkBuffer vertexBuffers[] = {vertexDataBuffers[modelId].buffer, *pBuffer}; //textbox.instanceDataBuffer.buffer
	//std::cout<<"BindVertexInstanceBuffer: objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<", instanceBuffers.size()="<<instanceDataBuffers.size()<<std::endl;
    VkDeviceSize offsets[] = { 0, 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 2, vertexBuffers, offsets); //vertexBuffers here contains both vertex and instance buffer
}
void RendererCore::BindIndexBuffer(int modelId){
	vkCmdBindIndexBuffer(commandBuffers[graphicsCmdId][currentFrame], indexDataBuffers[modelId].buffer, 0, VK_INDEX_TYPE_UINT32);
}
void RendererCore::BindExternalBuffer(std::vector<CWxjBuffer> &buffer){
    VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &buffer[currentFrame].buffer, offsets);

}
void RendererCore::BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, 
        VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, 
        uint32_t dynamicObjectOffset, uint32_t dynamicTextOffset){
    //you can bind many descriptor sets for one mesh, they are identified in shader by set index
    //also, each descriptor set can have multiple writes, they are identified in shader by binding index
    //unsigned int setCount = 1;
    //VkDescriptorSet sets[setCount] = { descriptorSets[currentFrame] };
    unsigned int setCount = descriptorSets.size();
    //std::cout<<"setCount = "<<setCount<<std::endl;
    //VkDescriptorSet sets[setCount] = { descriptorSets[0][currentFrame] };
    //VkDescriptorSet sets[setCount] = { descriptorSets[0][currentFrame], descriptorSets[1][currentFrame] };
    VkDescriptorSet sets[setCount];
    for(unsigned int i = 0; i < setCount; i++){
        sets[i] = descriptorSets[i][currentFrame];
    }

    int object_size = sizeof(ObjectTextData);
    int text_size = sizeof(ObjectTextData);
    if(bEnableObject && bEnableText && pipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS){
        uint32_t offsets[2] ={object_size * dynamicObjectOffset, text_size * dynamicTextOffset};
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets,  
            2, //dynamicOffsetCount. # means there is (exact)# uniforms in the descriptor sets that are set to be dynamic 
            offsets 
        );
    }else if (bEnableObject && pipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS){
        uint32_t offsets[1] ={object_size * dynamicObjectOffset}; 
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets,  
            1,
            offsets 
        );
    }else if (bEnableText && pipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS){
        uint32_t offsets[1] ={text_size * dynamicTextOffset}; 
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets,  
            1,
            offsets 
        );
    }else{
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets, 
            0, 
            nullptr
        );
    }

    //Issue here: there are 2 descriptor sets. say [0]] is model transformation, [1] is texture
    //If set offset to a positive number, both transform and texture will have offset value
    //However the texture dont have offset at all
    //Reason is only uniform are set to be dynamic, so texture doesn't apply offset at all
    //So it is safe to set offset

    //vulkan specifies: dynamicOffsetCount should match dynamic uniform number
    //only use dynamic uniform offset
    //but how does renderer know this?
    //this function is called by CObject, so CObject should know this... (some object use model, some not...)
    //one solution is to force all object to use uniform...
    //second solution is to can set a bool variable bDisableModelMat. for multiCubes, bDisableModelMat=false; for furMark, bDisableModelMat=true
    //all objects must have texture, so must create texture descriptor set

}
void RendererCore::BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectOffset, uint32_t dynamicTextOffset){
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsCmdId, dynamicObjectOffset, dynamicTextOffset);
}
void RendererCore::BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets){
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
}
void RendererCore::BindRaytracingDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets){
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, raytracingCmdId);
}

void RendererCore::PushConstantToCommand(void* pcData, VkPipelineLayout graphicsPipelineLayout, VkPushConstantRange &pushConstantRange) {
    vkCmdPushConstants(commandBuffers[graphicsCmdId][currentFrame], graphicsPipelineLayout, 
                pushConstantRange.stageFlags, pushConstantRange.offset, 
                pushConstantRange.size, pcData);
}
void RendererCore::DrawIndexed(int model_id){
	//vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
    vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3Ds[model_id].size()), 1, 0, 0, 0);
}
void RendererCore::Draw(uint32_t n){
	vkCmdDraw(commandBuffers[graphicsCmdId][currentFrame], n, 1, 0, 0);
}
void RendererCore::DrawInstanceIndexed(int model_id, int instanceCount){
    vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3Ds[model_id].size()), instanceCount, 0, 0, 0);
}
void RendererCore::EndGraphicsRenderPass(){
	vkCmdEndRenderPass(commandBuffers[graphicsCmdId][currentFrame]);
}
void RendererCore::EndCommandBuffer(int commandBufferIndex){
	if (vkEndCommandBuffer(commandBuffers[commandBufferIndex][currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
void RendererCore::EndGraphicsCommandBuffer(){ EndCommandBuffer(graphicsCmdId); }
void RendererCore::EndComputeCommandBuffer(){ EndCommandBuffer(computeCmdId); }

/**************************
 * Compute Shader Functions
 * ***********************/
void RendererCore::StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout){
    BeginCommandBuffer(computeCmdId);
    BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
}
void RendererCore::EndRecordComputeCommandBuffer(){ EndCommandBuffer(computeCmdId); }

/**************************
 * Raytracing Shader Functions
 * ***********************/
void RendererCore::StartRecordRaytracingCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout){
    BeginCommandBuffer(raytracingCmdId);
    BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, raytracingCmdId);
}
void RendererCore::EndRecordRaytracingCommandBuffer(){ EndCommandBuffer(raytracingCmdId); }

/**************************
 * Utility Functions
 * ***********************/
void RendererCore::RecordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
            VkAccessFlags scrAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
            VkImageMemoryBarrier barrier{};
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcAccessMask = scrAccessMask;
            barrier.dstAccessMask = dstAccessMask;
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            VkImageSubresourceRange sub{};
            sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            sub.baseArrayLayer = 0;
            sub.baseMipLevel = 0;
            sub.layerCount = VK_REMAINING_MIP_LEVELS;
            sub.levelCount = VK_REMAINING_MIP_LEVELS;
            barrier.subresourceRange = sub;

            vkCmdPipelineBarrier(buffer, srcStageMask, dstStageMask,
                0, 0, nullptr, 0, nullptr, 1, &barrier);
        }


void RendererCore::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    vkCmdDispatch(commandBuffers[computeCmdId][currentFrame], numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ); 
}

void RendererCore::InitialRaytracing(){
    if (!LoadRayTracingFunctions_core()) {
		std::cout<<"failed to load ray tracing functions!"<<std::endl;
		throw std::runtime_error("failed to load ray tracing functions!");
	}
    //std::cout<<"Load ray tracing functions successfully!"<<std::endl;
    //std::cout<<"Create Triangle Acceleration Structure..."<<std::endl;
    CreateTriangleBlas();
    //std::cout<<"Create Sphere Acceleration Structure..."<<std::endl;
    CreateSphereBlas();
    //std::cout<<"Create Instance Acceleration Structure..."<<std::endl;
    CreateInstanceBuffer();
    //std::cout<<"Create Top Level Acceleration Structure..."<<std::endl;
    CreateTlas();
    //std::cout<<"Done create TLAS."<<std::endl;
}

void RendererCore::CreateSBT(){
    QueryRayTracingProperties();

    const uint32_t handleSize  = rayTracingPipelineProperties.shaderGroupHandleSize;
    const uint32_t handleAlign = rayTracingPipelineProperties.shaderGroupHandleAlignment;
    const uint32_t baseAlign   = rayTracingPipelineProperties.shaderGroupBaseAlignment;

    const uint32_t handleSizeAligned = AlignUp(handleSize, handleAlign);

    // 5 个 group: rgen, primary miss, shadow miss, primary hit, shadow hit
    const uint32_t groupCount = 7;
    const uint32_t rgenCount  = 1;
    const uint32_t missCount  = 2;
    const uint32_t hitCount   = 4;

    const uint32_t rgenStride = AlignUp(handleSizeAligned, baseAlign);
    const uint32_t missStride = AlignUp(handleSizeAligned, baseAlign);
    const uint32_t hitStride  = AlignUp(handleSizeAligned, baseAlign);

    const VkDeviceSize rgenSize = rgenStride * rgenCount;
    const VkDeviceSize missSize = missStride * missCount;
    const VkDeviceSize hitSize  = hitStride  * hitCount;

    const VkDeviceSize rgenRegionOffset = 0;
    const VkDeviceSize missRegionOffset = AlignUp((uint32_t)(rgenRegionOffset + rgenSize), (uint32_t)baseAlign);
    const VkDeviceSize hitRegionOffset  = AlignUp((uint32_t)(missRegionOffset + missSize), (uint32_t)baseAlign);

    const VkDeviceSize sbtSize = hitRegionOffset + hitSize;

    std::vector<uint8_t> handles(groupCount * handleSize);
    fpGetRayTracingShaderGroupHandlesKHR(
        GetLogicalDevice(),
        GetRaytracingPipeline(),
        0,
        groupCount,
        handles.size(),
        handles.data()
    );

    VkResult result = sbt_buffer.init(
        sbtSize,
        VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true
    );

    std::vector<uint8_t> sbtData((size_t)sbtSize, 0);

    auto copyHandle = [&](uint32_t groupIndex, VkDeviceSize dstOffset){
        memcpy(
            sbtData.data() + dstOffset,
            handles.data() + groupIndex * handleSize,
            handleSize
        );
    };

    // group 0 = rgen
    copyHandle(0, rgenRegionOffset + 0 * rgenStride);

    // miss region
    copyHandle(1, missRegionOffset + 0 * missStride); // group 1 = primary miss, triangle/sphere
    copyHandle(2, missRegionOffset + 1 * missStride); // group 2 = shadow miss, triangle/sphere

    // hit region
    copyHandle(3, hitRegionOffset + 0 * hitStride); // triangle primary hit (rchit and rahit)
    copyHandle(4, hitRegionOffset + 1 * hitStride); // triangle shadow hit (rchit and rahit)
    copyHandle(5, hitRegionOffset + 2 * hitStride); // sphere procedural primary, (rchit and intersection)
    copyHandle(6, hitRegionOffset + 3 * hitStride); // sphere procedural shadow, (rchit and intersection)

    sbt_buffer.fill(sbtData.data(), GetLogicalDevice());

    VkDeviceAddress addr = GetBufferAddress(GetLogicalDevice(), sbt_buffer.buffer);

    rgenRegion.deviceAddress = addr + rgenRegionOffset;
    rgenRegion.stride        = rgenStride;
    rgenRegion.size          = rgenSize;

    missRegion.deviceAddress = addr + missRegionOffset;
    missRegion.stride        = missStride;
    missRegion.size          = missSize;

    hitRegion.deviceAddress  = addr + hitRegionOffset;
    hitRegion.stride         = hitStride;
    hitRegion.size           = hitSize;

    callRegion.deviceAddress = 0;
    callRegion.stride        = 0;
    callRegion.size          = 0;
}

void RendererCore::Trace(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    //std::cout<<"Ray Trace..."<<std::endl;
    fpCmdTraceRaysKHR(commandBuffers[raytracingCmdId][currentFrame],
        &rgenRegion,
        &missRegion,
        &hitRegion,
        &callRegion,
        numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ
    );
}

bool RendererCore::LoadRayTracingFunctions_core(){
    fpGetRayTracingShaderGroupHandlesKHR =
        reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkGetRayTracingShaderGroupHandlesKHR"));

    fpGetBufferDeviceAddressKHR =
        reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkGetBufferDeviceAddressKHR"));

	fpCmdTraceRaysKHR =
        reinterpret_cast<PFN_vkCmdTraceRaysKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkCmdTraceRaysKHR"));

    fpCreateAccelerationStructureKHR =
        reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkCreateAccelerationStructureKHR"));

    fpDestroyAccelerationStructureKHR = //optional
        reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkDestroyAccelerationStructureKHR"));

    fpGetAccelerationStructureBuildSizesKHR = 
        reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkGetAccelerationStructureBuildSizesKHR"));

    fpGetAccelerationStructureDeviceAddressKHR =
        reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkGetAccelerationStructureDeviceAddressKHR"));

    fpCmdBuildAccelerationStructuresKHR =
        reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkCmdBuildAccelerationStructuresKHR"));

    fpBuildAccelerationStructuresKHR = //optional
        reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(
            vkGetDeviceProcAddr(GetLogicalDevice(), "vkBuildAccelerationStructuresKHR"));

	bool ok = true;

    if (!fpGetRayTracingShaderGroupHandlesKHR) {
        logger->Log("Missing vkGetRayTracingShaderGroupHandlesKHR\n");
        ok = false;
    }

    if (!fpGetBufferDeviceAddressKHR) {
        logger->Log("Missing vkGetBufferDeviceAddressKHR\n");
        ok = false;
    }

	if (!fpCmdTraceRaysKHR) {
        logger->Log("Missing vkCmdTraceRaysKHR\n");
        ok = false;
    }

    if (!fpCreateAccelerationStructureKHR) {
        logger->Log("Missing vkCreateAccelerationStructureKHR\n");
        ok = false;
    }
    if (!fpDestroyAccelerationStructureKHR) {
        logger->Log("Missing vkDestroyAccelerationStructureKHR\n");
        ok = false;
    }
    if (!fpGetAccelerationStructureBuildSizesKHR) {
        logger->Log("Missing vkGetAccelerationStructureBuildSizesKHR\n");
        ok = false;
    }
    if (!fpGetAccelerationStructureDeviceAddressKHR) {
        logger->Log("Missing vkGetAccelerationStructureDeviceAddressKHR\n");
        ok = false;
    }
    if (!fpCmdBuildAccelerationStructuresKHR) {
        logger->Log("Missing vkCmdBuildAccelerationStructuresKHR\n");
        ok = false;
    }
    if (!fpBuildAccelerationStructuresKHR) {
        logger->Log("Missing vkBuildAccelerationStructuresKHR\n");
        ok = false;
    }

	return ok;
}



VkDeviceAddress RendererCore::GetBufferAddress(VkDevice device, VkBuffer buffer) {
    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    info.buffer = buffer;
    return fpGetBufferDeviceAddressKHR(device, &info);
}

/*
void RendererCore::CreateTriangleVertexBuffer(){
    //std::cout << "Creating ray tracing triangle vertex/index buffers..." << std::endl;

    struct Vertex
    {
        float x, y, z;
    };

    std::vector<Vertex> vertexData =
    {
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        {  0.0f,  0.5f, 0.0f }
    };

    std::vector<uint32_t> indexData =
    {
        0, 1, 2
    };

    VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertexData.size();
    VkDeviceSize indexBufferSize  = sizeof(uint32_t) * indexData.size();

    VkBufferUsageFlags rtGeometryUsage =
        VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VkResult result = rt_vertex_buffer.init(
        vertexBufferSize,
        rtGeometryUsage,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true // need Device address
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create ray tracing vertex buffer!");
    }

    result = rt_index_buffer.init(
        indexBufferSize,
        rtGeometryUsage,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true // need Device address
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create ray tracing index buffer!");
    }

    rt_vertex_buffer.fill(vertexData.data(), GetLogicalDevice());
    rt_index_buffer.fill(indexData.data(), GetLogicalDevice());

    rt_vertex_buffer_address = GetBufferAddress(GetLogicalDevice(), rt_vertex_buffer.buffer);
    rt_index_buffer_address  = GetBufferAddress(GetLogicalDevice(), rt_index_buffer.buffer);

    triangleVertexCount = static_cast<uint32_t>(vertexData.size());
    triangleIndexCount  = static_cast<uint32_t>(indexData.size());
    triangleVertexStride = sizeof(Vertex);

    //std::cout << "Triangle vertex buffer created. Device Address: " << rt_vertex_buffer_address << std::endl;
    //std::cout << "Triangle index buffer created. Device Address: " << rt_index_buffer_address << std::endl;
    std::cout << "triangleVertexCount = " << triangleVertexCount <<std::endl;
    std::cout << "triangleIndexCount = " << triangleIndexCount <<std::endl;
    std::cout << "triangleVertexStride = " << triangleVertexStride <<std::endl;
}*/

void RendererCore::BeginCommandBuffer_Raytracing(int commandBufferIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    //if (vkBeginCommandBuffer(commandBuffers[commandBufferIndex][currentFrame], &beginInfo) != VK_SUCCESS) {
    if (vkBeginCommandBuffer(initCommandBuffer, &beginInfo) != VK_SUCCESS) {
        std::cout << "failed to begin recording command buffer!" << std::endl;
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

void RendererCore::EndCommandBuffer_Raytracing(int commandBufferIndex){
    // if (vkEndCommandBuffer(commandBuffers[commandBufferIndex][currentFrame]) != VK_SUCCESS) {
    //     throw std::runtime_error("failed to record command buffer!");
    // }
    if (vkEndCommandBuffer(initCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void RendererCore::SubmitCommandBufferAndWait_Raytracing(int commandBufferIndex, VkQueue queue){
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    //submitInfo.pCommandBuffers = &commandBuffers[commandBufferIndex][currentFrame];
    submitInfo.pCommandBuffers = &initCommandBuffer; // Use the dedicated command buffer for ray tracing initialization

    // 1) 先确保上一次使用这把 fence 的提交已经结束
    vkWaitForFences(GetLogicalDevice(), 1, &rtInitFence, VK_TRUE, UINT64_MAX);

    // 2) submit 前 reset，保证 fence 处于 unsignaled
    vkResetFences(GetLogicalDevice(), 1, &rtInitFence);

    // 3) 提交
    if (vkQueueSubmit(queue, 1, &submitInfo, rtInitFence) != VK_SUCCESS) {
        //vkDestroyFence(GetLogicalDevice(), rtInitFence, nullptr);
        throw std::runtime_error("failed to submit command buffer!");
    }

    // 4) 如果你这个函数语义就是 “submit and wait”，那就在这里等这次完成
    //std::cout << "  Waiting for ray tracing command buffer to finish..." << std::endl;
    vkWaitForFences(GetLogicalDevice(), 1, &rtInitFence, VK_TRUE, UINT64_MAX);
    //vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &rtInitFence);
    //std::cout << "  Ray tracing command buffer finished." << std::endl;
    // vkDestroyFence(GetLogicalDevice(), rtInitFence, nullptr);
}

void RendererCore::CreateTriangleBlas(){
    //std::cout << "Creating BLAS for triangles..." << std::endl;

    for(int i = 0; i < game->GetRtMeshSize(); i++){
        RtMesh &rtMesh = game->GetRtMesh(i);

        // 一个 indexed triangle => 1 primitive
        const uint32_t primitiveCount = rtMesh.indexCount / 3;//  game->GetTriangleIndexCount() / 3 ;//1;
        //std::cout<<"CreateBLAS: primitiveCount = "<<primitiveCount<<std::endl;

        // 1) triangles data
        VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
        triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        triangles.vertexData.deviceAddress = rtMesh.vertexAddress;// game->GetRaytracingVertexBufferAddress(); // rt_vertex_buffer_address;
        triangles.vertexStride = rtMesh.vertexStride;// game->GetTriangleVertexStride(); // triangleVertexStride;
        triangles.maxVertex = rtMesh.vertexCount - 1;// game->GetTriangleVertexCount() - 1; // triangleVertexCount - 1;
        triangles.indexType = VK_INDEX_TYPE_UINT32;
        triangles.indexData.deviceAddress = rtMesh.indexAddress;// game->GetRaytracingIndexBufferAddress();// rt_index_buffer_address;
        triangles.transformData.deviceAddress = 0;

        // 2) geometry
        VkAccelerationStructureGeometryKHR geometry{};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        geometry.geometry.triangles = triangles;

        // 3) build info (for size query first)
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
        buildInfo.dstAccelerationStructure = VK_NULL_HANDLE;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &geometry;

        // 4) query build sizes
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        fpGetAccelerationStructureBuildSizesKHR(
            GetLogicalDevice(),
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            &primitiveCount,
            &sizeInfo
        );

        // 5) create BLAS storage buffer
        VkResult result = rtMesh.blasBuffer.init(
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            true
        );

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create BLAS storage buffer!");
        }

        // 6) create BLAS object
        VkAccelerationStructureCreateInfoKHR asCreateInfo{};
        asCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        asCreateInfo.buffer = rtMesh.blasBuffer.buffer;
        asCreateInfo.offset = 0;
        asCreateInfo.size = sizeInfo.accelerationStructureSize;
        asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        asCreateInfo.deviceAddress = 0;

        result = fpCreateAccelerationStructureKHR(
            GetLogicalDevice(),
            &asCreateInfo,
            nullptr,
            &rtMesh.blas
        );

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create BLAS object!");
        }

        // 7) create scratch buffer
        result = blas_scratch_buffer.init(
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            true
        );

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create BLAS scratch buffer!");
        }

        VkDeviceAddress scratchAddress =
            GetBufferAddress(GetLogicalDevice(), blas_scratch_buffer.buffer);

        // 8) final build info
        buildInfo.dstAccelerationStructure = rtMesh.blas;
        buildInfo.scratchData.deviceAddress = scratchAddress;

        // 9) build range
        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = primitiveCount;
        rangeInfo.primitiveOffset = 0;
        rangeInfo.firstVertex = 0;
        rangeInfo.transformOffset = 0;

        const VkAccelerationStructureBuildRangeInfoKHR* pRangeInfo = &rangeInfo;

        // 10) record command buffer
        BeginCommandBuffer_Raytracing(raytracingCmdId);

        fpCmdBuildAccelerationStructuresKHR(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            1,
            &buildInfo,
            &pRangeInfo
        );

        VkMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

        vkCmdPipelineBarrier(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            0,
            1, &barrier,
            0, nullptr,
            0, nullptr
        );

        EndCommandBuffer_Raytracing(raytracingCmdId);

        // 11) submit and wait
        SubmitCommandBufferAndWait_Raytracing(raytracingCmdId, CContext::GetHandle().GetComputeQueue());

        // 12) query BLAS device address for TLAS instance
        VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = rtMesh.blas;

        rtMesh.blasAddress = fpGetAccelerationStructureDeviceAddressKHR(GetLogicalDevice(), &addressInfo);

        //std::cout << "BLAS created. Device Address: " << blasDeviceAddress << std::endl;

        blas_scratch_buffer.DestroyAndFree(GetLogicalDevice());
    }
}

void RendererCore::CreateSphereBlas(){
    //std::cout << "Creating BLAS for Spheres..." << std::endl;

    const uint32_t sphereSize = game->GetRtSphereSize();
    if (sphereSize == 0) return;

    // 1) CPU build AABB array
    //std::vector<VkAabbPositionsKHR> aabbs(sphereSize);
    //for (uint32_t i = 0; i < sphereSize; ++i) {
        uint32_t primitiveCount = 1; //aabb has one primitive, the sphere itself

        VkAabbPositionsKHR aabb{};
        aabb.minX = -1.0f;
        aabb.minY = -1.0f;
        aabb.minZ = -1.0f;
        aabb.maxX =  1.0f;
        aabb.maxY =  1.0f;
        aabb.maxZ =  1.0f;

        // RtSphere& s = game->GetRtSphere(i);
        // aabb.minX = s.center.x - s.radius;
        // aabb.minY = s.center.y - s.radius;
        // aabb.minZ = s.center.z - s.radius;
        // aabb.maxX = s.center.x + s.radius;
        // aabb.maxY = s.center.y + s.radius;
        // aabb.maxZ = s.center.z + s.radius;
        

        // 2) upload AABB buffer (device local preferred)
        //sphereAabbBuffer.resize(1);
        VkDeviceSize aabbBufferSize = sizeof(VkAabbPositionsKHR);// * sphereSize;
        // 1) staging buffer
        CWxjBuffer stagingBuffer;
        stagingBuffer.init(
            aabbBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            false // host visible
        );

        // map + copy
        void* mapped = nullptr;
        vkMapMemory(GetLogicalDevice(), stagingBuffer.deviceMemory, 0, stagingBuffer.GetSize(), 0, &mapped);
        memcpy(mapped, &aabb, (size_t)aabbBufferSize);

        //如果你的内存本来就是 HOST_COHERENT，那就没必要强行加 flush；这时 flush 只会增加无意义的调用成本
        VkMappedMemoryRange mappedRange{};
        mappedRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = stagingBuffer.deviceMemory;
        mappedRange.offset = 0;
        mappedRange.size   = stagingBuffer.GetSize();   // 用实际 allocation size; VK_WHOLE_SIZE; // 调试时最省事
        vkFlushMappedMemoryRanges(GetLogicalDevice(), 1, &mappedRange);

        vkUnmapMemory(GetLogicalDevice(), stagingBuffer.deviceMemory);

        // 2) device local AABB buffer
        CWxjBuffer sphereAabbBuffer;
        sphereAabbBuffer.init(
            aabbBufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            true
        );

        // 3) copy staging -> device local
        // CopyBuffer(
        //     stagingBuffer.buffer,
        //     sphereAabbBuffer.buffer,
        //     aabbBufferSize
        // );
        //sphereAabbBuffer.fill(stagingBuffer.buffer, GetLogicalDevice());//TODO: verify
        //stagingBuffer.fill(aabbs.data(), GetLogicalDevice());//no need
        BeginCommandBuffer_Raytracing(raytracingCmdId);
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = aabbBufferSize;
        vkCmdCopyBuffer(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            stagingBuffer.buffer,
            sphereAabbBuffer.buffer,
            1,
            &copyRegion
        );
        
        ///////////////
        VkBufferMemoryBarrier aabbBarrier{};
        aabbBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        aabbBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        aabbBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
        aabbBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        aabbBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        aabbBarrier.buffer = sphereAabbBuffer.buffer;
        aabbBarrier.offset = 0;
        aabbBarrier.size = aabbBufferSize;

        vkCmdPipelineBarrier(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            0,
            0, nullptr,
            1, &aabbBarrier,
            0, nullptr
        );
        ////////////

        //EndCommandBuffer_Raytracing(raytracingCmdId);

        // 4) later get device address
        VkDeviceAddress aabbAddress =
            GetBufferAddress(GetLogicalDevice(), sphereAabbBuffer.buffer);

        // 3) AABB data
        VkAccelerationStructureGeometryAabbsDataKHR aabbData{};
        aabbData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
        aabbData.data.deviceAddress = aabbAddress;
        aabbData.stride = sizeof(VkAabbPositionsKHR);

        // 4) geometry
        VkAccelerationStructureGeometryKHR geometry{};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
        geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR; // opaque sphere first
        geometry.geometry.aabbs = aabbData;

        // 5) build info
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &geometry;

        

        // 6) query size
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        fpGetAccelerationStructureBuildSizesKHR(
            GetLogicalDevice(),
            VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
            &buildInfo,
            &primitiveCount,
            &sizeInfo
        );

        // 7) create BLAS storage
        sphere_blasBuffer.init(
            sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            true
        );

        VkAccelerationStructureCreateInfoKHR asCreateInfo{};
        asCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        asCreateInfo.buffer = sphere_blasBuffer.buffer;
        asCreateInfo.offset = 0;
        asCreateInfo.size = sizeInfo.accelerationStructureSize;
        asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

        fpCreateAccelerationStructureKHR(
            GetLogicalDevice(),
            &asCreateInfo,
            nullptr,
            &sphere_blas
        );

        // 8) scratch
        CWxjBuffer blas_sphere_scratch_buffer;
        blas_sphere_scratch_buffer.init(
            sizeInfo.buildScratchSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            GetLogicalDevice(),
            GetPhysicalDevice(),
            true
        );

        buildInfo.dstAccelerationStructure = sphere_blas;
        buildInfo.scratchData.deviceAddress =
            GetBufferAddress(GetLogicalDevice(), blas_sphere_scratch_buffer.buffer);

        // 9) range
        VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
        rangeInfo.primitiveCount = primitiveCount;
        rangeInfo.primitiveOffset = 0;
        rangeInfo.firstVertex = 0;      // AABB时基本无意义，写0即可
        rangeInfo.transformOffset = 0;

        const VkAccelerationStructureBuildRangeInfoKHR* pRangeInfo = &rangeInfo;

        // 10) build
        //BeginCommandBuffer_Raytracing(raytracingCmdId);

        fpCmdBuildAccelerationStructuresKHR(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            1,
            &buildInfo,
            &pRangeInfo
        );

        VkMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

        vkCmdPipelineBarrier(
            //commandBuffers[raytracingCmdId][currentFrame],
            initCommandBuffer,
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            0,
            1, &barrier,
            0, nullptr,
            0, nullptr
        );

        EndCommandBuffer_Raytracing(raytracingCmdId);
        SubmitCommandBufferAndWait_Raytracing(raytracingCmdId, CContext::GetHandle().GetComputeQueue());

        //vkDeviceWaitIdle(GetLogicalDevice());//no use

        // 11) get address
        VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = sphere_blas;

        sphere_blasAddress =
            fpGetAccelerationStructureDeviceAddressKHR(GetLogicalDevice(), &addressInfo);

        
        stagingBuffer.DestroyAndFree(GetLogicalDevice());
        sphereAabbBuffer.DestroyAndFree(GetLogicalDevice());//如果后面要做 update/rebuild，这个输入 buffer 不能立刻销毁，因为 AABB 位置更新属于 update 允许修改的内容之一，但前提是 primitive 数量不能变。
        blas_sphere_scratch_buffer.DestroyAndFree(GetLogicalDevice());
    //}
}

void RendererCore::CreateInstanceBuffer(){
    //std::cout << "Creating TLAS instance buffer for one triangle..." << std::endl;

    const uint32_t sphereSize = game->GetRtSphereSize();

    instances.resize(game->GetObjectSize() + sphereSize);//每个instance就是一个object
    //Step1 for triangle mesh
    int count = 0;
    for(int i = 0; i < game->GetObjectSize(); i++){
        //VkAccelerationStructureInstanceKHR instance{};
        
        glm::vec3 scale = game->GetObjectScale(i);
        glm::vec3 position = game->GetObjectPosition(i);
        int model_id = game->GetObjectModelID(i);
        //std::cout<<"CreateInstanceBuffer(): model_id = "<<model_id<<std::endl;
        glm::vec3 rotation = game->GetObjectRotation(i); //Pitch, Yaw, Roll

        // 如果 rotation 是角度，要先转弧度
        float pitch = glm::radians(rotation.x);
        float yaw   = glm::radians(rotation.y);
        float roll  = glm::radians(rotation.z);

        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::yawPitchRoll(yaw, pitch, roll); // 注意参数顺序是 yaw, pitch, roll
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

        // 常见模型矩阵：先 scale，再 rotate，再 translate
        glm::mat4 M = T * R * S;

        // 拷贝到 VkTransformMatrixKHR 的 3x4
        instances[i].transform.matrix[0][0] = M[0][0];
        instances[i].transform.matrix[0][1] = M[1][0];
        instances[i].transform.matrix[0][2] = M[2][0];
        instances[i].transform.matrix[0][3] = M[3][0];

        instances[i].transform.matrix[1][0] = M[0][1];
        instances[i].transform.matrix[1][1] = M[1][1];
        instances[i].transform.matrix[1][2] = M[2][1];
        instances[i].transform.matrix[1][3] = M[3][1];

        instances[i].transform.matrix[2][0] = M[0][2];
        instances[i].transform.matrix[2][1] = M[1][2];
        instances[i].transform.matrix[2][2] = M[2][2];
        instances[i].transform.matrix[2][3] = M[3][2];

        instances[i].instanceCustomIndex = model_id;
        instances[i].mask = 0x01; //triangle设置成01，那么在shader中traceRay时，ray的mask也必须cover 01才能命中这个instance
        instances[i].instanceShaderBindingTableRecordOffset = 0;
        instances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        instances[i].accelerationStructureReference = game->GetRtMesh(model_id).blasAddress;
        //std::cout<<"instance "<<i<<" : use model_id="<<model_id<<", use blasAddress="<<game->GetRtMesh(model_id).blasAddress<<std::endl;
        count++;
    }
    //step2 for sphere
    for(int i = 0; i < sphereSize; i++){
        RtSphere& s = game->GetRtSphere(i);

        // VkTransformMatrixKHR identity = {
        //     1.0f, 0.0f, 0.0f, 0.0f,
        //     0.0f, 1.0f, 0.0f, 0.0f,
        //     0.0f, 0.0f, 1.0f, 0.0f
        // };

        // unit sphere in object space:
        // center = (0,0,0), radius = 1
        // instance transform => scale by radius, then translate to center
        VkTransformMatrixKHR transform = {
            s.radius, 0.0f,     0.0f,     s.center.x,
            0.0f,     s.radius, 0.0f,     s.center.y,
            0.0f,     0.0f,     s.radius, s.center.z
        };
        instances[count+i].transform = transform;

        instances[count+i].instanceCustomIndex = s.materialIndex; //use this index to fetch material data in shader
        instances[count+i].mask = 0x02; //sphere设置成02，那么在shader中traceRay时，ray的mask也必须cover 02才能命中这个instance
        instances[count+i].instanceShaderBindingTableRecordOffset = 2; //there are 3 hit records in the hit SBT region: 0/1/2, group 2 is for sphere procedual hit
        instances[count+i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR; //this doesn't matter here
        instances[count+i].accelerationStructureReference = sphere_blasAddress;
    }

    //std::vector<VkAccelerationStructureInstanceKHR> instances = { instance };

    VkDeviceSize instanceBufferSize = sizeof(VkAccelerationStructureInstanceKHR) * instances.size();

    VkResult result = instance_buffer.init(
        instanceBufferSize,
        VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create TLAS instance buffer!");
    }

    instance_buffer.fill(instances.data(), GetLogicalDevice());

    instanceBufferAddress = GetBufferAddress(GetLogicalDevice(), instance_buffer.buffer);

    //instanceCount = static_cast<uint32_t>(instances.size());

    //std::cout << "TLAS instance buffer created. Device Address: " << instanceBufferAddress << std::endl;

}

void RendererCore::CreateTlas(){
    //std::cout << "Creating TLAS for one triangle instance..." << std::endl;

    const uint32_t primitiveCount = instances.size();

    // 1) instance data
    VkAccelerationStructureGeometryInstancesDataKHR instancesData{};
    instancesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    instancesData.arrayOfPointers = VK_FALSE;
    instancesData.data.deviceAddress = instanceBufferAddress;

    // 2) geometry
    VkAccelerationStructureGeometryKHR geometry{};
    geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    geometry.geometry.instances = instancesData;

    // 3) build info for size query
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
    buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.dstAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &geometry;

    // 4) query build sizes
    VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
    sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    fpGetAccelerationStructureBuildSizesKHR(
        GetLogicalDevice(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &buildInfo,
        &primitiveCount,
        &sizeInfo
    );

    // 5) create TLAS storage buffer
    VkResult result = tlas_buffer.init(
        sizeInfo.accelerationStructureSize,
        VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create TLAS storage buffer!");
    }

    // 6) create TLAS object
    VkAccelerationStructureCreateInfoKHR asCreateInfo{};
    asCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    asCreateInfo.buffer = tlas_buffer.buffer;
    asCreateInfo.offset = 0;
    asCreateInfo.size = sizeInfo.accelerationStructureSize;
    asCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    asCreateInfo.deviceAddress = 0;

    result = fpCreateAccelerationStructureKHR(
        GetLogicalDevice(),
        &asCreateInfo,
        nullptr,
        &tlas
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create TLAS object!");
    }

    // 7) create scratch buffer
    result = tlas_scratch_buffer.init(
        sizeInfo.buildScratchSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        GetLogicalDevice(),
        GetPhysicalDevice(),
        true
    );

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create TLAS scratch buffer!");
    }

    VkDeviceAddress scratchAddress =
        GetBufferAddress(GetLogicalDevice(), tlas_scratch_buffer.buffer);

    // 8) final build info
    buildInfo.dstAccelerationStructure = tlas;
    buildInfo.scratchData.deviceAddress = scratchAddress;

    // 9) build range
    VkAccelerationStructureBuildRangeInfoKHR rangeInfo{};
    rangeInfo.primitiveCount = primitiveCount;
    rangeInfo.primitiveOffset = 0;
    rangeInfo.firstVertex = 0;
    rangeInfo.transformOffset = 0;

    const VkAccelerationStructureBuildRangeInfoKHR* pRangeInfo = &rangeInfo;

    // 10) record command buffer
    int cmdId = raytracingCmdId;
    BeginCommandBuffer_Raytracing(cmdId);

    fpCmdBuildAccelerationStructuresKHR(
        //commandBuffers[cmdId][currentFrame],
        initCommandBuffer,
        1,
        &buildInfo,
        &pRangeInfo
    );

    VkMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
    barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    vkCmdPipelineBarrier(
        //commandBuffers[cmdId][currentFrame],
        initCommandBuffer,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        0,
        1, &barrier,
        0, nullptr,
        0, nullptr
    );

    EndCommandBuffer_Raytracing(cmdId);

    // 11) submit and wait
    SubmitCommandBufferAndWait_Raytracing(cmdId, CContext::GetHandle().GetComputeQueue());

    // 12) query TLAS device address (optional but useful)
    VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
    addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure = tlas;

    tlasDeviceAddress = fpGetAccelerationStructureDeviceAddressKHR(GetLogicalDevice(), &addressInfo);

    //std::cout << "TLAS created. Device Address: " << tlasDeviceAddress << std::endl;
}


void RendererCore::QueryRayTracingProperties(){
    //std::cout<<"Raytraycing Pipeline: Querying ray tracing properties..."<<std::endl;
    rayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

    VkPhysicalDeviceProperties2 deviceProperties2{};
    deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProperties2.pNext = &rayTracingPipelineProperties;
    vkGetPhysicalDeviceProperties2(GetPhysicalDevice(), &deviceProperties2);

    // std::cout<<"Ray Tracing Pipeline Properties:"<<std::endl;
    // std::cout<<"    shaderGroupHandleSize: "<<rayTracingPipelineProperties.shaderGroupHandleSize<<std::endl;
    // std::cout<<"    maxRayRecursionDepth: "<<rayTracingPipelineProperties.maxRayRecursionDepth<<std::endl;
    // std::cout<<"    maxShaderGroupStride: "<<rayTracingPipelineProperties.maxShaderGroupStride<<std::endl;
    // std::cout<<"    shaderGroupBaseAlignment: "<<rayTracingPipelineProperties.shaderGroupBaseAlignment<<std::endl;
    // std::cout<<"    shaderGroupHandleCaptureReplaySize: "<<rayTracingPipelineProperties.shaderGroupHandleCaptureReplaySize<<std::endl;
    // std::cout<<"    maxRayDispatchInvocationCount: "<<rayTracingPipelineProperties.maxRayDispatchInvocationCount<<std::endl;
    // std::cout<<"    shaderGroupHandleAlignment: "<<rayTracingPipelineProperties.shaderGroupHandleAlignment<<std::endl;
    // std::cout<<"    maxRayHitAttributeSize: "<<rayTracingPipelineProperties.maxRayHitAttributeSize<<std::endl;

    logger->Log("Ray Tracing Pipeline Properties:");
    logger->Log("    shaderGroupHandleSize: {}", rayTracingPipelineProperties.shaderGroupHandleSize);
    logger->Log("    maxRayRecursionDepth: {}", rayTracingPipelineProperties.maxRayRecursionDepth);
    logger->Log("    maxShaderGroupStride: {}", rayTracingPipelineProperties.maxShaderGroupStride);
    logger->Log("    shaderGroupBaseAlignment: {}", rayTracingPipelineProperties.shaderGroupBaseAlignment);
    logger->Log("    shaderGroupHandleCaptureReplaySize: {}", rayTracingPipelineProperties.shaderGroupHandleCaptureReplaySize);
    logger->Log("    maxRayDispatchInvocationCount: {}", rayTracingPipelineProperties.maxRayDispatchInvocationCount);
    logger->Log("    shaderGroupHandleAlignment: {}", rayTracingPipelineProperties.shaderGroupHandleAlignment);
    logger->Log("    maxRayHitAttributeSize: {}\n", rayTracingPipelineProperties.maxRayHitAttributeSize);
}

/**************************
 * Clean up Function
 * ***********************/

void RendererCore::Destroy(){
    //std::cout<<"Begin Destroy RenderCore(): vertexDataBuffers/indexDataBuffers"<<std::endl;
    for(size_t i = 0; i < vertexDataBuffers.size(); i++) vertexDataBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    vertexDataBuffers.clear();
    for(size_t i = 0; i < indexDataBuffers.size(); i++) indexDataBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    //for(size_t i = 0; i < instanceDataBuffers.size(); i++) instanceDataBuffers[i].DestroyAndFree();
    indexDataBuffers.clear();

    // for(int i = 0; i< availableSemaphores.size(); i++){
    //     vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), availableSemaphores[i], nullptr);
    // }

    for(int i = 0; i< renderFinishedSemaphores.size(); i++){
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), computeFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), raytracingFinishedSemaphores[i], nullptr);
    }

    //std::cout<<"Begin Destroy RenderCore(): sync objects"<<std::endl;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), inFlightFences[i], nullptr);
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), computeInFlightFences[i], nullptr);
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), raytracingInFlightFences[i], nullptr);
    }
    vkDestroyFence(GetLogicalDevice(), rtInitFence, nullptr);

    renderFinishedSemaphores.clear();
    imageAvailableSemaphores.clear();
    inFlightFences.clear();
    computeInFlightFences.clear();
    computeFinishedSemaphores.clear();
    raytracingInFlightFences.clear();
    raytracingFinishedSemaphores.clear();

    vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), commandPool, nullptr);
    commandPool = VK_NULL_HANDLE;

    vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), initCommandPool, nullptr);
    initCommandPool = VK_NULL_HANDLE;

    //Module Related
    logger->CloseLogFile();
    DestroyInstance(handle_module_logcore, logger);
    if (handle_module_logcore) {
        //std::cout<<"- FreeLibrary: handle_module_example. (~Application())"<<std::endl;
        FreeLibrary(handle_module_logcore);
        handle_module_logcore = nullptr;
    }

    if (tlas != VK_NULL_HANDLE) {
        fpDestroyAccelerationStructureKHR(GetLogicalDevice(), tlas, nullptr);
        tlas = VK_NULL_HANDLE;
    }

    for(int i = 0; i < game->GetRtMeshSize(); i++){
        RtMesh &rtMesh = game->GetRtMesh(i);
        if (rtMesh.blas != VK_NULL_HANDLE) {
            fpDestroyAccelerationStructureKHR(GetLogicalDevice(), rtMesh.blas, nullptr);
            rtMesh.blas = VK_NULL_HANDLE;
        }
    }
    // for(int i = 0; i < game->GetRtSphereSize(); i++){
    //     RtSphere &rtsphere = game->GetRtSphere(i);
    //     if (rtsphere.blas != VK_NULL_HANDLE) {
    //         fpDestroyAccelerationStructureKHR(GetLogicalDevice(), rtsphere.blas, nullptr);
    //         rtsphere.blas = VK_NULL_HANDLE;
    //     }
    // }
    if (sphere_blas != VK_NULL_HANDLE) {
        fpDestroyAccelerationStructureKHR(GetLogicalDevice(), sphere_blas, nullptr);
        sphere_blas = VK_NULL_HANDLE;
    }

    //std::cout<<"----Now free the SBT buffer----"<<std::endl;
    sbt_buffer.DestroyAndFree(GetLogicalDevice());

    //rt_vertex_buffer.DestroyAndFree(GetLogicalDevice());
    //rt_index_buffer.DestroyAndFree(GetLogicalDevice());

    //blas_buffer.DestroyAndFree(GetLogicalDevice());
    //blas_scratch_buffer.DestroyAndFree(GetLogicalDevice());
    sphere_blasBuffer.DestroyAndFree(GetLogicalDevice());

    instance_buffer.DestroyAndFree(GetLogicalDevice());

    tlas_buffer.DestroyAndFree(GetLogicalDevice());
    tlas_scratch_buffer.DestroyAndFree(GetLogicalDevice());

    //sphereAabbBuffer.DestroyAndFree(GetLogicalDevice());
    //blas_sphere_scratch_buffer.DestroyAndFree(GetLogicalDevice());
}

void RendererCore::DestroyInstance(HMODULE handle, void* instance){
    //std::cout<<"Application::DestroyInstance()"<<std::endl;
    if (instance) {
        using DestroyInstanceFunc = void(*)(void*);
        auto DestroyInstance =  (DestroyInstanceFunc)GetProcAddress(handle, "DestroyInstance");
        if(!DestroyInstance) { 
            std::cerr << "GetProcAddress failed! (DestroyInstance)" << std::endl;
            FreeLibrary(handle);
            return;
        }
        DestroyInstance(instance);
        instance = nullptr;
    }
}

/**************************
 * System Function
 * ***********************/
void RendererCore::SetApplication(LEGameEngine::IGameEngine* pApplication) {
    game = pApplication;
    renderProcess.game = game;
    graphicsDescriptorManager.game = game;
    //computeDescriptorManager.game = game;
    computeDescriptorManager.p_swapchain = &swapchain;
    raytracingDescriptorManager.p_swapchain = &swapchain;
    swapchain.game = game;

    void* pVoid = nullptr;
        //Load Log Core Module
    LoadModuleAndInstance(handle_module_logcore, pVoid, "core_log.dll");
    //logger = std::shared_ptr<LELog::ILogCore>(static_cast<LELog::ILogCore*>(pVoid));
    logger = static_cast<LELog::ILogCore*>(pVoid);

    std::string logName = logger->GetLogFileName("renderer");
    std::string folderPath = logger->CreateDateFolder(LOG_PATH);
    std::string fullLogName = folderPath + "/" + logName;
    //std::cout<<"fullLogName: "<<fullLogName<<std::endl;
    //mkdir(LOG_PATH);
    logger->SetLogFile(fullLogName);
    // logger->Print("Application started");// 演示所有用例
    // logger->Print("Integer: {}", 42); // 基本类型
    // logger->Print("Float: {}", 3.14f);
    // logger->Print("String: {}", "Hello");
    // logger->Print("Multiple: {}, {}, {}", 1, "test", 2.5f);// 多个参数
    //glm::vec3 vec(1.0f, 2.0f, 3.0f);// glm 向量
    //logger->Print("Vector: ({}, {}, {})", vec.x, vec.y, vec.z);
    //int numbers[] = {10, 20, 30};// 数组
    //logger->Print("Array: {}, {}, {}", numbers[0], numbers[1], numbers[2]);
    // logger->Log("Log Application started");// 演示所有用例
    // logger->Log("Log Integer: {}", 42); // 基本类型
    // logger->Log("Log Float: {}", 3.14f);
    // logger->Log("Log String: {}", "Hello");
    // logger->Log("Log Multiple: {}, {}, {}", 1, "test", 2.5f);// 多个参数
    // logger->Log("Log Vector: ({}, {}, {})", vec.x, vec.y, vec.z);
    // logger->Log("Log Array: {}, {}, {}", numbers[0], numbers[1], numbers[2]);

    swapchain.logger = logger;
}
void RendererCore::LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName){
    handle = LoadLibraryA(moduleName.c_str()); 
    if(!handle) { 
        std::cerr << "Module load failed! Module Name = " << moduleName << std::endl; 
        return; 
    }

    using CreateInstanceFunc = void*(*)();
    auto CreateInstance =  (CreateInstanceFunc)GetProcAddress(handle, "CreateInstance");
    if(!CreateInstance) { 
        std::cerr << "GetProcAddress failed! (CreateInstance_Module) Module Name = " << moduleName << std::endl;
        FreeLibrary(handle);
        instance = nullptr;
        return;
    }
    
    instance = CreateInstance();
    if (!instance) {
        std::cerr << "CreateInstance failed!" << std::endl;
        FreeLibrary(handle);
        handle = nullptr;
        return;
    }

}


}//end of namespace