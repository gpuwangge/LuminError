#include "raytracingDescriptor.h"
#include <iostream>
#include "IGameEngine.h"
#include "context.h"

namespace LERenderer{

//Declare static variables here:
/************
* Pool
************/
int CRaytracingDescriptorManager::raytracingUniformTypes;
VkDescriptorPool CRaytracingDescriptorManager::raytracingDescriptorPool;
std::vector<VkDescriptorPoolSize> CRaytracingDescriptorManager::raytracingDescriptorPoolSizes;
void CRaytracingDescriptorManager::createDescriptorPool(){
    //Descriptor Step 1/3

	raytracingDescriptorPoolSizes.resize(getPoolSize());
	int counter = 0;
    //std::cout<<"createDescriptorPool::textureSamplers.size() = " << textureSamplers.size()<<std::endl;
    if(bVerbose) std::cout<<"Raytracing Pool size = " << getPoolSize()<<std::endl;


    // if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){
    //     //std::cout<<": Storage Buffer(2)";
    //     computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//     computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    //     counter++;
    //     //}
    //     //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
    //     computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	//     computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    //     counter++;
    // }


    // if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
    //     //std::cout<<": Storage Image(for Texture)";
    //     computeDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	//     computeDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
    //     counter++;
    // }

    //Note: 
    //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER UBO，少量常量数据,只读
    //VK_DESCRIPTOR_TYPE_STORAGE_BUFFER SSBO，大量任意数据, 可读可写
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        //std::cout<<": Storage Image(for Swapchain Presentation)";
        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	    raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
        counter++;

        //TODO: add a new uniform type?
        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	    raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
        counter++;

        // raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; //storage buffer for vertex attributes
	    // raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        // counter++;

        // raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; //storage buffer for index
	    // raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        // counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; //geometryInfo:必须用storage buffer, 因为使用了vkdeviceAddress
	    raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //uniform buffer for material
	    raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //global uniform buffer
        raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //custom uniform buffer
	  	raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	 	counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //rtlight
        raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //instance
        raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //config uniform buffer
        raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;

        raytracingDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;//accumulated image
	    raytracingDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); ///!!!
        counter++;
    }
    //std::cout<<std::endl;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(raytracingDescriptorPoolSizes.size());
    //std::cout<<"poolInfo.poolSizeCount = "<<poolInfo.poolSizeCount <<std::endl;
	poolInfo.pPoolSizes = raytracingDescriptorPoolSizes.data();
	poolInfo.maxSets = ((counter==0)?1:counter)*10*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!TODO: currently support 10 sets?

	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &raytracingDescriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
	//REPORT("vkCreateDescriptorPool");
}

/************
 * Layout
 ************/
std::vector<VkDescriptorSetLayoutBinding> CRaytracingDescriptorManager::raytracingBindings;
VkDescriptorSetLayout CRaytracingDescriptorManager::descriptorSetLayout;
void CRaytracingDescriptorManager::createDescriptorSetLayout(VkDescriptorSetLayoutBinding *customBinding){
    //Descriptor Step 2/3

    raytracingBindings.resize(getLayoutSize());
	int counter = 0;
    if(bVerbose) std::cout<<"Layout(Raytracing) size = " << getLayoutSize()<<std::endl;

    // if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){
    //     computeBindings[counter].binding = counter;
    //     computeBindings[counter].descriptorCount = 1;
    //     computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //     computeBindings[counter].pImmutableSamplers = nullptr;
    //     computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //     counter++;
    //     //}
    //     //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){
    //     computeBindings[counter].binding = counter;
    //     computeBindings[counter].descriptorCount = 1;
    //     computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //     computeBindings[counter].pImmutableSamplers = nullptr;
    //     computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //     counter++;
    // }

    // if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
    //     computeBindings[counter].binding = counter;
    //     computeBindings[counter].descriptorCount = 1;
    //     computeBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    //     computeBindings[counter].pImmutableSamplers = nullptr;
    //     computeBindings[counter].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //     counter++;
    // }
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        raytracingBindings[counter].binding = counter;
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;//VK_SHADER_STAGE_COMPUTE_BIT;
        counter++;

        raytracingBindings[counter].binding = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        counter++;

        //if(computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXATTRIBUTE){
        // raytracingBindings[counter].binding = counter;
        // raytracingBindings[counter].descriptorCount = 1;
        // raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        // raytracingBindings[counter].pImmutableSamplers = nullptr;
        // raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        // counter++;
        //}
        //if(computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX){
        // raytracingBindings[counter].binding = counter;
        // raytracingBindings[counter].descriptorCount = 1;
        // raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        // raytracingBindings[counter].pImmutableSamplers = nullptr;
        // raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        // counter++;
        //}

        raytracingBindings[counter].binding = counter;
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        counter++;

        raytracingBindings[counter].binding = counter; //uniform buffer for material
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        counter++;

        VkDescriptorSetLayoutBinding binding = StructComputeGlobalUniformBuffer::GetBinding(); //global
        raytracingBindings[counter].binding = counter;
		raytracingBindings[counter].descriptorCount = 1;//binding.descriptorCount;
		raytracingBindings[counter].descriptorType = binding.descriptorType;
		raytracingBindings[counter].pImmutableSamplers = binding.pImmutableSamplers;
		raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;//binding.stageFlags;
		counter++;

        raytracingBindings[counter].binding = counter; //custom
		raytracingBindings[counter].descriptorCount = customBinding->descriptorCount;
		raytracingBindings[counter].descriptorType = customBinding->descriptorType;
		raytracingBindings[counter].pImmutableSamplers = customBinding->pImmutableSamplers;
		raytracingBindings[counter].stageFlags = customBinding->stageFlags;
		counter++;

        raytracingBindings[counter].binding = counter; //uniform buffer for rtlight
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        counter++;

        raytracingBindings[counter].binding = counter; //storage buffer for instance
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        counter++;

        raytracingBindings[counter].binding = counter; //config uniform buffer
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        counter++;

        raytracingBindings[counter].binding = counter;
        raytracingBindings[counter].descriptorCount = 1;
        raytracingBindings[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        raytracingBindings[counter].pImmutableSamplers = nullptr;
        raytracingBindings[counter].stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        counter++;
    }

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(raytracingBindings.size());
	layoutInfo.pBindings = raytracingBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	//REPORT("vkCreateDescriptorSetLayout");
}

/************
 * Set
 ************/
void CRaytracingDescriptorManager::createDescriptorSets(VkImageView textureImageView, VkAccelerationStructureKHR tlas){
    //Descriptor Step 3/3
    //HERE_I_AM("wxjCreateDescriptorSets");

    int descriptorSize = getSetSize();
    if(bVerbose) std::cout<<"Set(Raytracing) size = "<<getSetSize()<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = raytracingDescriptorPool;
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

  

        // //std::cout<<"m_storageBufferSize = "<<m_storageBufferSize<<std::endl;
        // if(computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP){ //for storage buffer 1
        //     VkDescriptorBufferInfo storageBufferInfo_1{};
        //     storageBufferInfo_1.buffer = storageBuffers_windowswap[(i - 1) % MAX_FRAMES_IN_FLIGHT].buffer; //storage buffer of last frame in flight as compute shader input
        //     storageBufferInfo_1.offset = 0;
        //     storageBufferInfo_1.range = sizeof(StructStorageBuffer_WindowSwap);

        //     descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrites[counter].dstSet = descriptorSets[i];
        //     descriptorWrites[counter].dstBinding = counter;
        //     descriptorWrites[counter].dstArrayElement = 0;
        //     descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        //     descriptorWrites[counter].descriptorCount = 1;
        //     descriptorWrites[counter].pBufferInfo = &storageBufferInfo_1;
        //     counter++;
        //     //}
        //     VkDescriptorBufferInfo storageBufferInfo_2{};
        //     //if(uniformBufferUsageFlags & UNIFORM_BUFFER_STORAGE_2_BIT){ //for storage buffer 2
        //     storageBufferInfo_2.buffer = storageBuffers_windowswap[i].buffer; //storage buffer of the current frame in flight as compute shader output
        //     storageBufferInfo_2.offset = 0;
        //     storageBufferInfo_2.range = sizeof(StructStorageBuffer_WindowSwap);

        //     descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrites[counter].dstSet = descriptorSets[i];
        //     descriptorWrites[counter].dstBinding = counter;
        //     descriptorWrites[counter].dstArrayElement = 0;
        //     descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        //     descriptorWrites[counter].descriptorCount = 1;
        //     descriptorWrites[counter].pBufferInfo = &storageBufferInfo_2;
        //     counter++;
        // }


        // //std::cout<<"UNIFORM_IMAGE_STORAGE_TEXTURE_BIT"<<std::endl;
        // if(computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE){
        //     VkDescriptorImageInfo storageImageInfo{};
        //     storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        //     storageImageInfo.imageView = textureImageView;
        //     storageImageInfo.sampler = VK_NULL_HANDLE; //textureSamplers[0];

        //     descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrites[counter].dstSet = descriptorSets[i];
        //     descriptorWrites[counter].dstBinding = counter;
        //     descriptorWrites[counter].dstArrayElement = 0;
        //     descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        //     descriptorWrites[counter].descriptorCount = 1;
        //     descriptorWrites[counter].pImageInfo = &storageImageInfo;
        //     counter++;
        // }

        //std::cout<<"UNIFORM_IMAGE_STORAGE_SWAPCHAIN_BIT"<<std::endl;
        if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
            //binding 0: storage image
            VkDescriptorImageInfo storageImageInfo{};
            storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            //storageImageInfo.imageView = (*swapchainImageViews)[i];
            storageImageInfo.imageView = p_swapchain->intermediaColor[i].view;
            storageImageInfo.sampler = VK_NULL_HANDLE; //textureSamplers[0];

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &storageImageInfo;
            counter++;

            //binding 1: TLAS
            VkWriteDescriptorSetAccelerationStructureKHR asInfo{};
            asInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            asInfo.pNext = nullptr;
            asInfo.accelerationStructureCount = 1;
            asInfo.pAccelerationStructures = &tlas;

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].pNext = &asInfo;
            descriptorWrites[counter].dstSet = descriptorSets[i];//raytracingDescriptorSet;
            descriptorWrites[counter].dstBinding = 1;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
            descriptorWrites[counter].pImageInfo = nullptr;
            descriptorWrites[counter].pBufferInfo = nullptr;
            descriptorWrites[counter].pTexelBufferView = nullptr;
            counter++;

            //if(computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXATTRIBUTE){
            // VkDescriptorBufferInfo storageBufferInfo1{};
            // storageBufferInfo1.buffer = storageBuffers_triangleVertexAttribute[i].buffer;
            // storageBufferInfo1.offset = 0;
            // storageBufferInfo1.range = sizeof(StructStorageBuffer_TriangleVertexAttribute);

            // descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // descriptorWrites[counter].dstSet = descriptorSets[i];
            // descriptorWrites[counter].dstBinding = counter;
            // descriptorWrites[counter].dstArrayElement = 0;
            // descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            // descriptorWrites[counter].descriptorCount = 1;
            // descriptorWrites[counter].pBufferInfo = &storageBufferInfo1;
            // counter++;
            //}

            //if(computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX){
            // VkDescriptorBufferInfo storageBufferInfo2{};
            // storageBufferInfo2.buffer = storageBuffers_triangleVertexIndex[i].buffer;
            // storageBufferInfo2.offset = 0;
            // storageBufferInfo2.range = sizeof(StructStorageBuffer_TriangleVertexIndex);

            // descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // descriptorWrites[counter].dstSet = descriptorSets[i];
            // descriptorWrites[counter].dstBinding = counter;
            // descriptorWrites[counter].dstArrayElement = 0;
            // descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            // descriptorWrites[counter].descriptorCount = 1;
            // descriptorWrites[counter].pBufferInfo = &storageBufferInfo2;
            // counter++;
            //}

            VkDescriptorBufferInfo storageBufferInfo3{}; //geometry
            storageBufferInfo3.buffer = storageBuffers_geometryInfo[i].buffer;
            storageBufferInfo3.offset = 0;
            storageBufferInfo3.range = sizeof(StructStorageBuffer_GeometryInfo);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo3;
            counter++;

            VkDescriptorBufferInfo storageBufferInfo4{}; //uniform buffer for material
            storageBufferInfo4.buffer = uniformBuffers_material[i].buffer;
            storageBufferInfo4.offset = 0;
            storageBufferInfo4.range = sizeof(StructUniformBuffer_Material);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo4;
            counter++;

            VkDescriptorBufferInfo globalBufferInfo{}; //for global
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

            VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
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

            VkDescriptorBufferInfo storageBufferInfo5{}; //uniform buffer for rtlight
            storageBufferInfo5.buffer = m_uniformBuffers_rtLight[i].buffer;
            storageBufferInfo5.offset = 0;
            storageBufferInfo5.range = sizeof(StructUniformBuffer_RtLight);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo5;
            counter++;

            VkDescriptorBufferInfo storageBufferInfo6{}; //uniform buffer for instance
            storageBufferInfo6.buffer = m_uniformBuffers_instance[i].buffer;
            storageBufferInfo6.offset = 0;
            storageBufferInfo6.range = sizeof(StructUniformBuffer_Instance);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &storageBufferInfo6;
            counter++;

            VkDescriptorBufferInfo uniformBufferInfo{}; //config uniform buffer
            uniformBufferInfo.buffer = m_uniformBuffers_config[i].buffer;
            uniformBufferInfo.offset = 0;
            uniformBufferInfo.range = sizeof(StructConfigUniformBuffer);

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &uniformBufferInfo;
            counter++;

            VkDescriptorImageInfo storageImageInfo2{};
            storageImageInfo2.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            //storageImageInfo.imageView = (*swapchainImageViews)[i];
            storageImageInfo2.imageView = p_swapchain->intermediaColor2.view;
            storageImageInfo2.sampler = VK_NULL_HANDLE; //textureSamplers[0];

            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &storageImageInfo2;
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
 * GLOBAL
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::m_globalUniformBuffers; 
std::vector<void*> CRaytracingDescriptorManager::m_globalUniformBuffersMapped;
//VkDeviceSize CRaytracingDescriptorManager::m_globalUniformBufferSize;
void CRaytracingDescriptorManager::addGlobalUniformBuffer_raytracing(){
    //computeUniformTypes |= COMPUTE_UNIFORMBUFFER_GLOBAL;

    m_globalUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_globalUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = m_globalUniformBuffers[i].init(sizeof(StructRaytracingGlobalUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_globalUniformBuffers[i].deviceMemory, 0, sizeof(StructRaytracingGlobalUniformBuffer), 0, &m_globalUniformBuffersMapped[i]);
    }
}
void CRaytracingDescriptorManager::uploadGlobalUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize){
    //if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL) {
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        if (data && dataSize > 0) {
            memcpy(m_globalUniformBuffersMapped[currentFrame], data, dataSize);
        }
    }
}
 
/************
 * ? STORAGEBUFFER RTLIGHT
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::m_uniformBuffers_rtLight; 
std::vector<void*> CRaytracingDescriptorManager::m_uniformBuffersMapped_rtLight;
void CRaytracingDescriptorManager::addUniformBuffer_rtLight(){
    //computeUniformTypes |= COMPUTE_UNIFORMBUFFER_GLOBAL;

    m_uniformBuffers_rtLight.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped_rtLight.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = m_uniformBuffers_rtLight[i].init(sizeof(StructUniformBuffer_RtLight), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_uniformBuffers_rtLight[i].deviceMemory, 0, sizeof(StructUniformBuffer_RtLight), 0, &m_uniformBuffersMapped_rtLight[i]);
    }
}
void CRaytracingDescriptorManager::uploadUniformBuffer_rtLight(uint32_t currentFrame, const void* data, size_t dataSize){
    //if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL) {
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        if (data && dataSize > 0) {
            memcpy(m_uniformBuffersMapped_rtLight[currentFrame], data, dataSize);
        }
    }
}

/************
 * 7 UNIFORMBUFFER INSTANCE
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::m_uniformBuffers_instance; 
std::vector<void*> CRaytracingDescriptorManager::m_uniformBuffersMapped_instance;
void CRaytracingDescriptorManager::addUniformBuffer_instance(){
    //computeUniformTypes |= COMPUTE_UNIFORMBUFFER_GLOBAL;

    m_uniformBuffers_instance.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped_instance.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = m_uniformBuffers_instance[i].init(sizeof(StructUniformBuffer_Instance), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_uniformBuffers_instance[i].deviceMemory, 0, sizeof(StructUniformBuffer_Instance), 0, &m_uniformBuffersMapped_instance[i]);
    }
}
void CRaytracingDescriptorManager::uploadUniformBuffer_instance(uint32_t currentFrame, const void* data, size_t dataSize){
    //if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL) {
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        std::cout<<"uploadUniformBuffer_instance: dataSize = "<<dataSize<<std::endl;
        if (data && dataSize > 0) {
            memcpy(m_uniformBuffersMapped_instance[currentFrame], data, dataSize);
        }
    }
}

/************
 * 8 UNIFORMBUFFER CONFIG
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::m_uniformBuffers_config; 
std::vector<void*> CRaytracingDescriptorManager::m_uniformBuffersMapped_config;
void CRaytracingDescriptorManager::addUniformBuffer_config(){
    //computeUniformTypes |= COMPUTE_UNIFORMBUFFER_GLOBAL;

    m_uniformBuffers_config.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped_config.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = m_uniformBuffers_config[i].init(sizeof(StructConfigUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_uniformBuffers_config[i].deviceMemory, 0, sizeof(StructConfigUniformBuffer), 0, &m_uniformBuffersMapped_config[i]);
    }
}
void CRaytracingDescriptorManager::uploadUniformBuffer_config(uint32_t currentFrame, const void* data, size_t dataSize){
    //if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL) {
    if(raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN){
        if (data && dataSize > 0) {
            memcpy(m_uniformBuffersMapped_config[currentFrame], data, dataSize);
        }
    }
}

/************
 * 2 COMPUTE_STORAGEBUFFER_WINDOWSWAP
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_windowswap;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_windowswap;
// //VkDeviceSize CRaytracingDescriptorManager::m_storageBufferSize_windowswap;
// void CRaytracingDescriptorManager::addStorageBuffer_windowswap(){
//     computeUniformTypes |= COMPUTE_STORAGEBUFFER_WINDOWSWAP;
//     //std::cout<<"addStorageBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

//     storageBuffers_windowswap.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_windowswap.resize(MAX_FRAMES_IN_FLIGHT);

//     //m_storageBufferSize_windowswap = storageBufferSize;

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &shaderStorageBuffers_compute[i]);// Create a staging buffer used to upload data to the gpu
//         //FillDataBufferHelper(shaderStorageBuffers_compute[i], (void *)(particles.data()));// Copy initial particle data to all storage buffers
//         //shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
//         storageBuffers_windowswap[i].init(sizeof(StructStorageBuffer_WindowSwap), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_windowswap[i].deviceMemory, 0, sizeof(StructStorageBuffer_WindowSwap), 0, &storageBuffersMapped_windowswap[i]);
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_windowswap(uint32_t currentFrame, const void* data, size_t size) {
//     if (data && size > 0) {
//         //std::cout<<"updateStorageBuffer: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_windowswap[currentFrame], data, size);
//     }
// }
// void CRaytracingDescriptorManager::downloadStorageBuffer_windowswap(uint32_t currentFrame, void* data, size_t size) {
//     if (data && size > 0) {
//         memcpy(data, storageBuffersMapped_windowswap[currentFrame], size);
//     }
// }

/************
 * 3 UNIFORMBUFFER_MATERIAL
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::uniformBuffers_material;
std::vector<void*> CRaytracingDescriptorManager::uniformBuffersMapped_material;
void CRaytracingDescriptorManager::addUniformBuffer_material(){
    //std::cout<<"Raytracing Pipeline Descriptor: addUniformBuffer_material()"<<std::endl;
    //computeUniformTypes |= COMPUTE_STORAGEBUFFER_MATERIAL;

    uniformBuffers_material.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped_material.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers_material[i].init(sizeof(StructUniformBuffer_Material), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), uniformBuffers_material[i].deviceMemory, 0, sizeof(StructUniformBuffer_Material), 0, &uniformBuffersMapped_material[i]);
 
    }
}
void CRaytracingDescriptorManager::uploadUniformBuffer_material(uint32_t currentFrame, const void* data, size_t size){
    //std::cout<<"Raytracing Pipeline Descriptor: uploadUniformBuffer_material(), size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
    if (data && size > 0) {
        //std::cout<<"uploadUniformBuffer_material: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
        memcpy(uniformBuffersMapped_material[currentFrame], data, size);
    }
}

/************
 * 4 COMPUTE_STORAGEBUFFER_TRIANGLEVERTEX
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_triangleVertexAttribute;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_triangleVertexAttribute;
// void CRaytracingDescriptorManager::addStorageBuffer_triangleVertexAttribute(){
//    //computeUniformTypes |= COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXATTRIBUTE;

//     storageBuffers_triangleVertexAttribute.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_triangleVertexAttribute.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         storageBuffers_triangleVertexAttribute[i].init(sizeof(StructStorageBuffer_TriangleVertexAttribute), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_triangleVertexAttribute[i].deviceMemory, 0, sizeof(StructStorageBuffer_TriangleVertexAttribute), 0, &storageBuffersMapped_triangleVertexAttribute[i]);
 
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_triangleVertexAttribute(uint32_t currentFrame, const void* data, size_t size){
//     if (data && size > 0) {
//         //std::cout<<"uploadStorageBuffer_material: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_triangleVertexAttribute[currentFrame], data, size);
//     }
// }

/************
 * 5 COMPUTE_STORAGEBUFFER_TRIANGLEINDEX
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_triangleVertexIndex;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_triangleVertexIndex;
// void CRaytracingDescriptorManager::addStorageBuffer_triangleVertexIndex(){
//     //computeUniformTypes |= COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX;

//     storageBuffers_triangleVertexIndex.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_triangleVertexIndex.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         storageBuffers_triangleVertexIndex[i].init(sizeof(StructStorageBuffer_TriangleVertexIndex), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_triangleVertexIndex[i].deviceMemory, 0, sizeof(StructStorageBuffer_TriangleVertexIndex), 0, &storageBuffersMapped_triangleVertexIndex[i]);
 
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_triangleVertexIndex(uint32_t currentFrame, const void* data, size_t size){
//     if (data && size > 0) {
//         //std::cout<<"uploadStorageBuffer_material: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_triangleVertexIndex[currentFrame], data, size);
//     }
// }

/************
 * 5.2 geometry info
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_geometryInfo;
std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_geometryInfo;
void CRaytracingDescriptorManager::addStorageBuffer_geometryInfo(){
    //computeUniformTypes |= COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX;
    //std::cout<<"addStorageBuffer_geometryInfo()"<<std::endl;
    storageBuffers_geometryInfo.resize(MAX_FRAMES_IN_FLIGHT);
    storageBuffersMapped_geometryInfo.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        storageBuffers_geometryInfo[i].init(sizeof(StructStorageBuffer_GeometryInfo), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_geometryInfo[i].deviceMemory, 0, sizeof(StructStorageBuffer_GeometryInfo), 0, &storageBuffersMapped_geometryInfo[i]);
 
    }
}
void CRaytracingDescriptorManager::uploadStorageBuffer_geometryInfo(uint32_t currentFrame, const void* data, size_t size){
    if (data && size > 0) {
        //std::cout<<"uploadStorageBuffer_geometryInfo: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
        memcpy(storageBuffersMapped_geometryInfo[currentFrame], data, size);
    }
}

/************
 * 5.5 COMPUTE_STORAGEBUFFER_TRIANGLEREORDERINDEX
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_triangleReorderIndex;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_triangleReorderIndex;
// void CRaytracingDescriptorManager::addStorageBuffer_triangleReorderIndex(){
//     computeUniformTypes |= COMPUTE_STORAGEBUFFER_TRIANGLEREORDERINDEX;

//     storageBuffers_triangleReorderIndex.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_triangleReorderIndex.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         storageBuffers_triangleReorderIndex[i].init(sizeof(StructStorageBuffer_TriangleReorderIndex), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_triangleReorderIndex[i].deviceMemory, 0, sizeof(StructStorageBuffer_TriangleReorderIndex), 0, &storageBuffersMapped_triangleReorderIndex[i]);
 
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_triangleReorderIndex(uint32_t currentFrame, const void* data, size_t size){
//     if (data && size > 0) {
//         //std::cout<<"uploadStorageBuffer_material: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_triangleReorderIndex[currentFrame], data, size);
//     }
// }

/************
 * 6 COMPUTE_STORAGEBUFFER_BVHNODE
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_bvhNode;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_bvhNode;
// void CRaytracingDescriptorManager::addStorageBuffer_bvhNode(){
//     computeUniformTypes |= COMPUTE_STORAGEBUFFER_BVHNODE;

//     storageBuffers_bvhNode.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_bvhNode.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         storageBuffers_bvhNode[i].init(sizeof(StructStorageBuffer_BVHNode), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_bvhNode[i].deviceMemory, 0, sizeof(StructStorageBuffer_BVHNode), 0, &storageBuffersMapped_bvhNode[i]);
 
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_bvhNode(uint32_t currentFrame, const void* data, size_t size){
//     if (data && size > 0) {
//         memcpy(storageBuffersMapped_bvhNode[currentFrame], data, size);
//     }
// }


/************
 * 7 COMPUTE_STORAGEBUFFER_SPHERE
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_sphere;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_sphere;
// void CRaytracingDescriptorManager::addStorageBuffer_sphere(){
//     computeUniformTypes |= COMPUTE_STORAGEBUFFER_SPHERE;

//     storageBuffers_sphere.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_sphere.resize(MAX_FRAMES_IN_FLIGHT);

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         storageBuffers_sphere[i].init(sizeof(StructStorageBuffer_Sphere), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_sphere[i].deviceMemory, 0, sizeof(StructStorageBuffer_Sphere), 0, &storageBuffersMapped_sphere[i]);
 
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_sphere(uint32_t currentFrame, const void* data, size_t size){
//     if (data && size > 0) {
//         //std::cout<<"uploadStorageBuffer_material: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_sphere[currentFrame], data, size);
//     }
// }

/************
 * 8 COMPUTE_UNIFORMBUFFER_CUSTOM
 ************/
std::vector<CWxjBuffer> CRaytracingDescriptorManager::customUniformBuffers;
std::vector<void*> CRaytracingDescriptorManager::customUniformBuffersMapped;
VkDeviceSize CRaytracingDescriptorManager::m_customUniformBufferSize;
void CRaytracingDescriptorManager::addCustomUniformBuffer_raytracing(VkDeviceSize customUniformBufferSize){
    //computeUniformTypes |= COMPUTE_UNIFORMBUFFER_CUSTOM;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	customUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	customUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_customUniformBufferSize = customUniformBufferSize;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = customUniformBuffers[i].init(m_customUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), customUniformBuffers[i].deviceMemory, 0, m_customUniformBufferSize, 0, &customUniformBuffersMapped[i]);
	}    
}
void CRaytracingDescriptorManager::uploadCustomUniformBuffer_raytracing(uint32_t currentFrame, const void* data, size_t dataSize) {
    //if (computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM) {
        if (data && dataSize > 0) {
            memcpy(customUniformBuffersMapped[currentFrame], data, dataSize);
        }
    //}
}

/************
 * 9 COMPUTE_STORAGEBUFFER_CUSTOMSWAP
 ************/
// std::vector<CWxjBuffer> CRaytracingDescriptorManager::storageBuffers_customswap;
// std::vector<void*> CRaytracingDescriptorManager::storageBuffersMapped_customswap;
// VkDeviceSize CRaytracingDescriptorManager::m_storageBufferSize_customswap;
// void CRaytracingDescriptorManager::addStorageBuffer_customswap(VkDeviceSize storageBufferSize, VkBufferUsageFlags usage){
//     computeUniformTypes |= COMPUTE_STORAGEBUFFER_CUSTOMSWAP;
//     //std::cout<<"addStorageBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

//     storageBuffers_customswap.resize(MAX_FRAMES_IN_FLIGHT);
//     storageBuffersMapped_customswap.resize(MAX_FRAMES_IN_FLIGHT);

//     m_storageBufferSize_customswap = storageBufferSize;

//     for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//         //VkResult result = InitDataBufferHelper(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, &shaderStorageBuffers_compute[i]);// Create a staging buffer used to upload data to the gpu
//         //FillDataBufferHelper(shaderStorageBuffers_compute[i], (void *)(particles.data()));// Copy initial particle data to all storage buffers
//         //shaderStorageBuffers_compute[i].init(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
//         storageBuffers_customswap[i].init(storageBufferSize, usage, CContext::GetHandle().GetLogicalDevice(), CContext::GetHandle().GetPhysicalDevice());
//         vkMapMemory(CContext::GetHandle().GetLogicalDevice(), storageBuffers_customswap[i].deviceMemory, 0, storageBufferSize, 0, &storageBuffersMapped_customswap[i]);
//     }
// }
// void CRaytracingDescriptorManager::uploadStorageBuffer_customswap(uint32_t currentFrame, const void* data, size_t size) {
//     if (data && size > 0) {
//         //std::cout<<"updateStorageBuffer: size = "<<size<<", currentFrame = "<<currentFrame<<std::endl;
//         memcpy(storageBuffersMapped_customswap[currentFrame], data, size);
//     }
// }
// void CRaytracingDescriptorManager::downloadStorageBuffer_customswap(uint32_t currentFrame, void* data, size_t size) {
//     if (data && size > 0) {
//         memcpy(data, storageBuffersMapped_customswap[currentFrame], size);
//     }
// }


/************
 * 10 COMPUTE_STORAGEIMAGE_TEXTURE
 * 11 COMPUTE_STORAGEIMAGE_SWAPCHAIN
 ************/
void CRaytracingDescriptorManager::addStorageImage(VkBufferUsageFlags usage){
    raytracingUniformTypes |= usage;
}

/************
 * Helper Functions
 ************/
int CRaytracingDescriptorManager::getPoolSize(){
    //std::cout<<"getPoolSize::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;
	int descriptorPoolSize = 0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_UNIFORMBUFFER_GLOBAL ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_WINDOWSWAP ? 2:0; //add 2 because use storage for input/output,count as 2 unique uniforms
	// descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_MATERIAL ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXATTRIBUTE ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEVERTEXINDEX ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_TRIANGLEREORDERINDEX ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_BVHNODE ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_SPHERE ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_UNIFORMBUFFER_CUSTOM ? 1:0;
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEBUFFER_CUSTOMSWAP ? 2:0; 
    // descriptorPoolSize += computeUniformTypes & COMPUTE_STORAGEIMAGE_TEXTURE ? 1:0;
    descriptorPoolSize += raytracingUniformTypes & RAYTRACING_STORAGEIMAGE_SWAPCHAIN ? 10:0; 
    //TODO: currently combine image and tlas, geometry Info, material, global, custom, rtlight, instance, config, accumulated image
	return descriptorPoolSize;
}
int CRaytracingDescriptorManager::getLayoutSize(){
	return getPoolSize();
}

int CRaytracingDescriptorManager::getSetSize(){
    return getLayoutSize();
}

void CRaytracingDescriptorManager::DestroyAndFree(){
    for (size_t i = 0; i < m_globalUniformBuffers.size(); i++) 
        m_globalUniformBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // for (size_t i = 0; i < storageBuffers_windowswap.size(); i++) {
    //     storageBuffers_windowswap[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    for (size_t i = 0; i < uniformBuffers_material.size(); i++) {
        uniformBuffers_material[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    // for (size_t i = 0; i < storageBuffers_triangleVertexAttribute.size(); i++) {
    //     storageBuffers_triangleVertexAttribute[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    // for (size_t i = 0; i < storageBuffers_triangleVertexIndex.size(); i++) {
    //     storageBuffers_triangleVertexIndex[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    for (size_t i = 0; i < storageBuffers_geometryInfo.size(); i++) {
        storageBuffers_geometryInfo[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    for (size_t i = 0; i < m_uniformBuffers_rtLight.size(); i++) {
        m_uniformBuffers_rtLight[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    for (size_t i = 0; i < m_uniformBuffers_instance.size(); i++) {
        m_uniformBuffers_instance[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    // for (size_t i = 0; i < storageBuffers_triangleReorderIndex.size(); i++) {
    //     storageBuffers_triangleReorderIndex[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    // for (size_t i = 0; i < storageBuffers_bvhNode.size(); i++) {
    //     storageBuffers_bvhNode[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    // for (size_t i = 0; i < storageBuffers_sphere.size(); i++) {
    //     storageBuffers_sphere[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    // for (size_t i = 0; i < storageBuffers_customswap.size(); i++) {
    //     storageBuffers_customswap[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    // }
    for (size_t i = 0; i < customUniformBuffers.size(); i++) {
         customUniformBuffers[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }
    for (size_t i = 0; i < m_uniformBuffers_config.size(); i++) {
        m_uniformBuffers_config[i].DestroyAndFree(CContext::GetHandle().GetLogicalDevice());
    }

    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), raytracingDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout, nullptr);
}

} //namespace