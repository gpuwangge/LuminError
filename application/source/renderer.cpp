#include "../include/renderer.h"

CRenderer::CRenderer(){
    currentFrame = 0;
    imageIndex = 0;
    //debugger = new CDebugger("../logs/renderer.log");
    graphicsCmdId = 0;
    computeCmdId = 0;
}
CRenderer::~CRenderer(){
    //if (!debugger) delete debugger;
}

void CRenderer::Update(){
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CRenderer::CreateIndexBuffer(std::vector<uint32_t> &indices3D){
    //Init05CreateIndexBuffer();
    CWxjBuffer indexDataBuffer;

	//HERE_I_AM("wxjCreateIndexBuffer");
    VkDeviceSize bufferSize = sizeof(indices3D[0]) * indices3D.size();

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexDataBuffer);
    VkResult result = indexDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

	//REPORT("InitIndexDataBuffer");
    //FillDataBufferHelper(indexDataBuffer, (void *)(indices3D.data()));
	indexDataBuffer.fill((void *)(indices3D.data()));

    indexDataBuffers.push_back(indexDataBuffer);
    indices3Ds.push_back(indices3D);
}

// void CRenderer::CreateInstanceBuffer(std::vector<TextInstanceData> &instanceData){
//     CWxjBuffer instanceDataBuffer;
//     VkDeviceSize bufferSize = sizeof(instanceData[0]) * instanceData.size();
//     VkResult result = instanceDataBuffer.init(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//     instanceDataBuffer.fill((void *)(instanceData.data()));
//     instanceDataBuffers.push_back(instanceDataBuffer);
// }


/**************************
 * 
 * Command Buffer Functions
 * 
 * ***********************/
void CRenderer::CreateCommandPool(VkSurfaceKHR &surface) {
    //HERE_I_AM("Init06CommandPools");

    VkResult result = VK_SUCCESS;

    //QueueFamilyIndices queueFamilyIndices = instance->pickedPhysicalDevice->get()->findQueueFamilies(surface);
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

void CRenderer::CreateGraphicsCommandBuffer(){
    graphicsCmdId = commandBuffers.size();
    //std::cout<<"graphicsCmdId="<<graphicsCmdId<<std::endl;
    CreateCommandBuffers();
}
void CRenderer::CreateComputeCommandBuffer(){
    computeCmdId = commandBuffers.size();
    CreateCommandBuffers();
}
void CRenderer::CreateCommandBuffers() {
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

// void CRenderer::CreateComputeCommandBuffers() {
// 	commandBuffers_compute.resize(MAX_FRAMES_IN_FLIGHT);

// 	VkCommandBufferAllocateInfo allocInfo{};
// 	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
// 	allocInfo.commandPool = commandPool;
// 	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
// 	allocInfo.commandBufferCount = (uint32_t)commandBuffers_compute.size();

// 	if (vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, commandBuffers_compute.data()) != VK_SUCCESS) { //compute recordCommandBuffer需要用这个buffer： recordCompute CommandBuffer(commandBuffers_compute[currentFrame]);
// 		throw std::runtime_error("failed to allocate compute command buffers!");
// 	}
// }


/**************************
 * 
 * Universial Render Functions
 * 
 * ***********************/
void CRenderer::AquireSwapchainImage(CSwapchain &swapchain){
    VkResult result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
}

void CRenderer::WaitForComputeFence(){
    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void CRenderer::SubmitCompute(){
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
            VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] }; //to wait until image is ready
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;

            //Also because this mode need present swap image, need to tell present that compute is finished
            VkSemaphore signalSemaphores[] = { computeFinishedSemaphores[currentFrame] }; 
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;
        }
        break;
        case COMPUTE_GRAPHICS:
        {
            //This mode doesn't interact with swap image, this semaphore is to tell graphics that compute is finished
            VkSemaphore signalSemaphores[] = { computeFinishedSemaphores[currentFrame] }; 
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
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void CRenderer::WaitForGraphicsFence(){
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//must call vkWaitForFences before record command buffer
    //Validation Error: vkBeginCommandBuffer() on active VkCommandBuffer 0x8c99500[] before it has completed. 
    //You must check command buffer fence before this call. 
    //The Vulkan spec states: commandBuffer must not be in the recording or pending state
}

void CRenderer::SubmitGraphics(){
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //this code handles graphics semaphores
    switch(m_renderMode){
        case GRAPHICS:
        {
            //pure graphics pipeline, need wait swap image is ready
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }
        break;
        case GRAPHICS_SHADOWMAP:
        {
            //shadowmap pass, need wait for shadowmap to be ready
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
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
            VkSemaphore waitSemaphores[] = {computeFinishedSemaphores[currentFrame], imageAvailableSemaphores[currentFrame] };
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 2;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
        }
        break;
        default:
        break;
    }
   

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[graphicsCmdId][currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
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

void CRenderer::PresentSwapchainImage(CSwapchain &swapchain){
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;


    presentInfo.waitSemaphoreCount = 1;
    VkSemaphore signalSemaphores[1];// = { computeFinishedSemaphores[currentFrame] }; //can not present until compute/graphics signal is ready renderFinishedSemaphores
    switch(m_renderMode){
        case GRAPHICS:
            //present only if render is finished
            signalSemaphores[0] = renderFinishedSemaphores[currentFrame]; 
        break;
        case GRAPHICS_SHADOWMAP:
            signalSemaphores[0] = renderFinishedSemaphores[currentFrame];
        break;
        case COMPUTE:
            //no need to present image for pure compute application
        break;
        case COMPUTE_SWAPCHAIN:
            //present only if compute is finished
            signalSemaphores[0] = computeFinishedSemaphores[currentFrame]; 
        break;
        case COMPUTE_GRAPHICS:
            //present only if render is finished
            signalSemaphores[0] = renderFinishedSemaphores[currentFrame]; 
        break;
        default:
        break;
    }
    

    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);
}

/**************************
 * 
 * Graphics Functions
 * 
 * ***********************/
/*
void CRenderer::preRecordGraphicsCommandBuffer(CSwapchain &swapchain){//prepareCurrentFrameAndAcquireImageIndex
    //printf("currentFrame: %d, ", currentFrame);

    // VkSubmitInfo submitInfo{};
    // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //CPU-GPU sync use fence
    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    //if(result == VK_SUCCESS) printf("frame is ready. ");
    //else printf("waiting for frame ready. ");

    //CPU need to know which framebuffer to draw,GPU notify CPU the image is ready or not
    //GPU-GPU sync use semaphore
    //semaphore check if image is ready or not. imageIndex is the ready image.
    result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    //if(result == VK_SUCCESS) printf("acquired next image %d. \n", imageIndex);
    //else printf("Unable to aquire next image. \n");

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[graphicsCmdId][currentFrame],  0);//VkCommandBufferResetFlagBits
}

void CRenderer::postRecordGraphicsCommandBuffer(CSwapchain &swapchain) {//drawFrame
    //printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[graphicsCmdId][currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    //GPU read recorded command buffer and execute
    if (vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        //debugger->writeMSG("Failed to submit draw command buffer! CurrentFrame: %d\n", currentFrame);
        throw std::runtime_error("failed to submit draw command buffer!");
    }



    VkPresentInfoKHR presentInfo{};
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;


    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    //present engine read image the present(present imageIndex)
    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);
}*/

void CRenderer::CreateSyncObjects(int swapchainSize) {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    //computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    //computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);;
    computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);;

    //imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    //imagesInFlight.resize(swapchainSize, VK_NULL_HANDLE);
    //imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    //imagesInFlight.resize(3, VK_NULL_HANDLE);///!!!

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS ||
            vkCreateFence(CContext::GetHandle().GetLogicalDevice(), &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS ||
            vkCreateSemaphore(CContext::GetHandle().GetLogicalDevice(), &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
        //if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
            //vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
            //throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        //}
    }
}


void CRenderer::StartRecordGraphicsCommandBuffer(VkRenderPass &renderPass, 
        std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent,
        std::vector<VkClearValue> &clearValues){
    BeginCommandBuffer(graphicsCmdId);
    BeginRenderPass(renderPass, swapChainFramebuffers, extent, clearValues, false);
    SetViewport(extent);
    SetScissor(extent);
}
void CRenderer::EndRecordGraphicsCommandBuffer(){
	EndRenderPass();
	EndCommandBuffer(graphicsCmdId);
}

void CRenderer::BeginCommandBuffer(int commandBufferIndex){
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    //Step1
    if (vkBeginCommandBuffer(commandBuffers[commandBufferIndex][currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
        std::cout<<"failed to begin recording command buffer!"<<std::endl;
    }
}
void CRenderer::BeginRenderPass(VkRenderPass &renderPass, std::vector<VkFramebuffer> &swapChainFramebuffers, VkExtent2D &extent, std::vector<VkClearValue> &clearValues, bool useSingleFramebuffer){
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    if(useSingleFramebuffer) renderPassInfo.framebuffer = swapChainFramebuffers[0];
    else renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
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
void CRenderer::BindPipeline(VkPipeline &pipeline, VkPipelineBindPoint pipelineBindPoint, int commandBufferIndex){
	vkCmdBindPipeline(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipeline); //renderProcess.graphicsPipeline
}
void CRenderer::SetViewport(VkExtent2D &extent){
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
void CRenderer::SetScissor(VkExtent2D &extent){
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent; //swapchain.swapChainExtent;
    vkCmdSetScissor(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &scissor);
}
void CRenderer::BindVertexBuffer(int modelId){
    //std::cout<<"objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<std::endl;
    if(vertexDataBuffers.size() <= 0) return;
	VkBuffer vertexBuffers[] = {vertexDataBuffers[modelId].buffer};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 1, vertexBuffers, offsets);
}
void CRenderer::BindVertexInstanceBuffer(int modelId, CTextbox &textbox){
    //std::cout<<"objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<", instanceDataBuffers.size()="<<instanceDataBuffers.size()<<std::endl;
    if(vertexDataBuffers.size() <= 0) return;
	VkBuffer vertexBuffers[] = {vertexDataBuffers[modelId].buffer, textbox.instanceDataBuffer.buffer};
	//std::cout<<"BindVertexInstanceBuffer: objectId="<<objectId<<", vertexDataBuffers.size()="<<vertexDataBuffers.size()<<", instanceBuffers.size()="<<instanceDataBuffers.size()<<std::endl;
    VkDeviceSize offsets[] = { 0, 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 2, vertexBuffers, offsets); //vertexBuffers here contains both vertex and instance buffer
}
void CRenderer::BindIndexBuffer(int modelId){
	vkCmdBindIndexBuffer(commandBuffers[graphicsCmdId][currentFrame], indexDataBuffers[modelId].buffer, 0, VK_INDEX_TYPE_UINT32);
}
void CRenderer::BindExternalBuffer(std::vector<CWxjBuffer> &buffer){
    VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[graphicsCmdId][currentFrame], 0, 1, &buffer[currentFrame].buffer, offsets);

}
void CRenderer::BindDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, 
        VkPipelineBindPoint pipelineBindPoint, uint32_t commandBufferIndex, 
        uint32_t dynamicObjectMVPOffset, uint32_t dynamicTextboxMVPOffset){
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

    if(bUseObjectMVP && bUseTextboxMVP){
        uint32_t offsets[2] ={512 * dynamicObjectMVPOffset, 512 * dynamicTextboxMVPOffset};
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets,  
            2, //dynamicOffsetCount. # means there is (exact)# uniforms in the descriptor sets that are set to be dynamic 
            offsets 
        );
    }else if (bUseObjectMVP){
        uint32_t offsets[1] ={512 * dynamicObjectMVPOffset}; 
        vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex][currentFrame], pipelineBindPoint, pipelineLayout, 0, 
            setCount, sets,  
            1,
            offsets 
        );
    }else if (bUseTextboxMVP){
        uint32_t offsets[1] ={512 * dynamicTextboxMVPOffset}; 
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
    


    //Issue here: there are 2 descriptor sets. say [0]] is mvp, [1] is texture
    //If set offset to a positive number, both mvp and texture will have offset value
    //However the texture dont have offset at all
    //Reason is only VP/MVP uniform are set to be dynamic, so texture doesn't apply offset at all
    //So it is safe to set offset

    //vulkan specifies: dynamicOffsetCount should match dynamic uniform number
    //only mvp/vp use dynamic uniform offset
    //but how does renderer know this?
    //this function is called by CObject, so CObject should know this... (some object use mvp, some not...)
    //one solution is to force all object to use MVP/VP uniform...
    //second solution is to can set a bool variable bDisableModelMat. for multiCubes, bDisableModelMat=false; for furMark, bDisableModelMat=true
    //all objects must have texture, so must create texture descriptor set

}
void CRenderer::BindGraphicsDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets, uint32_t dynamicObjectMVPOffset, uint32_t dynamicTextboxMVPOffset){
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsCmdId, dynamicObjectMVPOffset, dynamicTextboxMVPOffset);
}
void CRenderer::BindComputeDescriptorSets(VkPipelineLayout &pipelineLayout, std::vector<std::vector<VkDescriptorSet>> &descriptorSets){
    BindDescriptorSets(pipelineLayout, descriptorSets, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
}

void CRenderer::DrawIndexed(int model_id){
	//vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3D.size()), 1, 0, 0, 0);
    vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3Ds[model_id].size()), 1, 0, 0, 0);
}
void CRenderer::Draw(uint32_t n){
	vkCmdDraw(commandBuffers[graphicsCmdId][currentFrame], n, 1, 0, 0);
}
void CRenderer::DrawInstanceIndexed(int model_id, int instanceCount){
    vkCmdDrawIndexed(commandBuffers[graphicsCmdId][currentFrame], static_cast<uint32_t>(indices3Ds[model_id].size()), instanceCount, 0, 0, 0);
}
void CRenderer::EndRenderPass(){
	vkCmdEndRenderPass(commandBuffers[graphicsCmdId][currentFrame]);
}
void CRenderer::EndCommandBuffer(int commandBufferIndex){
	if (vkEndCommandBuffer(commandBuffers[commandBufferIndex][currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

/**************************
 * 
 * Compute Shader Functions
 * 
 * ***********************/
/*
void CRenderer::preRecordComputeCommandBuffer(CSwapchain &swapchain){ //prepareCurrentFrame
   
    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    //uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    //if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    //    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    //}
    //imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[imageIndex], VK_TRUE, UINT64_MAX);

    //printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][currentFrame];///!!!

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(CContext::GetHandle().GetComputeQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }





    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);



    VkResult result = vkWaitForFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    result = vkAcquireNextImageKHR(CContext::GetHandle().GetLogicalDevice(), swapchain.getHandle(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);//

    //updateUniformBuffer_compute(currentFrame);
    vkResetFences(CContext::GetHandle().GetLogicalDevice(), 1, &inFlightFences[currentFrame]);
    //vkResetCommandBuffer(commandBuffers[graphicsCmdId][currentFrame],  0);////////////////////
    
  
}*/
/*
void CRenderer::postRecordComputeCommandBuffer(CSwapchain &swapchain){
    
    printf("currentFrame: %d, imageIndex: %d \n", currentFrame, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame] };//
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//
    submitInfo.waitSemaphoreCount = 1;//
    submitInfo.pWaitSemaphores = waitSemaphores;//
    submitInfo.pWaitDstStageMask = waitStages;//
    
    submitInfo.commandBufferCount = 1;
    //submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][currentFrame];
    submitInfo.pCommandBuffers = &commandBuffers[computeCmdId][imageIndex];//

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };//
    submitInfo.signalSemaphoreCount = 1;//
    submitInfo.pSignalSemaphores = signalSemaphores;//
    //submitInfo.signalSemaphoreCount = 1;//
    //submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];//

    if (vkQueueSubmit(CContext::GetHandle().GetComputeQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute command buffer!");
    }; 


    VkPresentInfoKHR presentInfo{};//
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;//

    //VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };//
    presentInfo.waitSemaphoreCount = 1;//
    presentInfo.pWaitSemaphores = signalSemaphores;//

    VkSwapchainKHR swapChains[] = { swapchain.getHandle() };//
    presentInfo.swapchainCount = 1;//
    presentInfo.pSwapchains = swapChains;//

    presentInfo.pImageIndices = &imageIndex;//

    VkResult result = vkQueuePresentKHR(CContext::GetHandle().GetPresentQueue(), &presentInfo);//
    
}*/

void CRenderer::StartRecordComputeCommandBuffer(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout){
    BeginCommandBuffer(computeCmdId);
    BindPipeline(pipeline, VK_PIPELINE_BIND_POINT_COMPUTE, computeCmdId);
}
void CRenderer::EndRecordComputeCommandBuffer(){
	//EndRenderPass();
	EndCommandBuffer(computeCmdId);
}

void CRenderer::Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    vkCmdDispatch(commandBuffers[computeCmdId][currentFrame], numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ); 
}


/**************************
 * 
 * Clean up Function
 * 
 * ***********************/

void CRenderer::Destroy(){
    for(size_t i = 0; i < vertexDataBuffers.size(); i++)  vertexDataBuffers[i].DestroyAndFree();
    for(size_t i = 0; i < indexDataBuffers.size(); i++) indexDataBuffers[i].DestroyAndFree();
    //for(size_t i = 0; i < instanceDataBuffers.size(); i++) instanceDataBuffers[i].DestroyAndFree();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), inFlightFences[i], nullptr);

        vkDestroyFence(CContext::GetHandle().GetLogicalDevice(), computeInFlightFences[i], nullptr);
        vkDestroySemaphore(CContext::GetHandle().GetLogicalDevice(), computeFinishedSemaphores[i], nullptr);
    }

    vkDestroyCommandPool(CContext::GetHandle().GetLogicalDevice(), commandPool, nullptr);
}



