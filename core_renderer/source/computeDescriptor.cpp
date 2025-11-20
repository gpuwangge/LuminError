#include "computeDescriptor.h"
#include <iostream>
#include "IGameEngine.h"
#include "context.h"

namespace LERenderer{

//Declare static variables here:
/************
* Pool
************/
int CComputeDescriptorManager::computeUniformTypes;
VkDescriptorPool CComputeDescriptorManager::computeDescriptorPool;
std::vector<VkDescriptorPoolSize> CComputeDescriptorManager::computeDescriptorPoolSizes;
void CComputeDescriptorManager::createDescriptorPool(){
    //Descriptor Step 1/3

	computeDescriptorPoolSizes.resize(getPoolSize());
	int counter = 0;
    //std::cout<<"createDescriptorPool::textureSamplers.size() = " << textureSamplers.size()<<std::endl;
    std::cout<<"Compute Pool size = " << getPoolSize()<<std::endl;

    if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL){
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){
        //std::cout<<": Storage Buffer(2)";
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
        //}
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
    }
    if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM){
        //std::cout<<": Custom Buffer";
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
    if(computeUniformTypes & COMPUTE_STORAGEBUFFER_CUSTOMSWAP){
        //std::cout<<": Storage Buffer(2)";
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
        //}
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
        //std::cout<<": Storage Image(for Texture)";
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
        counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN){
        //std::cout<<": Storage Image(for Swapchain Presentation)";
        computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	    computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
        counter++;
    }
    //std::cout<<std::endl;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(computeDescriptorPoolSizes.size());
    //std::cout<<"poolInfo.poolSizeCount = "<<poolInfo.poolSizeCount <<std::endl;
	poolInfo.pPoolSizes = computeDescriptorPoolSizes.data();
	poolInfo.maxSets = ((counter==0)?1:counter)*10*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!TODO: currently support 10 sets?

	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &computeDescriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	//REPORT("vkCreateDescriptorPool");
}

/************
 * Layout
 ************/
std::vector<VkDescriptorSetLayoutBinding> CComputeDescriptorManager::computeBindings;
VkDescriptorSetLayout CComputeDescriptorManager::descriptorSetLayout;
void CComputeDescriptorManager::createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding){
    //Descriptor Step 2/3

    computeBindings.resize(getLayoutSize());
	int counter = 0;
    std::cout<<"Layout(Compute) size = " << getLayoutSize()<<std::endl;
    //std::cout.flush();
    if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL){
        VkDescriptorSetLayoutBinding binding = StructComputeGlobalUniformBuffer::GetBinding();
        computeBindings[counter].binding = counter;
		computeBindings[counter].descriptorCount = binding.descriptorCount;
		computeBindings[counter].descriptorType = binding.descriptorType;
		computeBindings[counter].pImmutableSamplers = binding.pImmutableSamplers;
		computeBindings[counter].stageFlags = binding.stageFlags;
		counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
        //}
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
    }
    if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM){
        computeBindings[counter].binding = counter;
		computeBindings[counter].descriptorCount = customBinding->descriptorCount;
		computeBindings[counter].descriptorType = customBinding->descriptorType;
		computeBindings[counter].pImmutableSamplers = customBinding->pImmutableSamplers;
		computeBindings[counter].stageFlags = customBinding->stageFlags;
		counter++;
	}
    if(computeUniformTypes & COMPUTE_STORAGEBUFFER_CUSTOMSWAP){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
        //}
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
    }
    if(computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN){
        computeBindings[counter].binding = counter;
        computeBindings[counter].descriptorCount = 1;
        computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        computeBindings[counter].pImmutableSamplers = nullptr;
        computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;
    }

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(computeBindings.size());
	layoutInfo.pBindings = computeBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	//REPORT("vkCreateDescriptorSetLayout");
}

/************
 * Set
 ************/
void CComputeDescriptorManager::createDescriptorSets(VkImageView textureImageView, std::vector<VkImageView> *swapchainImageViews){
    //Descriptor Step 3/3
    //HERE_I_AM("wxjCreateDescriptorSets");

    int descriptorSize = getSetSize();
    //std::cout<<"Set(Compute) size = "<<getSetSize()<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = computeDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    //std::cout<<"before vkAllocateDescriptorSets(). "<<std::endl;
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets.data());
    //std::cout<<"after vkAllocateDescriptorSets(). "<<std::endl;
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");
    //REPORT("vkAllocateDescriptorSets");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {///!!!
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        //VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
        //VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

        descriptorWrites.resize(descriptorSize);
        int counter = 0;

        VkDescriptorBufferInfo globalBufferInfo{}; //for global
        if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL){
            //std::cout<<"Global3"<<std::endl;
            globalBufferInfo.buffer = m_globalUniformBuffers[i].buffer;
            globalBufferInfo.offset = 0;
            globalBufferInfo.range = sizeof(StructComputeGlobalUniformBuffer);
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &globalBufferInfo;
            counter++;
        }

        //std::cout<<"m_storageBufferSize = "<<m_storageBufferSize<<std::endl;
        if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){ //for storage buffer 1
            VkDescriptorBufferInfo storageBufferInfo_1{};
            storageBufferInfo_1.buffer = storageBuffers_windowswap[(i - 1) % MAX_FRAMES_IN_FLIGHT].buffer; //storage buffer of last frame in flight as compute shader input
            storageBufferInfo_1.offset = 0;
            storageBufferInfo_1.range = sizeof(StructStorageBuffer_WindowSwap);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo_1;
            counter++;
            //}
            VkDescriptorBufferInfo storageBufferInfo_2{};
            //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){ //for storage buffer 2
            storageBufferInfo_2.buffer = storageBuffers_windowswap[i].buffer; //storage buffer of the current frame in flight as compute shader output
            storageBufferInfo_2.offset = 0;
            storageBufferInfo_2.range = sizeof(StructStorageBuffer_WindowSwap);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo_2;
            counter++;
        }

        //std::cout<<"m_customUniformBufferSize = "<<m_customUniformBufferSize<<std::endl;
        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
        if(computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM){
            customBufferInfo.buffer = customUniformBuffers[i].buffer;
            customBufferInfo.offset = 0;
            customBufferInfo.range = m_customUniformBufferSize;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &customBufferInfo;
            counter++;
        }

        if(computeUniformTypes & COMPUTE_STORAGEBUFFER_CUSTOMSWAP){ //for storage buffer 1
            VkDescriptorBufferInfo storageBufferInfo_1{};
            storageBufferInfo_1.buffer = storageBuffers_customswap[(i - 1) % MAX_FRAMES_IN_FLIGHT].buffer; //storage buffer of last frame in flight as compute shader input
            storageBufferInfo_1.offset = 0;
            storageBufferInfo_1.range = m_storageBufferSize_customswap;//sizeof(uint32_t) * 4;//sizeof(Particle) * PARTICLE_COUNT;

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo_1;
            counter++;
            //}
            VkDescriptorBufferInfo storageBufferInfo_2{};
            //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){ //for storage buffer 2
            storageBufferInfo_2.buffer = storageBuffers_customswap[i].buffer; //storage buffer of the current frame in flight as compute shader output
            storageBufferInfo_2.offset = 0;
            storageBufferInfo_2.range = m_storageBufferSize_customswap;

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo_2;
            counter++;
        }

        //std::cout<<"UNIFORM_IMAGE_STORAGE_TEXTURE_BIT"<<std::endl;
        if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
            VkDescriptorImageInfo storageImageInfo{};
            storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            storageImageInfo.imageView = textureImageView;
            storageImageInfo.sampler = VK_NULL_HANDLE; //textureSamplers[0];

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &storageImageInfo;
            counter++;
        }

        //std::cout<<"UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT"<<std::endl;
        if(computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN){
            VkDescriptorImageInfo storageImageInfo{};
            storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            storageImageInfo.imageView = (*swapchainImageViews)[i];
            storageImageInfo.sampler = VK_NULL_HANDLE; //textureSamplers[0];

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &storageImageInfo;
            counter++;
        }
        
        //Step 4
        //std::cout<<"before vkUpdateDescriptorSets(). "<<std::endl;
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        //std::cout<<"after vkUpdateDescriptorSets(). "<<std::endl;
    }

    //std::cout<<"Done set descriptor(compute). "<<std::endl;
}

/************
 * 1 COMPUTE_UNIFORMBUFFER_GLOBAL
 ************/
std::vector<CWxjBuffer> CComputeDescriptorManager::m_globalUniformBuffers; 
std::vector<void*> CComputeDescriptorManager::m_globalUniformBuffersMapped;
//VkDeviceSize CComputeDescriptorManager::m_globalUniformBufferSize;
void CComputeDescriptorManager::addGlobalUniformBuffer(){
    computeUniformTypes |= COMPUTE_UNIFORMBUFFER_GLOBAL;

    m_globalUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_globalUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = m_globalUniformBuffers[i].init(sizeof(StructComputeGlobalUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_globalUniformBuffers[i].deviceMemory, 0, sizeof(StructComputeGlobalUniformBuffer), 0, &m_globalUniformBuffersMapped[i]);
    }
}
void CComputeDescriptorManager::uploadGlobalUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize){
    if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL) {
        if (data && dataSize > 0) {
            memcpy(m_globalUniformBuffersMapped[currentFrame], data, dataSize);
        }
    }
}
 

/************
 * 2 COMPUTE_STORAGEBUFFER_WINDOWSWAP
 ************/
std::vector<CWxjBuffer> CComputeDescriptorManager::storageBuffers_windowswap;
std::vector<void*> CComputeDescriptorManager::storageBuffersMapped_windowswap;
//VkDeviceSize CComputeDescriptorManager::m_storageBufferSize_windowswap;
void CComputeDescriptorManager::addStorageBuffer_windowswap(){
    computeUniformTypes |= COMPUTE_STORAGEBUFFER_WINDOWSWAP;
    //std::cout<<"addStorageBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    storageBuffers_windowswap.resize(MAX_FRAMES_IN_FLIGHT);
    storageBuffersMapped_windowswap.resize(MAX_FRAMES_IN_FLIGHT);

    //m_storageBufferSize_windowswap = storageBufferSize;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &shaderStorageBuffers_compute[i]);// Create a staging buffer used to upload data to the gpu
        //FillDataBufferHelper(shaderStorageBuffers_compute[i], (void *)(particles.data()));// Copy initial particle data to all storage buffers
        //shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        storageBuffers_windowswap[i].init(sizeof(StructStorageBuffer_WindowSwap), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_windowswap[i].deviceMemory, 0, sizeof(StructStorageBuffer_WindowSwap), 0, &storageBuffersMapped_windowswap[i]);
    }
}
void CComputeDescriptorManager::uploadStorageBuffer_windowswap(uint32_t currentFrame, const void* data, size_t size) {
    if (data && size > 0) {
        //std::cout<<"updateStorageBuffer: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
        memcpy(storageBuffersMapped_windowswap[currentFrame], data, size);
    }
}
void CComputeDescriptorManager::downloadStorageBuffer_windowswap(uint32_t currentFrame, void* data, size_t size) {
    if (data && size > 0) {
        memcpy(data, storageBuffersMapped_windowswap[currentFrame], size);
    }
}

/************
 * 7 COMPUTE_STORAGEBUFFER_CUSTOMSWAP
 ************/
std::vector<CWxjBuffer> CComputeDescriptorManager::storageBuffers_customswap;
std::vector<void*> CComputeDescriptorManager::storageBuffersMapped_customswap;
VkDeviceSize CComputeDescriptorManager::m_storageBufferSize_customswap;
void CComputeDescriptorManager::addStorageBuffer_customswap(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage){
    computeUniformTypes |= COMPUTE_STORAGEBUFFER_CUSTOMSWAP;
    //std::cout<<"addStorageBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    storageBuffers_customswap.resize(MAX_FRAMES_IN_FLIGHT);
    storageBuffersMapped_customswap.resize(MAX_FRAMES_IN_FLIGHT);

    m_storageBufferSize_customswap = storageBufferSize;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &shaderStorageBuffers_compute[i]);// Create a staging buffer used to upload data to the gpu
        //FillDataBufferHelper(shaderStorageBuffers_compute[i], (void *)(particles.data()));// Copy initial particle data to all storage buffers
        //shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        storageBuffers_customswap[i].init(storageBufferSize, usage, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_customswap[i].deviceMemory, 0, storageBufferSize, 0, &storageBuffersMapped_customswap[i]);
    }
}
void CComputeDescriptorManager::uploadStorageBuffer_customswap(uint32_t currentFrame, const void* data, size_t size) {
    if (data && size > 0) {
        //std::cout<<"updateStorageBuffer: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
        memcpy(storageBuffersMapped_customswap[currentFrame], data, size);
    }
}
void CComputeDescriptorManager::downloadStorageBuffer_customswap(uint32_t currentFrame, void* data, size_t size) {
    if (data && size > 0) {
        memcpy(data, storageBuffersMapped_customswap[currentFrame], size);
    }
}

/************
 * 3 COMPUTE_UNIFORMBUFFER_CUSTOM
 ************/
std::vector<CWxjBuffer> CComputeDescriptorManager::customUniformBuffers;
std::vector<void*> CComputeDescriptorManager::customUniformBuffersMapped;
VkDeviceSize CComputeDescriptorManager::m_customUniformBufferSize;
void CComputeDescriptorManager::addCustomUniformBuffer(VkDeviceSize customUniformBufferSize){
    computeUniformTypes |= COMPUTE_UNIFORMBUFFER_CUSTOM;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	customUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	customUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_customUniformBufferSize = customUniformBufferSize;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = customUniformBuffers[i].init(m_customUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), customUniformBuffers[i].deviceMemory, 0, m_customUniformBufferSize, 0, &customUniformBuffersMapped[i]);
	}    
}
void CComputeDescriptorManager::uploadCustomUniformBuffer(uint32_t currentFrame, const void* data, size_t dataSize) {
    if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM) {
        if (data && dataSize > 0) {
            memcpy(customUniformBuffersMapped[currentFrame], data, dataSize);
        }
    }
}

/************
 * 3 COMPUTE_STORAGEIMAGE_TEXTURE
 * 4 COMPUTE_STORAGEIMAGE_SWAPCHAIN
 ************/
void CComputeDescriptorManager::addStorageImage(VkBufferUsageFlags usage){
    computeUniformTypes |= usage;
}

/************
 * Helper Functions
 ************/
int CComputeDescriptorManager::getPoolSize(){
    //std::cout<<"getPoolSize::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;
	int descriptorPoolSize = 0;
    descriptorPoolSize += computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL ? 1:0;
    descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP ? 2:0; //add 2 because use storage for input/output,count as 2 unique uniforms
	descriptorPoolSize += computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM ? 1:0;
    descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_CUSTOMSWAP ? 2:0; 
    descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE ? 1:0;
    descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEIMAGE_SWAPCHAIN ? 1:0;
	return descriptorPoolSize;
}
int CComputeDescriptorManager::getLayoutSize(){
	return getPoolSize();
}

int CComputeDescriptorManager::getSetSize(){
    return getLayoutSize();
}

void CComputeDescriptorManager::DestroyAndFree(){
    for (size_t i = 0; i < m_globalUniformBuffers.size(); i++) 
        m_globalUniformBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    for (size_t i = 0; i < storageBuffers_windowswap.size(); i++) {
        storageBuffers_windowswap[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    for (size_t i = 0; i < storageBuffers_customswap.size(); i++) {
        storageBuffers_customswap[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    for (size_t i = 0; i < customUniformBuffers.size(); i++) {
        customUniformBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }

    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), computeDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
}

} //namespace