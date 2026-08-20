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
            uniformBufferObject_Material.materials[i].albedo = glm::vec3(appInfo->Materials[i].albedo[0], appInfo->Materials[i].albedo[1], appInfo->Materials[i].albedo[2]);
            uniformBufferObject_Material.materials[i].emissionColor = glm::vec3(appInfo->Materials[i].emissionColor[0], appInfo->Materials[i].emissionColor[1], appInfo->Materials[i].emissionColor[2]);
            uniformBufferObject_Material.materials[i].transmissionColor = glm::vec3(appInfo->Materials[i].transmissionColor[0], appInfo->Materials[i].transmissionColor[1], appInfo->Materials[i].transmissionColor[2]);
            uniformBufferObject_Material.materials[i].type = appInfo->Materials[i].material_type;
            uniformBufferObject_Material.materials[i].metallic = appInfo->Materials[i].metallic;
            uniformBufferObject_Material.materials[i].roughness = appInfo->Materials[i].roughness;
            uniformBufferObject_Material.materials[i].alpha = appInfo->Materials[i].alpha;
            uniformBufferObject_Material.materials[i].emissionStrength = appInfo->Materials[i].emissionStrength;
            uniformBufferObject_Material.materials[i].reflectance = appInfo->Materials[i].reflectance;
            uniformBufferObject_Material.materials[i].specular = appInfo->Materials[i].specular;
            uniformBufferObject_Material.materials[i].ior = appInfo->Materials[i].ior;
            uniformBufferObject_Material.materials[i].transmission = appInfo->Materials[i].transmission;
        }
    }
    //Ray Tracing Setup 2: prepare sphere storage buffer data
    if(appInfo->Uniform.b_storage_compute_sphere){
        for(int i = 0; i < appInfo->CustomSpheres.size() && i < SPHERE_SIZE; i++){
            storageBufferObject_Sphere.spheres[i].position = glm::vec3(appInfo->CustomSpheres[i].custom_sphere_position[0], appInfo->CustomSpheres[i].custom_sphere_position[1], appInfo->CustomSpheres[i].custom_sphere_position[2]);
            storageBufferObject_Sphere.spheres[i].radius = appInfo->CustomSpheres[i].custom_sphere_radius;
            storageBufferObject_Sphere.spheres[i].material_id = appInfo->CustomSpheres[i].custom_sphere_material_id;
            storageBufferObject_Sphere.spheres[i].visibility = appInfo->CustomSpheres[i].custom_sphere_visibility;
        }
    }
}

void GameEngine::SetupComputeRayTracing(){
    //Ray Tracing Setup 3: upload material storage buffer data
    if(appInfo->Uniform.b_storage_compute_material){
        //std::cout<<"sizeof(StructStorageBuffer_Material)="<<sizeof(StructStorageBuffer_Material)<<std::endl;
        UploadComputeUniformBuffer_Material(GetCurrentFrame(), &uniformBufferObject_Material, sizeof(StructUniformBuffer_Material));
        UploadComputeUniformBuffer_Material(GetCurrentFrame()+1, &uniformBufferObject_Material, sizeof(StructUniformBuffer_Material));
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
            //std::cout<<"Filling data for object "<<j<<", position=("<<objects[j].Position.x<<","<<objects[j].Position.y<<","<<objects[j].Position.z<<")"<<std::endl;
            //std::cout<<"    Object "<<j<<" has "<<modelData[objects[j].m_model_id].modelVertices3D.size()<<" vertices and "<<modelData[objects[j].m_model_id].modelIndices3D.size()/3<<" triangles."<<std::endl;
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
            //std::cout<<"    vertexOffset after filling object "<<j<<": "<<vertexOffset<<std::endl;
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

void GameEngine::SetupRayTracing(bool bVerboseRaytracing){
    /****************
    * BLAS for triangles
    * Create buffer address so BLAS(in renderer core) can use
    * 建立BLAS就是建立Geometry,也就是model的信心。只需要建立一次就可以了。
    * 以前(上面的code)是通过storageBufferObject_TriangleVertexAttribute(本质是个TriangleVertexInfo的数组)建立的，这里使用storageBufferObject_GeometryInfo
    * storageBufferObject_GeometryInfo里面有一个vertex buffer地址，这个地址也应该指向一个TriangleVertexInfo的数组
    **************/
    rtMeshes.resize(modelData.size());
    //if(bVerboseRaytracing) std::cout<<"Done resize rtMeshes to: "<<rtMeshes.size()<<std::endl;
    
    for(int i = 0; i < modelData.size(); i++){
        rtMeshes[i].modelId = i;
        rtMeshes[i].vertexCount = static_cast<uint32_t>(modelData[i].modelVertices3D.size());
        rtMeshes[i].indexCount = static_cast<uint32_t>(modelData[i].modelIndices3D.size());
        //if(bVerboseRaytracing) std::cout << "vertexCount = " << rtMeshes[i].vertexCount << std::endl;
        //if(bVerboseRaytracing) std::cout << "indexCount = " << rtMeshes[i].indexCount << std::endl;

        std::vector<TriangleVertexInfo> modelVertexAttribute;
        rtMeshes[i].vertexStride = sizeof(TriangleVertexInfo);
        modelVertexAttribute.resize(rtMeshes[i].vertexCount);
        for(int j = 0; j < rtMeshes[i].vertexCount; j++){
            modelVertexAttribute[j].normal = modelData[i].modelVertices3D[j].normal;
            modelVertexAttribute[j].position = modelData[i].modelVertices3D[j].pos;
            modelVertexAttribute[j].color = modelData[i].modelVertices3D[j].color;
            modelVertexAttribute[j].uv = modelData[i].modelVertices3D[j].texCoord;
            modelVertexAttribute[j].tangent = modelData[i].modelVertices3D[j].tangent;
        }
        //std::cout<<"test1"<<std::endl;
        //std::cout.flush();

        rtMeshes[i].vertexBuffer.init(
            sizeof(TriangleVertexInfo) * rtMeshes[i].vertexCount,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            renderer->GetLogicalDevice(),
            renderer->GetPhysicalDevice(),
            true
        );
        //std::cout<<"test1.5"<<std::endl;
        //std::cout.flush();
        //test
        // if(i == 1){
        //     std::cout<<"try to print out some position."<<std::endl;
        //     std::cout.flush();
        //     TriangleVertexInfo* p = modelVertexAttribute.data();
        //     std::cout << p[0].position.x << std::endl;
        //     std::cout.flush();
        //     std::cout << p[100].position.x << std::endl;
        //     std::cout << p[1000].position.x << std::endl;
        //     std::cout << p[10000].position.x << std::endl;
        //     std::cout << p[100000].position.x << std::endl;
        //     std::cout << p[437644].position.x << std::endl;
        //     std::cout.flush();
        // }

        VkResult result = rtMeshes[i].vertexBuffer.fill(modelVertexAttribute.data(), renderer->GetLogicalDevice());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to fill vertex buffer");
        }
        //std::cout<<"test2"<<std::endl;
        //std::cout.flush();

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
        //std::cout<<"test3"<<std::endl;

        fpGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(renderer->GetLogicalDevice(), "vkGetBufferDeviceAddressKHR"));

        rtMeshes[i].vertexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[i].vertexBuffer.buffer);
        rtMeshes[i].indexAddress = GetBufferAddress(renderer->GetLogicalDevice(), rtMeshes[i].indexBuffer.buffer);

        storageBufferObject_GeometryInfo.geometryInfos[i].vertexBuf = rtMeshes[i].vertexAddress;
        storageBufferObject_GeometryInfo.geometryInfos[i].indexBuf = rtMeshes[i].indexAddress;

        //int textureIndex = i%69;
        //std::cout<<"assign "<<i<<" to storageBufferObject_GeometryInfo.geometryInfos[i].materialIndex"<<std::endl;
        //storageBufferObject_GeometryInfo.geometryInfos[i].materialIndex = i; //todo

        // std::cout<<"rtMeshes["<<i<<"].vertexAddress = "<<rtMeshes[i].vertexAddress<<std::endl;
        // std::cout<<"rtMeshes["<<i<<"].indexAddress = "<<rtMeshes[i].indexAddress<<std::endl;
        // std::cout<<"rtMeshes["<<i<<"].vertexCount = "<<rtMeshes[i].vertexCount<<std::endl;
        // std::cout<<"rtMeshes["<<i<<"].indexCount = "<<rtMeshes[i].indexCount<<std::endl;
        // std::cout<<"rtMeshes["<<i<<"].vertexStride = "<<rtMeshes[i].vertexStride<<std::endl;

        //以下是送到rchit shader的内容
        // logger->Log("i = {}", i);
        // logger->Log("rtMeshes[{}].indexAddress = {}", i, rtMeshes[i].indexAddress);
        // for(int j = 0; j < modelData[i].modelIndices3D.size(); j++) 
        //     logger->Log("modelData[{}].modelIndices3D[{}] = {}", i, j, modelData[i].modelIndices3D[j]);
        // logger->Log("rtMeshes[{}].vertexAddress = {}", i, rtMeshes[i].vertexAddress);
        // for(int j = 0; j < modelVertexAttribute.size(); j++){
        //     logger->Log("modelVertexAttribute[{}].position = {}, {}, {}", j, modelVertexAttribute[j].position.x, modelVertexAttribute[j].position.y, modelVertexAttribute[j].position.z);
        //     logger->Log("modelVertexAttribute[{}].normal = {}, {}, {}", j, modelVertexAttribute[j].normal.x, modelVertexAttribute[j].normal.y, modelVertexAttribute[j].normal.z);
        // }
        //if(bVerboseRaytracing) std::cout<<"Done upload geometry(model): "<<i<<std::endl;
        std::cout.flush();
    }
    //if(bVerboseRaytracing) std::cout<<"Done upload all geometry(model) data to storage buffer objects."<<std::endl;

    //static_assert(sizeof(GeometryInfoGPU) == 16);
    //static_assert(alignof(GeometryInfoGPU) >= 8);

    UploadRaytracingStorageBuffer_GeometryInfo(GetCurrentFrame(), &storageBufferObject_GeometryInfo, sizeof(StructStorageBuffer_GeometryInfo));
    UploadRaytracingStorageBuffer_GeometryInfo(GetCurrentFrame()+1, &storageBufferObject_GeometryInfo, sizeof(StructStorageBuffer_GeometryInfo));
    //if(bVerboseRaytracing) std::cout<<"Done upload geometry info to device."<<std::endl;

    // static_assert(sizeof(VkDeviceAddress) == 8);
    // static_assert(alignof(GeometryInfoGPU) == 16);
    // static_assert(offsetof(GeometryInfoGPU, vertexBuf) == 0);
    // static_assert(offsetof(GeometryInfoGPU, indexBuf) == 8);
    // static_assert(offsetof(GeometryInfoGPU, materialIndex) == 16);
    // static_assert(offsetof(GeometryInfoGPU, _pad0) == 20);
    // static_assert(sizeof(GeometryInfoGPU) == 32);

    /****************
    * BLAS for Spheres
    **************/
    rtSpheres.resize(appInfo->CustomSpheres.size());
    for(int i = 0; i < appInfo->CustomSpheres.size(); i++){
        rtSpheres[i].center = glm::vec3(appInfo->CustomSpheres[i].custom_sphere_position[0], appInfo->CustomSpheres[i].custom_sphere_position[1], appInfo->CustomSpheres[i].custom_sphere_position[2]);
        rtSpheres[i].radius = appInfo->CustomSpheres[i].custom_sphere_radius;
        rtSpheres[i].materialIndex = appInfo->CustomSpheres[i].custom_sphere_material_id;
    }

    /****************
    * Custom for Triangles and Spheres
    **************/
    if(appInfo->Uniform.b_storage_raytracing_material){
        yamler->ReadMaterialYAMLFile("Materials");
        for(int i = 0; i < appInfo->Materials.size(); i++){
            uniformBufferObject_Material.materials[i].albedo = glm::vec3(appInfo->Materials[i].albedo[0], appInfo->Materials[i].albedo[1], appInfo->Materials[i].albedo[2]);
            uniformBufferObject_Material.materials[i].emissionColor = glm::vec3(appInfo->Materials[i].emissionColor[0], appInfo->Materials[i].emissionColor[1], appInfo->Materials[i].emissionColor[2]);
            uniformBufferObject_Material.materials[i].transmissionColor = glm::vec3(appInfo->Materials[i].transmissionColor[0], appInfo->Materials[i].transmissionColor[1], appInfo->Materials[i].transmissionColor[2]);
            uniformBufferObject_Material.materials[i].type = appInfo->Materials[i].material_type;
            uniformBufferObject_Material.materials[i].metallic = appInfo->Materials[i].metallic;
            uniformBufferObject_Material.materials[i].roughness = appInfo->Materials[i].roughness;
            uniformBufferObject_Material.materials[i].alpha = appInfo->Materials[i].alpha;
            uniformBufferObject_Material.materials[i].emissionStrength = appInfo->Materials[i].emissionStrength;
            uniformBufferObject_Material.materials[i].reflectance = appInfo->Materials[i].reflectance;
            uniformBufferObject_Material.materials[i].specular = appInfo->Materials[i].specular;
            uniformBufferObject_Material.materials[i].ior = appInfo->Materials[i].ior;
            uniformBufferObject_Material.materials[i].transmission = appInfo->Materials[i].transmission;
        }
    }
    //std::cout<<"SetupRayTracing(): Found "<<appInfo->Materials.size()<<" materials."<<std::endl;
    if(appInfo->Uniform.b_storage_raytracing_material){
        renderer->uploadRaytracingUniformBuffer_material(GetCurrentFrame(), &uniformBufferObject_Material, sizeof(StructUniformBuffer_Material));
        renderer->uploadRaytracingUniformBuffer_material(GetCurrentFrame()+1, &uniformBufferObject_Material, sizeof(StructUniformBuffer_Material));
    }
    //if(bVerboseRaytracing) std::cout<<"Done upload material info to device."<<std::endl;

    //Ray Tracing: prepare rt lights buffer data
    //std::cout<<"Preparing RT Lights buffer data..."<<appInfo->RTLights.size()<<std::endl;
    //TODO: bool switch
    for(int i = 0; i < appInfo->RTLights.size() && i < RTLIGHT_SIZE; i++){
        uniformBufferObject_rtLight.lights[i].position = glm::vec4(appInfo->RTLights[i].rt_light_position[0], appInfo->RTLights[i].rt_light_position[1], appInfo->RTLights[i].rt_light_position[2], 1.0f);
        uniformBufferObject_rtLight.lights[i].color = glm::vec4(appInfo->RTLights[i].rt_light_color[0], appInfo->RTLights[i].rt_light_color[1], appInfo->RTLights[i].rt_light_color[2], 1.0f);
        uniformBufferObject_rtLight.lights[i].direction = glm::vec4(appInfo->RTLights[i].rt_light_direction[0], appInfo->RTLights[i].rt_light_direction[1], appInfo->RTLights[i].rt_light_direction[2], 1.0f);
        uniformBufferObject_rtLight.lights[i].lightParams[0] = appInfo->RTLights[i].rt_light_intensity;
        uniformBufferObject_rtLight.lights[i].lightParams[1] = appInfo->RTLights[i].rt_light_radius;
        uniformBufferObject_rtLight.lights[i].lightParams[2] = appInfo->RTLights[i].rt_light_range;
        uniformBufferObject_rtLight.lights[i].lightParams[3] = appInfo->RTLights[i].rt_light_type;
        uniformBufferObject_rtLight.lights[i].attenuation[0] = appInfo->RTLights[i].rt_light_falloff;
        //uniformBufferObject_rtLight.lights[i].angle = appInfo->RTLights[i].rt_light_angle;
        //uniformBufferObject_rtLight.lights[i].type = appInfo->RTLights[i].rt_light_type;
        //storageBufferObject_rtLight.lights[i].rt_light_id = appInfo->RTLights[i].rt_light_id;
    }

    renderer->uploadRaytracingUniformBuffer_rtLight(GetCurrentFrame(), &uniformBufferObject_rtLight, sizeof(StructUniformBuffer_RtLight));
    renderer->uploadRaytracingUniformBuffer_rtLight(GetCurrentFrame()+1, &uniformBufferObject_rtLight, sizeof(StructUniformBuffer_RtLight));
    //if(bVerboseRaytracing) std::cout<<"Done upload ray tracing light info to device."<<std::endl;
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
