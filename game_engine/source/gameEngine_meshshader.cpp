#include "gameEngine.h"
#include "Foundation.h"
#include <locale>
#include <iomanip>
#include <iostream>

namespace LEGameEngine{

void GameEngine::SetupMeshShader(){
    //std::cout<<"GameEngine::SetupMeshShader()"<<std::endl;

    //intialize storageBufferObject_VertexFloatData and storageBufferObject_IndexUintData;
    //std::cout<<"objects.size(): "<<objects.size()<<std::endl;
    for(int j = 0; j < 1; j++){ //todo: objects.size()
        // Vertex Data for a 3d model
        int modelIndex = 0;//objects[j].m_model_id;
        //std::cout<<"modelIndex: "<<modelIndex<<std::endl;
        //std::cout<<"modelData.size(): "<<modelData.size()<<std::endl;
        //std::cout<<"modelData[modelIndex].modelVertices3D.size(): "<<modelData[modelIndex].modelVertices3D.size()<<std::endl;
        for(int i = 0; i < modelData[modelIndex].modelVertices3D.size(); i++){
            //std::cout<<"i="<<i<<std::endl;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 0] = modelData[modelIndex].modelVertices3D[i].pos.x;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 1] = modelData[modelIndex].modelVertices3D[i].pos.y;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 2] = modelData[modelIndex].modelVertices3D[i].pos.z;

            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 3] = modelData[modelIndex].modelVertices3D[i].color.x;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 4] = modelData[modelIndex].modelVertices3D[i].color.y;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 5] = modelData[modelIndex].modelVertices3D[i].color.z;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 6] = modelData[modelIndex].modelVertices3D[i].texCoord.x;
            storageBufferObject_VertexFloatData.vertexFloatData[i*8 + 7] = modelData[modelIndex].modelVertices3D[i].texCoord.y;
        }
        //std::cout<<"modelData[modelIndex].modelVertices3D.size(): "<<modelData[modelIndex].modelVertices3D.size()<<std::endl;

        // Index Data for a 3d model
        for(int i = 0; i < modelData[modelIndex].modelIndices3D.size(); i++){
            storageBufferObject_IndexUintData.indexUintData[i] = modelData[modelIndex].modelIndices3D[i];
        }
        //std::cout<<"modelData[modelIndex].modelIndices3D.size(): "<<modelData[modelIndex].modelIndices3D.size()<<std::endl;
    }

    //uint32_t currentFrame, const void* data_vertex, const void* data_index, size_t dataSize_vertex, size_t dataSize_index
    //std::cout<<"Uploading mesh shader storage buffer to GPU..."<<std::endl;
    renderer->uploadGraphicsStorageBuffer_meshshader(GetCurrentFrame(),
        &storageBufferObject_VertexFloatData, &storageBufferObject_IndexUintData, 
        sizeof(VertexFloatDataStruct), sizeof(IndexUintDataStruct)
    );
    renderer->uploadGraphicsStorageBuffer_meshshader(GetCurrentFrame()+1,
        &storageBufferObject_VertexFloatData, &storageBufferObject_IndexUintData, 
        sizeof(VertexFloatDataStruct), sizeof(IndexUintDataStruct)
    );

    //std::cout<<"Uploaded mesh shader storage buffer to GPU."<<std::endl;

    if(!renderer->LoadMeshShaderFunctions()) {
        throw std::runtime_error("Failed to load mesh shader functions");
    }

    //below should be between begincmdbuffer and endcommandbuffer
    /*
    renderer->BindGraphicsPipeline(renderer->GetGraphicsPipeline(2), VK_PIPELINE_BIND_POINT_GRAPHICS);

    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    //set = 0 is for general uniform; set = 1 is for texture sampler uniform
    std::vector<VkDescriptorSet> *p_descriptorSets_graphics_general = &(renderer->GetDescriptorSets_General());
    if(renderer->GetSetSize_General() > 0) dsSets.push_back(*p_descriptorSets_graphics_general); 
    //if(renderer->GetTextureImageSamplersSize() > 0) dsSets.push_back(descriptorSets_graphics_texture_image_sampler); 
    
    //std::cout<<"dsSets.size()="<<dsSets.size()<<std::endl;
    //std::cout<<"p_renderer->indices3Ds.size()="<<p_renderer->indices3Ds.size()<<std::endl;

    int dynamicObjectOffset = 0;
    renderer->BindGraphicsDescriptorSets(renderer->GetGraphicsPipelineLayout(2), dsSets, dynamicObjectOffset, 0);

    renderer->DrawMeshTasks(10, 1, 1);
    */
}



}
