#include "../include/object.h"
#include "../include/application.h"

/******************
* Object
*******************/
void CObject::Update(float deltaTime, int currentFrame, Camera &mainCamera){
    if(!bRegistered || !bUpdate || !bVisible) return;
    if(p_controlNode != NULL && !p_controlNode->bVisible) return;
    CEntity::Update(deltaTime); //update translateMatrix, RotationMatrix and ScaleMatrix
    /********************************
    * Calculate model matrix based on Translation, Rotation and Scale
    ********************************/
    if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_MVP){
        //update model matrix to ubo
        if(p_controlNode == NULL) {
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = ModelMatrix;
        //    std::cout<<"CObject::Update: object id="<<m_object_id<<" has no control node, use its own model matrix"<<std::endl;
        }else {
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model = p_controlNode->ModelMatrix * ModelMatrix;
        //     std::cout<<"CObject::Update: object id="<<m_object_id<<" use control node model matrix"<<std::endl;
        //     std::cout<<"p_controlNode->ModelMatrix:"<<std::endl;
        //     std::cout<<p_controlNode->ModelMatrix[0][0]<<","<<p_controlNode->ModelMatrix[0][1]<<","<<p_controlNode->ModelMatrix[0][2]<<","<<p_controlNode->ModelMatrix[0][3]<<std::endl;
        //     std::cout<<p_controlNode->ModelMatrix[1][0]<<","<<p_controlNode->ModelMatrix[1][1]<<","<<p_controlNode->ModelMatrix[1][2]<<","<<p_controlNode->ModelMatrix[1][3]<<std::endl;
        //     std::cout<<p_controlNode->ModelMatrix[2][0]<<","<<p_controlNode->ModelMatrix[2][1]<<","<<p_controlNode->ModelMatrix[2][2]<<","<<p_controlNode->ModelMatrix[2][3]<<std::endl;
        //     std::cout<<p_controlNode->ModelMatrix[3][0]<<","<<p_controlNode->ModelMatrix[3][1]<<","<<p_controlNode->ModelMatrix[3][2]<<","<<p_controlNode->ModelMatrix[3][3]<<std::endl;
        //     std::cout<<"ModelMatrix:"<<std::endl;
        //     std::cout<<ModelMatrix[0][0]<<","<<ModelMatrix[0][1]<<","<<ModelMatrix[0][2]<<","<<ModelMatrix[0][3]<<std::endl;
        //     std::cout<<ModelMatrix[1][0]<<","<<ModelMatrix[1][1]<<","<<ModelMatrix[1][2]<<","<<ModelMatrix[1][3]<<std::endl;
        //     std::cout<<ModelMatrix[2][0]<<","<<ModelMatrix[2][1]<<","<<ModelMatrix[2][2]<<","<<ModelMatrix[2][3]<<std::endl;
        //     std::cout<<ModelMatrix[3][0]<<","<<ModelMatrix[3][1]<<","<<ModelMatrix[3][2]<<","<<ModelMatrix[3][3]<<std::endl;
        }

        //update view and perspective matrices to ubo
        if(!bSticker){
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraProj = mainCamera.matrices.projection;
            if(bSkybox) CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = glm::mat4(glm::mat3(mainCamera.matrices.view)); //remove translate
            else CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = mainCamera.matrices.view;
        }else{
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraProj = glm::mat4(1.0f);
            CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].mainCameraView = glm::mat4(1.0f);
        }

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] lightCameraProj:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraProj[3][3]<<std::endl;

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] lightCameraView:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].lightCameraView[3][3]<<std::endl;

        // std::cout<<"myUBO.mvpDATA["<<m_object_id<<"] model:"<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[0][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[1][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[2][3]<<std::endl;
        // std::cout<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][0]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][1]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][2]<<","<<CGraphicsDescriptorManager::mvpUBO.mvpData[m_object_id].model[3][3]<<std::endl;


        //memcpy to GPU memory
        memcpy(CGraphicsDescriptorManager::mvpUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::mvpUBO, sizeof(CGraphicsDescriptorManager::mvpUBO));
    }

    if(CGraphicsDescriptorManager::graphicsUniformTypes & GRAPHCIS_UNIFORMBUFFER_VP){
        CGraphicsDescriptorManager::vpUBO.view = mainCamera.matrices.view;
        CGraphicsDescriptorManager::vpUBO.proj = mainCamera.matrices.projection;
        memcpy(CGraphicsDescriptorManager::vpUniformBuffersMapped[currentFrame], &CGraphicsDescriptorManager::vpUBO, sizeof(CGraphicsDescriptorManager::vpUBO));
    }
}

// float CObject::ComputeDifference(glm::vec3 v1, glm::vec3 v2){
//     return glm::sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
// }


void CObject::CleanUp(){
    //textureDescriptor.DestroyAndFree();
}

void CObject::CreateDescriptorSets_TextureImageSampler(VkDescriptorPool &descriptorPool, VkDescriptorSetLayout &descriptorSetLayout, std::vector<VkSampler> &samplers, std::vector<VkImageView> *swapchainImageViews){
    //std::cout<<"TextureDescriptor::createDescriptorSets."<<std::endl;
    if(samplers.size() < 1) return;
    
    int descriptorSize = samplers.size();//getDescriptorSize();
    //std::cout<<"createTextureDescriptorSets::samplers.size(): "<<samplers.size()<<std::endl;
    //std::cout<<"Object Set(Sampler) size = "<<descriptorSize<<std::endl;

    VkResult result = VK_SUCCESS;

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);///!!!
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);///!!!
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_graphics_texture_image_sampler.resize(MAX_FRAMES_IN_FLIGHT);///!!!
    //Step 3
    result = vkAllocateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), &allocInfo, descriptorSets_graphics_texture_image_sampler.data());
    if (result != VK_SUCCESS) throw std::runtime_error("failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        std::vector<VkWriteDescriptorSet> descriptorWrites;

        descriptorWrites.resize(descriptorSize);
        //int counter = 0;

        VkDescriptorBufferInfo customBufferInfo{}; //for custom uniform

        std::vector<VkDescriptorImageInfo> imageInfo{}; //for texture sampler
        //if(uniformBufferUsageFlags & UNIFORM_BUFFER_SAMPLER_BIT){
        imageInfo.resize(samplers.size());
        for(int j = 0; j < samplers.size(); j++){
            //std::cout<<"CreateTextureDescriptorSets::samplers:"<<j<<std::endl;
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL; //test compute storage image: ?need figure this out. VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            //imageInfo[j].imageView = textureImages[j].m_textureImageBuffer.view;
            //imageInfo[j].sampler = samplers[j];

            // if(b_isText){
            //     if(j < m_text_ids.size()){
            //         imageInfo[j].imageView = p_textImageManager->textureImages[m_text_ids[j]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textImageManager->textureImages[m_text_ids[j]].m_sampler_id]; 
            //     }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
            //         imageInfo[j].imageView = p_textImageManager->textureImages[m_text_ids[0]].m_textureImageBuffer.view;
            //         imageInfo[j].sampler = samplers[p_textImageManager->textureImages[m_text_ids[0]].m_sampler_id]; 
            //     }
            // }else{
            if(j < m_texture_ids.size()){
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[j]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[j]].m_sampler_id]; 
            }else{ //There are more samplers than textures for this object, so use the first texture to fill other samplers
                imageInfo[j].imageView = p_textureManager->textureImages[m_texture_ids[0]].m_textureImageBuffer.view;
                imageInfo[j].sampler = samplers[p_textureManager->textureImages[m_texture_ids[0]].m_sampler_id]; 
            }
            //}

            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptorSets_graphics_texture_image_sampler[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pImageInfo = &imageInfo[j];
            //counter++;
        }
        //}

        //Step 4
        vkUpdateDescriptorSets(CContext::GetHandle().GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }

    //std::cout<<"Done set descriptor. "<<std::endl;
}

void CObject::Register(CApplication *p_app){
    //if(p_app->appInfo.VertexBufferType == VertexStructureTypes::TwoDimension || p_app->appInfo.VertexBufferType == VertexStructureTypes::ThreeDimension){
    Length_original = p_app->modelManager.modelLengths.size() > m_model_id ? p_app->modelManager.modelLengths[m_model_id] : glm::vec3();
    LengthMin_original = p_app->modelManager.modelLengthsMin.size() > m_model_id ? p_app->modelManager.modelLengthsMin[m_model_id] : glm::vec3();
    LengthMax_original = p_app->modelManager.modelLengthsMax.size() > m_model_id ? p_app->modelManager.modelLengthsMax[m_model_id] : glm::vec3();
    // }else{ 
    //     Length_original = glm::vec3(); //put to initialization
    //     LengthMin_original = glm::vec3();
    //     LengthMax_original = glm::vec3();        
    // }
    Length = Length_original;
    //std::cout<<"Length = "<<Length.x<<", "<<Length.y<<", "<<Length.z<<std::endl;
    //std::cout<<"LengthMin = "<<LengthMin.x<<", "<<LengthMin.y<<", "<<LengthMin.z<<std::endl;
    //std::cout<<"LengthMax = "<<LengthMax.x<<", "<<LengthMax.y<<", "<<LengthMax.z<<std::endl;
    
    //Prepare pointers for drawcall
    p_renderer = &(p_app->renderer);
    p_renderProcess = &(p_app->renderProcess);
    //p_graphicsPipelineLayout = &(p_app->renderProcess.graphicsPipelineLayouts[m_graphics_pipeline_id]);
    p_descriptorSets_graphics_general = &(p_app->graphicsDescriptorManager.descriptorSets_general);//?
    p_textureManager = &(p_app->textureManager);
    p_textImageManager = &(p_app->textImageManager);


    //there are up to 3 samplers, support up to 3 different textures
    //for(int i = 0; i < m_texture_ids.size(); i++)
    //    std::cout<<"Register object:"<<m_object_id<<" with texture:"<<m_texture_ids[i]<<" and model:"<<model_id<<" and graphcis pipeline:"<<m_graphics_pipeline_id<<std::endl;

    //bUseTextureSampler = true; 
    if(m_texture_ids.size() > 0){
        CreateDescriptorSets_TextureImageSampler(
            CGraphicsDescriptorManager::graphicsDescriptorPool,
            CGraphicsDescriptorManager::descriptorSetLayout_textureImageSampler,
            CGraphicsDescriptorManager::textureImageSamplers
        );
    }

    bRegistered = true;
}


void CObject::Draw(int graphicsPipelineId, uint32_t n){
    //std::cout<<"CObject::Draw() bVisible:"<<bVisible<<std::endl;
    //if(p_controlNode) std::cout<<"CObject::Draw() p_controlNode->bVisible:"<<p_controlNode->bVisible<<std::endl;
    if(!bRegistered || !bVisible) return;
    if(p_controlNode != NULL && !p_controlNode->bVisible) return;

    int current_graphics_pipeline_id = (graphicsPipelineId == -1) ? m_default_graphics_pipeline_id : graphicsPipelineId;

    VkPipelineLayout *p_graphicsPipelineLayout = &(p_renderProcess->graphicsPipelineLayouts[current_graphics_pipeline_id]);
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[current_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);


    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    if(CGraphicsDescriptorManager::getSetSize_General() > 0) dsSets.push_back(*p_descriptorSets_graphics_general); 
    if(CGraphicsDescriptorManager::textureImageSamplers.size() > 0) dsSets.push_back(descriptorSets_graphics_texture_image_sampler); 
    //std::cout<<"test3.dsSets.size()="<<dsSets.size()<<std::endl;
    
    if(dsSets.size() > 0){
        //int dynamicObjectMVPOffset = -1; //-1 means not use dynamic offset (no MVP/VP used)
        //if(bUseMVP_VP) 
        int dynamicObjectMVPOffset = m_object_id; //assume descriptor uniform(MVP/VP) offset is m_id
        p_renderer->BindGraphicsDescriptorSets(*p_graphicsPipelineLayout, dsSets, dynamicObjectMVPOffset, 0);
    }//else std::cout<<"No Descritpor is used."<<std::endl;
    //std::cout<<"test4."<<std::endl;
    //if(!vertices3D.empty() || !vertices2D.empty()){
    p_renderer->BindVertexBuffer(m_model_id);
    //}//else std::cout<<"No vertex buffer is used."<<std::endl;
    //std::cout<<"test5."<<std::endl;
    //if(indices3D.empty()){
    if(p_renderer->indices3Ds.empty()){
        //std::cout<<"No index buffer is used."<<std::endl;
        p_renderer->Draw(n);
    }else{
        p_renderer->BindIndexBuffer(m_model_id);
        p_renderer->DrawIndexed(m_model_id);
    }

}


void CObject::Draw(std::vector<CWxjBuffer> &buffer, int graphicsPipelineId, uint32_t n){ //const VkBuffer *pBuffers
    if(!bRegistered || !bVisible) return;

    // int graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    //     switch(renderPassType){
    //     case RenderPassTypes::MAINSCENE:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene;
    //         break;
    //     case RenderPassTypes::MAINSCENE2:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene2;
    //         break;
    //     case RenderPassTypes::SHADOWMAP:
    //         graphicsPipelineId = m_graphics_pipeline_id_shadowmap;
    //         break;
    //     default:
    //         graphicsPipelineId = m_graphics_pipeline_id_mainscene; //default to mainscene
    // }
    
    int current_graphics_pipeline_id = (graphicsPipelineId == -1) ? m_default_graphics_pipeline_id : graphicsPipelineId;
    p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[current_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //this function is used in sample:simpleparticle only
    //std::cout<<"testdraw1,"<<m_graphics_pipeline_id<<","<<p_renderer->graphicsCmdId<<std::endl;

    // if(graphicsPipelineId != -1)
    //     p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[graphicsPipelineId], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);
    // else
    //     p_renderer->BindPipeline(p_renderProcess->graphicsPipelines[m_default_graphics_pipeline_id], VK_PIPELINE_BIND_POINT_GRAPHICS, p_renderer->graphicsCmdId);

    //TODO: bind descriptor set
    //std::cout<<"testdraw2"<<std::endl;

    //VkDeviceSize offsets[] = { 0 };
    //vkCmdBindVertexBuffers(p_renderer->commandBuffers[p_renderer->graphicsCmdId][p_renderer->currentFrame], 0, 1, &buffer[p_renderer->currentFrame].buffer, offsets);
    p_renderer->BindExternalBuffer(buffer);
    p_renderer->Draw(n);
    //std::cout<<"testdraw3"<<std::endl;
}
