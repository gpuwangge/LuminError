#include "../include/graphicsDescriptor.h"

//Declare static variables here:
/************
* Pool
************/
int CGraphicsDescriptorManager::graphicsUniformTypes;
VkDescriptorPool CGraphicsDescriptorManager::graphicsDescriptorPool;
std::vector<VkDescriptorPoolSize> CGraphicsDescriptorManager::graphicsDescriptorPoolSizes;
unsigned int CGraphicsDescriptorManager::textureImageSamplerSize;
void CGraphicsDescriptorManager::createDescriptorPool(unsigned int object_textbox_count){
    //Descriptor Step 1/3
    //objectCount = object_count;

	graphicsDescriptorPoolSizes.resize(getPoolSize());
	int counter = 0;
    //std::cout<<"createDescriptorPool::textureSamplers.size() = " << textureSamplers.size()<<std::endl;
    std::cout<<"Graphics Pool size = " << getPoolSize()<<std::endl;

    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        //std::cout<<": MVP";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP){
        //std::cout<<": Text MVP";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        counter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
        //std::cout<<": Custom Buffer";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
        //std::cout<<": Lighting Buffer";
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 	graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		counter++;
	}
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE){
        //std::cout<<": Sampler("<<samplerCount<<")";
        for(int i = 0; i < textureImageSamplerSize; i++){
            graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	 	    graphicsDescriptorPoolSizes[counter].descriptorCount = object_textbox_count*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//each object has a sampler
		    counter++;
        }
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE){
        //std::cout<<"createDescriptorPool():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//one depth sampler
        counter++; 
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE){
        //std::cout<<"createDescriptorPool():GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE"<<std::endl;
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//one depth sampler
        counter++; 
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS){
        //std::cout<<"createDescriptorPool():GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS"<<std::endl;
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT*LIGHT_MAX);
        counter++; 
    }
    /*
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2){
        //std::cout<<"createDescriptorPool():GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2"<<std::endl;
        graphicsDescriptorPoolSizes[counter].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        graphicsDescriptorPoolSizes[counter].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//one depth sampler
        counter++; 
    }*/
    

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(graphicsDescriptorPoolSizes.size());
    //std::cout<<"poolInfo.poolSizeCount = "<<poolInfo.poolSizeCount <<std::endl;
	poolInfo.pPoolSizes = graphicsDescriptorPoolSizes.data();
	poolInfo.maxSets = ((counter==0)?1:counter)*20*static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!TODO: currently support 20 sets?

	VkResult result = vkCreateDescriptorPool(CContext::GetHandle().GetLogicalDevice(), &poolInfo, nullptr, &graphicsDescriptorPool);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor pool!");
}

/************
* Layout
************/
std::vector<VkDescriptorSetLayoutBinding> CGraphicsDescriptorManager::graphicsBindings;
VkDescriptorSetLayout CGraphicsDescriptorManager::descriptorSetLayout_general;
VkDescriptorSetLayout CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler;
void CGraphicsDescriptorManager::createDescriptorSetLayout_General(VkDescriptorSetLayoutBinding *customBinding){
    //Descriptor Step 2/3
    graphicsBindings.resize(getLayoutSize_General());
	int bindingCounter = 0;
    std::cout<<"Layout(Graphics General) size = " << graphicsBindings.size()<<std::endl;

    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){
        VkDescriptorSetLayoutBinding binding = MVPUniformBufferObject::GetBinding();
        //std::cout<<"DEBUG: MVP Layout binding="<<counter<<std::endl;
        graphicsBindings[bindingCounter].binding = bindingCounter;
        graphicsBindings[bindingCounter].descriptorCount = binding.descriptorCount;
        graphicsBindings[bindingCounter].descriptorType = binding.descriptorType;
        graphicsBindings[bindingCounter].pImmutableSamplers = binding.pImmutableSamplers;
        graphicsBindings[bindingCounter].stageFlags = binding.stageFlags;
        bindingCounter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP){
        VkDescriptorSetLayoutBinding binding = MVPUniformBufferObject::GetBinding();
        //std::cout<<"DEBUG: MVP Layout binding="<<counter<<std::endl;
        graphicsBindings[bindingCounter].binding = bindingCounter;
        graphicsBindings[bindingCounter].descriptorCount = binding.descriptorCount;
        graphicsBindings[bindingCounter].descriptorType = binding.descriptorType;
        graphicsBindings[bindingCounter].pImmutableSamplers = binding.pImmutableSamplers;
        graphicsBindings[bindingCounter].stageFlags = binding.stageFlags;
        bindingCounter++;
    }
	if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = customBinding->descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = customBinding->descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = customBinding->pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = customBinding->stageFlags;
		bindingCounter++;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = m_lightingUBO.GetBinding().descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = m_lightingUBO.GetBinding().descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = m_lightingUBO.GetBinding().pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = m_lightingUBO.GetBinding().stageFlags;
		bindingCounter++;
        //std::cout<<"created lighting bindings "<<std::endl;
	}
    if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = binding.descriptorCount;
		graphicsBindings[bindingCounter].descriptorType = binding.descriptorType;
		graphicsBindings[bindingCounter].pImmutableSamplers = binding.pImmutableSamplers;
		graphicsBindings[bindingCounter].stageFlags = binding.stageFlags;
		bindingCounter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE){
        //std::cout<<"createDescriptorSetLayout_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = 1;
		graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
		graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindingCounter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE){
        //std::cout<<"createDescriptorSetLayout_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = 1;
		graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
		graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindingCounter++;
    }
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS){
        //std::cout<<"createDescriptorSetLayout_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = LIGHT_MAX;
		graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
		graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindingCounter++;
    }
    /*
    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2){
        //std::cout<<"createDescriptorSetLayout_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;
        VkDescriptorSetLayoutBinding binding = VPUniformBufferObject::GetBinding();
        graphicsBindings[bindingCounter].binding = bindingCounter;
		graphicsBindings[bindingCounter].descriptorCount = 1;
		graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
		graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindingCounter++;
    }*/

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(graphicsBindings.size());
	layoutInfo.pBindings = graphicsBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout_general);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
	//REPORT("vkCreateDescriptorSetLayout");
    //std::cout<<"created bindings DONE"<<std::endl;
}
void CGraphicsDescriptorManager::createDescriptorSetLayout_TextureImageSampler(){
    //Descriptor Step 2/3
    graphicsBindings.resize(textureImageSamplers.size());//sampleCount?
    std::cout<<"Layout(Graphcis Sampler) size = "<<textureImageSamplers.size()<<std::endl;
	int bindingCounter = 0;

    if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE){
        for(int i = 0; i < textureImageSamplers.size(); i++){
            graphicsBindings[bindingCounter].binding = bindingCounter;
            graphicsBindings[bindingCounter].descriptorCount = 1;
            graphicsBindings[bindingCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            graphicsBindings[bindingCounter].pImmutableSamplers = nullptr;
            graphicsBindings[bindingCounter].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindingCounter++;
        }
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(graphicsBindings.size());
	layoutInfo.pBindings = graphicsBindings.data();

	VkResult result = vkCreateDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), &layoutInfo, nullptr, OUT &descriptorSetLayout_textureImageSampler);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create descriptor set layout!");
}

/************
* Set
************/
void CGraphicsDescriptorManager::createDescriptorSets_General(VkImageView depthImageView, std::vector<CWxjImageBuffer> &lightDepthBuffers){
    //Descriptor Step 3/3
    //HERE_I_AM("wxjCreateDescriptorSets");

    int descriptorSize = getSetSize_General();
    //std::cout<<"Set(Graphics, Non-sampler) size = "<<getSetSize()<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_general);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = graphicsDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_general.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets_general.data());
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {///!!!
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        //VkDescriptorBufferInfo storageBufferInfoLastFrame{}; //for compute shader
        //VkDescriptorBufferInfo storageBufferInfoCurrentFrame{}; //for compute shader

        descriptorWrites.resize(descriptorSize);
        int counter = 0;

        VkDescriptorBufferInfo mvpBufferInfo{}; //for mvp
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){ //TODO: Getbinding
            mvpBufferInfo.buffer = mvpUniformBuffers[i].buffer;
            mvpBufferInfo.offset = 0;
            //sizeof(MVPUniformBufferObject) is 512, including 2 mvp matrices. We only use one at a time.
            //spec requires alighment to be multiple of 256 (1080 TI). Maybe change this later?
            mvpBufferInfo.range = 512;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &mvpBufferInfo;
            counter++;
        }
        VkDescriptorBufferInfo textMVPBufferInfo{}; //for text mvp
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP){ //TODO: Getbinding
            textMVPBufferInfo.buffer = textMVPUniformBuffers[i].buffer;
            textMVPBufferInfo.offset = 0;
            //sizeof(MVPUniformBufferObject) is 512, including 2 mvp matrices. We only use one at a time.
            //spec requires alighment to be multiple of 256 (1080 TI). Maybe change this later?
            textMVPBufferInfo.range = 512;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &textMVPBufferInfo;
            counter++;
        }

        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM){
            customBufferInfo.buffer = customUniformBuffers[i].buffer;
            customBufferInfo.offset = 0;
            customBufferInfo.range = m_customUniformBufferSize;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &customBufferInfo;
            counter++;
        }

        VkDescriptorBufferInfo lightingBufferInfo{}; //for lighting uniform
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING){
            lightingBufferInfo.buffer = m_lightingUniformBuffers[i].buffer;
            lightingBufferInfo.offset = 0;
            lightingBufferInfo.range = m_lightingUniformBufferSize;
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &lightingBufferInfo;
            counter++;
        }
        
        VkDescriptorBufferInfo vpBufferInfo{}; //for vp
        if(graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
            vpBufferInfo.buffer = vpUniformBuffers[i].buffer;
            vpBufferInfo.offset = 0;
            vpBufferInfo.range = sizeof(VPUniformBufferObject);
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pBufferInfo = &vpBufferInfo;
            counter++;
        }
        VkDescriptorImageInfo depthImageInfo{}; //for depth sampler
        if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE){
            //std::cout<<"createDescriptorSets_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;

            depthImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_GENERAL; 
            depthImageInfo.imageView = depthImageView; //depth image from swapchain
            depthImageInfo.sampler = depthImageSampler; 

            // if(j < m_texture_ids.size()){
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            // }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            // }
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &depthImageInfo;
            counter++;
        }
        VkDescriptorImageInfo lightDepthImageInfo{}; //for lightdepth sampler
        if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE){
            //std::cout<<"createDescriptorSets_General():GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE"<<std::endl;

            lightDepthImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_GENERAL; 
            lightDepthImageInfo.imageView = lightDepthBuffers[0].view; //lightDepthImageView0; //depth image from swapchain
            lightDepthImageInfo.sampler = lightDepthImageSampler; 

            // if(j < m_texture_ids.size()){
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            // }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            // }
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &lightDepthImageInfo;
            counter++;
        }
        //VkDescriptorImageInfo lightDepthImage_hardware_Info{}; //for lightdepth sampler(HARDWARE)
        VkDescriptorImageInfo lightDepthImage_hardware_Info[LIGHT_MAX] = {};
        if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS){
            //std::cout<<"createDescriptorSets_General():GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS"<<std::endl;
            int lightBufferSize = lightDepthBuffers.size();
            for(int i = 0; i < LIGHT_MAX; i++){
                if(i < lightBufferSize){
                    lightDepthImage_hardware_Info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    lightDepthImage_hardware_Info[i].imageView = lightDepthBuffers[i].view; 
                    lightDepthImage_hardware_Info[i].sampler = lightDepthImageSampler_hardwareDepthBias;
                }else{
                    lightDepthImage_hardware_Info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    lightDepthImage_hardware_Info[i].imageView = lightDepthBuffers[0].view; 
                    lightDepthImage_hardware_Info[i].sampler = lightDepthImageSampler_hardwareDepthBias;
                }
            }
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[counter].descriptorCount = LIGHT_MAX;
            descriptorWrites[counter].pImageInfo = lightDepthImage_hardware_Info;
            counter++;
        }
        /*
        VkDescriptorImageInfo lightDepthImage_hardware_Info2{}; //for lightdepth sampler(HARDWARE)
        if(graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2){
            //std::cout<<"createDescriptorSets_General():GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2"<<std::endl;

            //lightDepthImage_hardware_Info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_GENERAL; 
            lightDepthImage_hardware_Info2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            lightDepthImage_hardware_Info2.imageView = lightDepthBuffers[1].view; //depth image from swapchain TODO: change this
            lightDepthImage_hardware_Info2.sampler = lightDepthImageSampler_hardwareDepthBias2;

            // if(j < m_texture_ids.size()){
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            // }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //     imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
            //     imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            // }
            descriptorWrites[counter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[counter].dstSet = descriptorSets_general[i];
            descriptorWrites[counter].dstBinding = counter;
            descriptorWrites[counter].dstArrayElement = 0;
            descriptorWrites[counter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[counter].descriptorCount = 1;
            descriptorWrites[counter].pImageInfo = &lightDepthImage_hardware_Info2;
            counter++;
        }*/

        //Step 4
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    }

    //std::cout<<"createDescriptorSets_General():done"<<std::endl;
}


/************
* 1 GRAPHCIS_UNIFORMBUFFER_MVP
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::mvpUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
std::vector<void*> CGraphicsDescriptorManager::mvpUniformBuffersMapped;
MVPUniformBufferObject CGraphicsDescriptorManager::mvpUBO;
void CGraphicsDescriptorManager::addMVPUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_MVP;
    //std::cout<<"addMVPUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    mvpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mvpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = mvpUniformBuffers[i].init(sizeof(MVPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), mvpUniformBuffers[i].deviceMemory, 0, sizeof(MVPUniformBufferObject), 0, &mvpUniformBuffersMapped[i]);
    }
}

/************
* 2 GRAPHCIS_UNIFORMBUFFER_TEXT_MVP
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::textMVPUniformBuffers; //need one mvp buffer for each host resource: MAX_FRAMES_IN_FLIGHT
std::vector<void*> CGraphicsDescriptorManager::textMVPUniformBuffersMapped;
TextMVPUniformBufferObject CGraphicsDescriptorManager::textMVPUBO;
void CGraphicsDescriptorManager::addTextMVPUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_TEXT_MVP;
    //std::cout<<"addMVPUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    textMVPUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    textMVPUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = textMVPUniformBuffers[i].init(sizeof(TextMVPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), textMVPUniformBuffers[i].deviceMemory, 0, sizeof(TextMVPUniformBufferObject), 0, &textMVPUniformBuffersMapped[i]);
    }
}


/************
* 3 GRAPHCIS_UNIFORMBUFFER_CUSTOM
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::customUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::customUniformBuffersMapped;
VkDeviceSize CGraphicsDescriptorManager::m_customUniformBufferSize;
void CGraphicsDescriptorManager::addCustomUniformBuffer(VkDeviceSize customUniformBufferSize){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_CUSTOM;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	customUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	customUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	m_customUniformBufferSize = customUniformBufferSize;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = customUniformBuffers[i].init(m_customUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), customUniformBuffers[i].deviceMemory, 0, m_customUniformBufferSize, 0, &customUniformBuffersMapped[i]);
	}
}

/************
* 4 GRAPHCIS_UNIFORMBUFFER_LIGHTING
************/
LightingUniformBufferObject CGraphicsDescriptorManager::m_lightingUBO;
std::vector<CWxjBuffer> CGraphicsDescriptorManager::m_lightingUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::m_lightingUniformBuffersMapped;
VkDeviceSize CGraphicsDescriptorManager::m_lightingUniformBufferSize;
void CGraphicsDescriptorManager::addLightingUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_LIGHTING;
    //std::cout<<"addCustomUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

	m_lightingUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	m_lightingUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	//m_customUniformBufferSize = customUniformBufferSize;
    m_lightingUniformBufferSize = sizeof(LightingUniformBufferObject);
    //std::cout<<"addLightingUniformBuffer::m_lightingUniformBufferSize = " << m_lightingUniformBufferSize<<std::endl;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkResult result = m_lightingUniformBuffers[i].init( m_lightingUniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		vkMapMemory(CContext::GetHandle().GetLogicalDevice(), m_lightingUniformBuffers[i].deviceMemory, 0,  m_lightingUniformBufferSize, 0, & m_lightingUniformBuffersMapped[i]);
	}
}

/************
* 5 GRAPHCIS_UNIFORMBUFFER_VP
************/
std::vector<CWxjBuffer> CGraphicsDescriptorManager::vpUniformBuffers; 
std::vector<void*> CGraphicsDescriptorManager::vpUniformBuffersMapped;
VPUniformBufferObject CGraphicsDescriptorManager::vpUBO;
void CGraphicsDescriptorManager::addVPUniformBuffer(){
    graphicsUniformTypes |= GRAPHCIS_UNIFORMBUFFER_VP;

    vpUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    vpUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult result = vpUniformBuffers[i].init(sizeof(VPUniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), vpUniformBuffers[i].deviceMemory, 0, sizeof(VPUniformBufferObject), 0, &vpUniformBuffersMapped[i]);
    }
}
// bool CGraphicsDescriptorManager::CheckMVP(){ //to check if all objects associate this graphcis descriptor use MVP/VP or not. If return true, means it will use dynamic descriptor offset
//     return ((graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP) 
//         || (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP)
//         || (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP));
// }


/************
* 6 GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE
************/
std::vector<VkSampler> CGraphicsDescriptorManager::textureImageSamplers;
void CGraphicsDescriptorManager::addTextureImageSamplerUniformBuffer(std::vector<int> mipLevels, std::vector<std::vector<bool>> UVWRepeats){
    graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE;//non-static content
    //std::cout<<"addImageSamplerUniformBuffer::uniformBufferUsageFlags = " << uniformBufferUsageFlags<<std::endl;

    textureImageSamplerSize = mipLevels.size();

    for(int i = 0; i < mipLevels.size(); i++){
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = UVWRepeats[i][0] ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = UVWRepeats[i][1] ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = UVWRepeats[i][2] ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        if (mipLevels[i] > 1) {
            //std::cout<<"Sampler MipLevels = "<< mipLevels<<"(>1). Enable mipmap for all textures using this sampler"<<std::endl;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(mipLevels[i] / 2);
            samplerInfo.mipLodBias = 0.0f;
        }

        VkSampler sampler;
        VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &sampler);
        //textureSamplers[textureSamplerCount++]
        textureImageSamplers.push_back(sampler);
        //samplerCount++;
        //REPORT("vkCreateSampler");
    }
}

/************
 * 7 GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE
 ************/
VkSampler CGraphicsDescriptorManager::depthImageSampler;
void CGraphicsDescriptorManager::addDepthImageSamplerUniformBuffer(){
    CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE;
    //std::cout<<"depthImageSampler()" << std::endl;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    // if (mipLevels[i] > 1) {
    //     //std::cout<<"Sampler MipLevels = "<< mipLevels<<"(>1). Enable mipmap for all textures using this sampler"<<std::endl;
    //     samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// VK_SAMPLER_MIPMAP_MODE_NEAREST;
    //     samplerInfo.minLod = 0.0f;
    //     samplerInfo.maxLod = static_cast<float>(mipLevels[i] / 2);
    //     samplerInfo.mipLodBias = 0.0f;
    // }

    //VkSampler sampler;
    VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &depthImageSampler);
    //textureSamplers[textureSamplerCount++]
    //textureImageSamplers.push_back(sampler);
}

/************
 * 8 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE
 ************/
VkSampler CGraphicsDescriptorManager::lightDepthImageSampler;
void CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer(){
    CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE;
    //std::cout<<"depthImageSampler()" << std::endl;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE; //for PCF software shadowmap
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    //samplerInfo.compareEnable = VK_TRUE; //for hardware shadowmap
   // samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &lightDepthImageSampler);
}

/************
 * 9 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS
 ************/
VkSampler CGraphicsDescriptorManager::lightDepthImageSampler_hardwareDepthBias;
void CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias(){
    CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS;
    //std::cout<<"8 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS()" << std::endl;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;//VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;//VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;//VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE; //for hardware shadowmap
    samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &lightDepthImageSampler_hardwareDepthBias);
}

/************
 * 10 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2
 ************/
/*
VkSampler CGraphicsDescriptorManager::lightDepthImageSampler_hardwareDepthBias2;
void CGraphicsDescriptorManager::addLightDepthImageSamplerUniformBuffer_hardwareDepthBias2(){
    CGraphicsDescriptorManager::graphicsUniformTypes |= GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2;
    //std::cout<<"9 GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2()" << std::endl;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(CContext::GetHandle().GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;//VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;//VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;//VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_TRUE; //for hardware shadowmap
    samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(CContext::GetHandle().GetLogicalDevice(), &samplerInfo, nullptr, &lightDepthImageSampler_hardwareDepthBias2);
}*/

/************
* Helper Functions
************/
int CGraphicsDescriptorManager::getPoolSize(){ //to calculate descriptor pool size
	int descriptorPoolSize = 0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) ? 1:0;
	descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE ? textureImageSamplerSize:0; //pool size need count texture
    descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE ? 1:0;
    descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE ? 1:0;
    descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS ? 1:0;
    //descriptorPoolSize += graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2 ? 1:0;

	return descriptorPoolSize;
}
int CGraphicsDescriptorManager::getLayoutSize_General(){ //to get descriptor layout/set size (general: exclude texture)
	int descriptorPoolSize = 0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP || graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_TEXT_MVP) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_CUSTOM) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_LIGHTING) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE) ? 1:0;
    descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS) ? 1:0;
    //descriptorPoolSize += (graphicsUniformTypes & GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2) ? 1:0;

	return descriptorPoolSize;
}
int CGraphicsDescriptorManager::getSetSize_General(){
    return getLayoutSize_General();
}
void CGraphicsDescriptorManager::DestroyAndFree(){
    for(int i = 0; i < textureImageSamplers.size(); i++)
        vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), textureImageSamplers[i], nullptr);
    vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), depthImageSampler, nullptr);
    vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), lightDepthImageSampler, nullptr);
    vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), lightDepthImageSampler_hardwareDepthBias, nullptr);
    //vkDestroySampler(CContext::GetHandle().GetLogicalDevice(), lightDepthImageSampler_hardwareDepthBias2, nullptr);
    
    for (size_t i = 0; i < mvpUniformBuffers.size(); i++) 
        mvpUniformBuffers[i].DestroyAndFree();

    for (size_t i = 0; i < textMVPUniformBuffers.size(); i++) 
        textMVPUniformBuffers[i].DestroyAndFree();

    for (size_t i = 0; i < customUniformBuffers.size(); i++) 
        customUniformBuffers[i].DestroyAndFree();

    for (size_t i = 0; i < vpUniformBuffers.size(); i++) 
        vpUniformBuffers[i].DestroyAndFree();
    
    for (size_t i = 0; i < m_lightingUniformBuffers.size(); i++) 
        m_lightingUniformBuffers[i].DestroyAndFree();
    
    //no need to destroy descriptorSets, because they are from descriptorPool
    vkDestroyDescriptorPool(CContext::GetHandle().GetLogicalDevice(), graphicsDescriptorPool, nullptr);//to be move to base class
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout_general, nullptr);
    vkDestroyDescriptorSetLayout(CContext::GetHandle().GetLogicalDevice(), descriptorSetLayout_textureImageSampler, nullptr);
}
