#include "gameEngine.h"
#include "Foundation.h"
#include <locale>
#include <iomanip>
#include <iostream>
#include "bvhBuilder.h"

namespace LEGameEngine{

void GameEngine::InitializeComputeRayTracing(){
    //Ray Tracing Setup 1: prepare material storage buffer data
    if(appInfo->Uniform.b_storage_compute_material){
        yamler->ReadMaterialYAMLFile("Materials");
        for(int i = 0; i < appInfo->Materials.size(); i++){
            storageBufferObject_Material.materials[i].albedo = glm::vec3(appInfo->Materials[i].albedo[0], appInfo->Materials[i].albedo[1], appInfo->Materials[i].albedo[2]);
            storageBufferObject_Material.materials[i].emissionColor = glm::vec3(appInfo->Materials[i].emissionColor[0], appInfo->Materials[i].emissionColor[1], appInfo->Materials[i].emissionColor[2]);
            storageBufferObject_Material.materials[i].transmissionColor = glm::vec3(appInfo->Materials[i].transmissionColor[0], appInfo->Materials[i].transmissionColor[1], appInfo->Materials[i].transmissionColor[2]);
            storageBufferObject_Material.materials[i].metallic = appInfo->Materials[i].metallic;
            storageBufferObject_Material.materials[i].roughness = appInfo->Materials[i].roughness;
            storageBufferObject_Material.materials[i].alpha = appInfo->Materials[i].alpha;
            storageBufferObject_Material.materials[i].emissionStrength = appInfo->Materials[i].emissionStrength;
            storageBufferObject_Material.materials[i].reflectance = appInfo->Materials[i].reflectance;
            storageBufferObject_Material.materials[i].specular = appInfo->Materials[i].specular;
            storageBufferObject_Material.materials[i].ior = appInfo->Materials[i].ior;
            storageBufferObject_Material.materials[i].transmission = appInfo->Materials[i].transmission;
        }
    }
    //Ray Tracing Setup 2: prepare sphere storage buffer data
    if(appInfo->Uniform.b_storage_compute_sphere){
        for(int i = 0; i < appInfo->ComputeSpheres.size() && i < SPHERE_SIZE; i++){
            storageBufferObject_Sphere.spheres[i].position = glm::vec3(appInfo->ComputeSpheres[i].compute_sphere_position[0], appInfo->ComputeSpheres[i].compute_sphere_position[1], appInfo->ComputeSpheres[i].compute_sphere_position[2]);
            storageBufferObject_Sphere.spheres[i].radius = appInfo->ComputeSpheres[i].compute_sphere_radius;
            storageBufferObject_Sphere.spheres[i].material_id = appInfo->ComputeSpheres[i].compute_sphere_material_id;
            storageBufferObject_Sphere.spheres[i].visibility = appInfo->ComputeSpheres[i].compute_sphere_visibility;
        }
    }
}

void GameEngine::SetupComputeRayTracing(){
    //Ray Tracing Setup 3: upload material storage buffer data
    if(appInfo->Uniform.b_storage_compute_material){
        //std::cout<<"sizeof(StructStorageBuffer_Material)="<<sizeof(StructStorageBuffer_Material)<<std::endl;
        UploadComputeStorageBuffer_Material(GetCurrentFrame(), &storageBufferObject_Material, sizeof(StructStorageBuffer_Material));
        UploadComputeStorageBuffer_Material(GetCurrentFrame()+1, &storageBufferObject_Material, sizeof(StructStorageBuffer_Material));
        //std::cout<<"Uploaded material storage buffer to GPU."<<std::endl;
    }

    //Ray Tracing Setup 4: prepare triangle vertex and index storage buffer data
    std::vector<Vertex3D> allVertices3D_forBVH; //this is just for BVH construction, not the actual storage buffer data. The actual storage buffer data is stored in storageBufferObject_TriangleVertexAttribute and storageBufferObject_TriangleVertexIndex
    std::vector<uint32_t> allIndices3D_forBVH;
    if(appInfo->Uniform.b_storage_compute_triangle_vertex_attribute && appInfo->Uniform.b_storage_compute_triangle_vertex_index){
        int vertexCount = 0;
        int indexCount = 0;
        int vertexOffset = 0;
        for(int j = 0; j < objects.size(); j++){ //Assume each object uses one model for now
            std::cout<<"Filling data for object "<<j<<", position=("<<objects[j].Position.x<<","<<objects[j].Position.y<<","<<objects[j].Position.z<<")"<<std::endl;
            std::cout<<"    Object "<<j<<" has "<<modelData[objects[j].m_model_id].modelVertices3D.size()<<" vertices and "<<modelData[objects[j].m_model_id].modelIndices3D.size()/3<<" triangles."<<std::endl;
            // Vertex Data for a 3d model
            int modelIndex = objects[j].m_model_id;
            for(int i = 0; i < modelData[modelIndex].modelVertices3D.size(); i++){
                glm::vec3 tranformedVertexPos; //this is to do what vertex shader normally does, transform the vertex position from model space to world space by applying scale and translation (no rotation for now)
                tranformedVertexPos = modelData[modelIndex].modelVertices3D[i].pos * objects[j].Scale;
                tranformedVertexPos = tranformedVertexPos + objects[j].Position;
                storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].position = tranformedVertexPos;
                storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].normal = modelData[modelIndex].modelVertices3D[i].normal;
                storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].material_id = objects[j].m_material_id;
                vertexCount++;

                Vertex3D vertexForBVH;
                vertexForBVH.pos = tranformedVertexPos;
                allVertices3D_forBVH.push_back(vertexForBVH);
            }
            // Index Data for a 3d model
            for(int i = 0; i < modelData[modelIndex].modelIndices3D.size(); i++){
                uint32_t vertexIndex = modelData[modelIndex].modelIndices3D[i] + vertexOffset;
                storageBufferObject_TriangleVertexIndex.indices[indexCount] = vertexIndex;
                indexCount++;

                allIndices3D_forBVH.push_back(vertexIndex);
                //std::cout<<"    Filling index "<<indexCount-1<<": "<<storageBufferObject_TriangleVertexIndex.indices[indexCount-1]<<std::endl;
            }
            vertexOffset += modelData[modelIndex].modelVertices3D.size();
            std::cout<<"    vertexOffset after filling object "<<j<<": "<<vertexOffset<<std::endl;
            logger->Log("BVH: {} vertices and {} indices filled so far.", vertexCount, indexCount);
            //std::cout<<vertexCount<<" vertices and "<<indexCount<<" indices filled so far."<<std::endl;
        }
    }

    //Ray Tracing Setup 5: create BVH for triangle data
    if(appInfo->Uniform.b_storage_compute_triangle_vertex_attribute && appInfo->Uniform.b_storage_compute_triangle_vertex_index && appInfo->Uniform.b_storage_compute_triangle_reorder_index && appInfo->Uniform.b_storage_compute_bvhnode){
        // logger->Log("BVH: creation for triangle: total vertices=modelVertexSize*objectSize={}, modelVertexSize={}, modelIndexSize={}, modelTriangleSize=modelIndexSize/3={}, total triangles=modelTriangleSize*objectSize={}", 
        //     modelVertices3D.size()* objects.size(),
        //     modelVertices3D.size(),
        //     modelIndices3D.size(),
        //     modelIndices3D.size()/3,
        //     modelIndices3D.size()/3 * objects.size());

        // std::cout<<"Create BVH for triangle data: "<<std::endl;
        // std::cout<<"Number of vertices: "<<modelVertices3D.size()* objects.size()<<std::endl;
        // std::cout<<"Each object has "<<modelVertices3D.size()<<" vertices."<<std::endl;
        // std::cout<<"Each object has "<<modelIndices3D.size()/3<<" triangles."<<std::endl;
        // std::cout<<"Number of triangles: "<<modelIndices3D.size()/3 * objects.size()<<std::endl;

        std::cout<<"allVertices3D_forBVH size: "<<allVertices3D_forBVH.size()<<std::endl;
        std::cout<<"allIndices3D_forBVH size: "<<allIndices3D_forBVH.size()<<std::endl;

        std::vector<Triangle> tris;
        for(int i = 0; i < allIndices3D_forBVH.size(); i+=3){
            glm::vec3 v0 = allVertices3D_forBVH[allIndices3D_forBVH[i]].pos;
            glm::vec3 v1 = allVertices3D_forBVH[allIndices3D_forBVH[i+1]].pos;
            glm::vec3 v2 = allVertices3D_forBVH[allIndices3D_forBVH[i+2]].pos;
            tris.emplace_back(v0, v1, v2);
        }
        //CreateTestCase2(tris, false);
        // int modelIndex = 1; //todo
        // for(int i = 0; i < modelData[modelIndex].modelIndices3D.size(); i+=3){
        //     glm::vec3 v0 = modelData[modelIndex].modelVertices3D[modelData[modelIndex].modelIndices3D[i]].pos;
        //     glm::vec3 v1 = modelData[modelIndex].modelVertices3D[modelData[modelIndex].modelIndices3D[i+1]].pos;
        //     glm::vec3 v2 = modelData[modelIndex].modelVertices3D[modelData[modelIndex].modelIndices3D[i+2]].pos;
        //     tris.emplace_back(v0, v1, v2);
        // }
        logger->Log("BVH: tris created {} triangles from model data.\n", tris.size());
        std::cout<<"Created "<<tris.size()<<" triangles from model data."<<std::endl;

        std::vector<BVHNode> nodes;
        std::vector<int> triangleReorderIndices; 
        BVHBuilder builder(tris, nodes, triangleReorderIndices, 3);

        builder.Build(logger);
        ValidateBVH(nodes, tris.size(), logger);

        for (int i = 0; i < nodes.size(); i++) storageBufferObject_BVHNode.nodes[i] = nodes[i];
        for(int i = 0; i < triangleReorderIndices.size(); i++) storageBufferObject_TriangleReorderIndex.indices[i] = triangleReorderIndices[i];
    }
    
    //Ray Tracing Setup 6: upload triangle vertex and index and bvh storage buffer data
    if(appInfo->Uniform.b_storage_compute_triangle_vertex_attribute){
        //Vertex Data for a quad(two triangles)
        //Vulkan use right-hand coordinate system, glm use right-hand too
        //Vulkan NDC: +X right, +Y down, +Z forward
        //glm NDC: +X right, +Y up, +Z backward
        //World space: +X right, +Y up, +Z forward
        // storageBufferObject_TriangleVertex.vertices[0].position = glm::vec3(-1.5f, 1.5f, 0.0f); //in world space, this is top-left
        // storageBufferObject_TriangleVertex.vertices[1].position = glm::vec3(-1.5f, -1.5f, 0.0f); //in world space, this is bottom-left
        // storageBufferObject_TriangleVertex.vertices[2].position = glm::vec3(1.5f, 1.5f, 0.0f); //in world space, this is top-right
        // storageBufferObject_TriangleVertex.vertices[3].position = glm::vec3(1.5f, -1.5f, 0.0f); //in world space, this is bottom-right
        // storageBufferObject_TriangleVertex.vertices[0].normal = glm::vec3(0.0f, 0.0f, 1.0f);
        // storageBufferObject_TriangleVertex.vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
        // storageBufferObject_TriangleVertex.vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
        // storageBufferObject_TriangleVertex.vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
        // storageBufferObject_TriangleVertex.vertices[0].material_id = 1;
        // storageBufferObject_TriangleVertex.vertices[1].material_id = 0;
        // storageBufferObject_TriangleVertex.vertices[2].material_id = 1;
        // storageBufferObject_TriangleVertex.vertices[3].material_id = 0;
        UploadComputeStorageBuffer_TriangleVertexAttribute(GetCurrentFrame(), &storageBufferObject_TriangleVertexAttribute, sizeof(StructStorageBuffer_TriangleVertexAttribute));
        UploadComputeStorageBuffer_TriangleVertexAttribute(GetCurrentFrame()+1, &storageBufferObject_TriangleVertexAttribute, sizeof(StructStorageBuffer_TriangleVertexAttribute));
    }
    if(appInfo->Uniform.b_storage_compute_triangle_vertex_index){
        // Index Data for a quad(two triangles)
        // storageBufferObject_TriangleIndex.indices[0] = 0;
        UploadComputeStorageBuffer_TriangleVertexIndex(GetCurrentFrame(), &storageBufferObject_TriangleVertexIndex, sizeof(StructStorageBuffer_TriangleVertexIndex));
        UploadComputeStorageBuffer_TriangleVertexIndex(GetCurrentFrame()+1, &storageBufferObject_TriangleVertexIndex, sizeof(StructStorageBuffer_TriangleVertexIndex));
    }
    if(appInfo->Uniform.b_storage_compute_triangle_reorder_index){
        UploadComputeStorageBuffer_TriangleReorderIndex(GetCurrentFrame(), &storageBufferObject_TriangleReorderIndex, sizeof(StructStorageBuffer_TriangleReorderIndex));
        UploadComputeStorageBuffer_TriangleReorderIndex(GetCurrentFrame()+1, &storageBufferObject_TriangleReorderIndex, sizeof(StructStorageBuffer_TriangleReorderIndex));
    }

    if(appInfo->Uniform.b_storage_compute_bvhnode){
        UploadComputeStorageBuffer_BVHNode(GetCurrentFrame(), &storageBufferObject_BVHNode, sizeof(StructStorageBuffer_BVHNode));
        UploadComputeStorageBuffer_BVHNode(GetCurrentFrame()+1, &storageBufferObject_BVHNode, sizeof(StructStorageBuffer_BVHNode));
    }
    //Ray Tracing Setup 7: Upload sphere storage buffer data
    if(appInfo->Uniform.b_storage_compute_sphere){
        UploadComputeStorageBuffer_Sphere(GetCurrentFrame(), &storageBufferObject_Sphere, sizeof(StructStorageBuffer_Sphere));
        UploadComputeStorageBuffer_Sphere(GetCurrentFrame()+1, &storageBufferObject_Sphere, sizeof(StructStorageBuffer_Sphere));
    }
}

VkDeviceAddress GameEngine::GetBufferAddress(VkDevice device, VkBuffer buffer) {
    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
    info.buffer = buffer;
    return fpGetBufferDeviceAddressKHR(device, &info);
}

void GameEngine::SetupRayTracing(){
    /********
     * Load data from modelData into allVertices3D and allIndices3D
     * if size of vertices is 20 and size of triangle is 10, then allVertices3D.size() = 20, allIndices3D.size() = 30
     */
    std::vector<Vertex3D> allVertices3D;
    std::vector<uint32_t> allIndices3D;
    int vertexCount = 0;
    int indexCount = 0;
    int vertexOffset = 0;
    std::cout<<"SetupRayTracing(): Found "<<objects.size()<<" objects."<<std::endl;
    std::cout<<"SetupRayTracing(): Found "<<modelData.size()<<" models."<<std::endl;
    for(int j = 0; j < objects.size(); j++){
        std::cout<<"Filling data for object "<<j<<", position=("<<objects[j].Position.x<<","<<objects[j].Position.y<<","<<objects[j].Position.z<<")"<<std::endl;
        std::cout<<"    Object "<<j<<" uses model_id="<<objects[j].m_model_id<<", and has "<<modelData[objects[j].m_model_id].modelVertices3D.size()<<" vertices and "<<modelData[objects[j].m_model_id].modelIndices3D.size()/3<<" triangles."<<std::endl;
        // Vertex Data for a 3d model
        int modelIndex = objects[j].m_model_id;
        for(int i = 0; i < modelData[modelIndex].modelVertices3D.size(); i++){
            glm::vec3 tranformedVertexPos; //this is to do what vertex shader normally does, transform the vertex position from model space to world space by applying scale and translation (no rotation for now)
            //tranformedVertexPos = modelData[modelIndex].modelVertices3D[i].pos * objects[j].Scale;
            //tranformedVertexPos = tranformedVertexPos + objects[j].Position;
            tranformedVertexPos = modelData[modelIndex].modelVertices3D[i].pos; //not use scale and translation
            storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].position = tranformedVertexPos;
            storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].normal = modelData[modelIndex].modelVertices3D[i].normal;
            storageBufferObject_TriangleVertexAttribute.vertices[vertexCount].material_id = objects[j].m_material_id;
            vertexCount++;

            Vertex3D vertexForBVH;
            vertexForBVH.pos = tranformedVertexPos;
            allVertices3D.push_back(vertexForBVH);
        }
        // Index Data for a 3d model
        for(int i = 0; i < modelData[modelIndex].modelIndices3D.size(); i++){
            uint32_t vertexIndex = modelData[modelIndex].modelIndices3D[i] + vertexOffset;
            storageBufferObject_TriangleVertexIndex.indices[indexCount] = vertexIndex;
            indexCount++;

            allIndices3D.push_back(vertexIndex);
            //std::cout<<"    Filling index "<<indexCount-1<<": "<<storageBufferObject_TriangleVertexIndex.indices[indexCount-1]<<std::endl;
        }
        vertexOffset += modelData[modelIndex].modelVertices3D.size();
        std::cout<<"    vertexOffset after filling object "<<j<<": "<<vertexOffset<<std::endl;
        logger->Log("BVH: {} vertices and {} indices filled so far.", vertexCount, indexCount);
    }
    //std::cout<<"allVertices3D.size = "<<allVertices3D.size()<<std::endl;
    //std::cout<<"allIndices3D.size = "<<allIndices3D.size()<<std::endl;

    /****************
    * Upload triangle vertex/index to storage buffer TODO: change from compute to ray tracing
    *****************/
    //if(appInfo->Uniform.b_storage_compute_triangle_vertex_attribute){
        UploadRaytracingStorageBuffer_TriangleVertexAttribute(GetCurrentFrame(), &storageBufferObject_TriangleVertexAttribute, sizeof(StructStorageBuffer_TriangleVertexAttribute));
        UploadRaytracingStorageBuffer_TriangleVertexAttribute(GetCurrentFrame()+1, &storageBufferObject_TriangleVertexAttribute, sizeof(StructStorageBuffer_TriangleVertexAttribute));
    //}
    //if(appInfo->Uniform.b_storage_compute_triangle_vertex_index){
        // Index Data for a quad(two triangles)
        // storageBufferObject_TriangleIndex.indices[0] = 0;
        UploadRaytracingStorageBuffer_TriangleVertexIndex(GetCurrentFrame(), &storageBufferObject_TriangleVertexIndex, sizeof(StructStorageBuffer_TriangleVertexIndex));
        UploadRaytracingStorageBuffer_TriangleVertexIndex(GetCurrentFrame()+1, &storageBufferObject_TriangleVertexIndex, sizeof(StructStorageBuffer_TriangleVertexIndex));
    //}


    /****************
    * Create buffer address so BLAS(in renderer core) can use
    **************/
    rtMeshes.resize(modelData.size());
    
    for(int i = 0; i < modelData.size(); i++){
        rtMeshes[i].modelId = i;
        rtMeshes[i].vertexCount = static_cast<uint32_t>(modelData[i].modelVertices3D.size());
        rtMeshes[i].indexCount = static_cast<uint32_t>(modelData[i].modelIndices3D.size());
        rtMeshes[i].vertexStride = sizeof(Vertex3D);

        rtMeshes[i].vertexBuffer.init(
            sizeof(Vertex3D) * rtMeshes[i].vertexCount,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            renderer->GetLogicalDevice(),
            renderer->GetPhysicalDevice(),
            true
        );
        rtMeshes[i].vertexBuffer.fill(modelData[i].modelVertices3D.data(), renderer->GetLogicalDevice());
        rtMeshes[i].indexBuffer.init(
            sizeof(uint32_t) * rtMeshes[i].indexCount,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            renderer->GetLogicalDevice(),
            renderer->GetPhysicalDevice(),
            true
        );
        rtMeshes[i].indexBuffer.fill(modelData[i].modelIndices3D.data(), renderer->GetLogicalDevice());

        fpGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(renderer->GetLogicalDevice(), "vkGetBufferDeviceAddressKHR"));

        rtMeshes[i].vertexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[i].vertexBuffer.buffer);
        rtMeshes[i].indexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[i].indexBuffer.buffer);

        std::cout<<"rtMeshes["<<i<<"].vertexAddress = "<<rtMeshes[i].vertexAddress<<std::endl;
        std::cout<<"rtMeshes["<<i<<"].indexAddress = "<<rtMeshes[i].indexAddress<<std::endl;
        std::cout<<"rtMeshes["<<i<<"].vertexCount = "<<rtMeshes[i].vertexCount<<std::endl;
        std::cout<<"rtMeshes["<<i<<"].indexCount = "<<rtMeshes[i].indexCount<<std::endl;
        std::cout<<"rtMeshes["<<i<<"].vertexStride = "<<rtMeshes[i].vertexStride<<std::endl;
    }


    // rtMeshes[0].modelId = 0;
    // rtMeshes[0].vertexCount = static_cast<uint32_t>(allVertices3D.size());
    // rtMeshes[0].indexCount = static_cast<uint32_t>(allIndices3D.size());
    // rtMeshes[0].vertexStride = sizeof(Vertex3D);

    // rtMeshes[0].vertexBuffer.init(
    //     sizeof(Vertex3D) * allVertices3D.size(),
    //     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
    //     VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
    //     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    //     renderer->GetLogicalDevice(),
    //     renderer->GetPhysicalDevice(),
    //     true
    // );
    // rtMeshes[0].vertexBuffer.fill(allVertices3D.data(), renderer->GetLogicalDevice());
    // rtMeshes[0].indexBuffer.init(
    //     sizeof(uint32_t) * allIndices3D.size(),
    //     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
    //     VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
    //     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    //     renderer->GetLogicalDevice(),
    //     renderer->GetPhysicalDevice(),
    //     true
    // );
    // rtMeshes[0].indexBuffer.fill(allIndices3D.data(), renderer->GetLogicalDevice());

    // fpGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(renderer->GetLogicalDevice(), "vkGetBufferDeviceAddressKHR"));

    // rtMeshes[0].vertexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[0].vertexBuffer.buffer);
    // rtMeshes[0].indexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[0].indexBuffer.buffer);

    // std::cout<<"rtMeshes[0].vertexAddress = "<<rtMeshes[0].vertexAddress<<std::endl;
    // std::cout<<"rtMeshes[0].indexAddress = "<<rtMeshes[0].indexAddress<<std::endl;
    // std::cout<<"rtMeshes[0].vertexCount = "<<rtMeshes[0].vertexCount<<std::endl;
    // std::cout<<"rtMeshes[0].indexCount = "<<rtMeshes[0].indexCount<<std::endl;
    // std::cout<<"rtMeshes[0].vertexStride = "<<rtMeshes[0].vertexStride<<std::endl;

    // triangleVertexCount = static_cast<uint32_t>(allVertices3D.size());
    // triangleIndexCount  = static_cast<uint32_t>(allIndices3D.size());
    // triangleVertexStride = sizeof(Vertex3D);

    // create + upload vertex buffer
    // raytracing_vertex_buffer.init(
    //     sizeof(Vertex3D) * allVertices3D.size(),
    //     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
    //     VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
    //     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    //     renderer->GetLogicalDevice(),
    //     renderer->GetPhysicalDevice(),
    //     true
    // );

    //raytracing_vertex_buffer.fill(allVertices3D.data(), renderer->GetLogicalDevice());

    // create + upload index buffer
    // raytracing_index_buffer.init(
    //     sizeof(uint32_t) * allIndices3D.size(),
    //     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
    //     VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
    //     VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    //     renderer->GetLogicalDevice(),
    //     renderer->GetPhysicalDevice(),
    //     true
    // );

    //raytracing_index_buffer.fill(allIndices3D.data(), renderer->GetLogicalDevice());


    //fpGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(renderer->GetLogicalDevice(), "vkGetBufferDeviceAddressKHR"));

    // get device addresses
    //raytracing_vertex_buffer_address = GetBufferAddress(renderer->GetLogicalDevice(), raytracing_vertex_buffer.buffer);
   //raytracing_index_buffer_address = GetBufferAddress(renderer->GetLogicalDevice(), raytracing_index_buffer.buffer);

    // std::cout<<"raytracing_vertex_buffer_address = "<<raytracing_vertex_buffer_address<<std::endl;
    // std::cout<<"raytracing_index_buffer_address = "<<raytracing_index_buffer_address<<std::endl;
    // std::cout<<"triangleVertexCount = "<<triangleVertexCount<<std::endl;
    // std::cout<<"triangleIndexCount = "<<triangleIndexCount<<std::endl;
    // std::cout<<"triangleVertexStride = "<<triangleVertexStride<<std::endl;
}

void GameEngine::Trace(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ){
    std::vector<std::vector<VkDescriptorSet>> dsSets; 
    dsSets.push_back(renderer->GetRaytracingDescriptorSets());

    //renderer->BindComputeDescriptorSets(renderer->GetRaytracingPipelineLayout(), dsSets);
    renderer->BindRaytracingDescriptorSets(renderer->GetRaytracingPipelineLayout(), dsSets);

    //renderer->Dispatch(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);
    renderer->Trace(numWorkGroupsX, numWorkGroupsY, numWorkGroupsZ);

}


}
