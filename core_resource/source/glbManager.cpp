
#define TINYGLTF_IMPLEMENTATION 
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
// 已经有 Texture.cpp 实现 stb_image
#define TINYGLTF_NO_STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE_IMPLEMENTATION
#include "glbManager.h"
#include "Foundation.h"
#include <iostream>

namespace LEResource{

void CGLBManager::LoadGLB(const std::string& filename){
    bool ok = loader.LoadBinaryFromFile(&model, &err, &warn, GLB_PATH + filename);

    if (!warn.empty()) std::cout << warn << std::endl;
    if (!err.empty()) std::cout << err << std::endl;
    if (!ok) std::cout << "Load failed\n";

    std::cout<<"GLB Mesh Size = "<< model.meshes.size()<< std::endl;
    //for (size_t i = 0; i < model.meshes.size(); ++i) 
    //    std::cout << "Mesh " << i << ": " << model.meshes[i].name << ", primitive count: " << model.meshes[i].primitives.size() << std::endl;
    std::cout<<"GLB Material Size = "<< model.materials.size()<< std::endl;
    std::cout<<"GLB Image Size = "<< model.images.size()<< std::endl;
    std::cout<<"GLB Texture Size = "<< model.textures.size()<< std::endl;
}

void CGLBManager::LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D){
    //unsigned int meshIndex = 100;
    //unsigned int primitiveIndex = 0;

    auto& mesh = model.meshes[meshIndex];
    // auto& material = model.materials[0];
    // auto& image = model.images[0];
    // auto& texture = model. textures  [0];

    if (mesh.primitives.empty()) {
        throw std::runtime_error("Mesh contains no primitives.");
    }

    // 这里只读取一个 primitive
    const tinygltf::Primitive& primitive = mesh.primitives[primitiveIndex];

    const tinygltf::Accessor* posAccessor = nullptr;
    const tinygltf::Accessor* normalAccessor = nullptr;
    const tinygltf::Accessor* texAccessor = nullptr;

    // POSITION
    auto it = primitive.attributes.find("POSITION");
    if (it != primitive.attributes.end())
        posAccessor = &model.accessors[it->second];

    if (!posAccessor)
        throw std::runtime_error("Mesh has no POSITION.");

    // NORMAL
    it = primitive.attributes.find("NORMAL");
    if (it != primitive.attributes.end())
        normalAccessor = &model.accessors[it->second];

    // TEXCOORD_0
    it = primitive.attributes.find("TEXCOORD_0");
    if (it != primitive.attributes.end())
        texAccessor = &model.accessors[it->second];

    

    // auto GetAccessorData = [&](const std::string& name)
    // {
    //     auto it = primitive.attributes.find(name);
    //     if (it == primitive.attributes.end())
    //         return (const tinygltf::Accessor*)nullptr;

    //     return &model.accessors[it->second];
    // };

    // const tinygltf::Accessor* posAccessor = nullptr;

    // auto it = primitive.attributes.find("POSITION");
    // if (it != primitive.attributes.end())
    // {
    //     posAccessor = &model.accessors[it->second];
    // }

    // if (!posAccessor)
    //     throw std::runtime_error("POSITION not found");

    // const tinygltf::Accessor* posAccessor = GetAccessorData("POSITION");
    // if (!posAccessor)
    //     throw std::runtime_error("Mesh has no POSITION.");

    // const tinygltf::Accessor* normalAccessor = GetAccessorData("NORMAL");
    // const tinygltf::Accessor* texAccessor    = GetAccessorData("TEXCOORD_0");

    //-----------------------------
    // POSITION
    //-----------------------------
    const tinygltf::BufferView& posView =
        model.bufferViews[posAccessor->bufferView];

    const tinygltf::Buffer& posBuffer =
        model.buffers[posView.buffer];

    const float* positions =
        reinterpret_cast<const float*>(
            posBuffer.data.data()
            + posView.byteOffset
            + posAccessor->byteOffset);


    //-----------------------------
    // NORMAL
    //-----------------------------
    const float* normals = nullptr;

    if (normalAccessor)
    {
        const tinygltf::BufferView& normalView =
            model.bufferViews[normalAccessor->bufferView];

        const tinygltf::Buffer& normalBuffer =
            model.buffers[normalView.buffer];

        normals = reinterpret_cast<const float*>(
            normalBuffer.data.data()
            + normalView.byteOffset
            + normalAccessor->byteOffset);
    }


    //-----------------------------
    // TEXCOORD
    //-----------------------------
    const float* texcoords = nullptr;

    if (texAccessor)
    {
        const tinygltf::BufferView& texView =
            model.bufferViews[texAccessor->bufferView];

        const tinygltf::Buffer& texBuffer =
            model.buffers[texView.buffer];

        texcoords = reinterpret_cast<const float*>(
            texBuffer.data.data()
            + texView.byteOffset
            + texAccessor->byteOffset);
    }


    //-----------------------------
    // Vertex
    //-----------------------------
    vertices3D.clear();

    size_t vertexCount = posAccessor->count;

    for (size_t i = 0; i < vertexCount; ++i)
    {
        Vertex3D vertex{};

        vertex.pos = glm::vec3(
            positions[i * 3 + 0],
            positions[i * 3 + 1],
            positions[i * 3 + 2]);

        vertex.color = glm::vec3(1.0f);

        if (normals)
        {
            vertex.normal = glm::vec3(
                normals[i * 3 + 0],
                normals[i * 3 + 1],
                normals[i * 3 + 2]);
        }
        else
        {
            vertex.normal = glm::vec3(0,1,0);
        }

        if (texcoords)
        {
            vertex.texCoord = glm::vec2(
                texcoords[i * 2 + 0],
                1.0f - texcoords[i * 2 + 1]);
        }
        else
        {
            vertex.texCoord = glm::vec2(0);
        }

        vertices3D.push_back(vertex);
    }


    //-----------------------------
    // Index
    //-----------------------------
    indices3D.clear();

    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indexAccessor =
            model.accessors[primitive.indices];

        const tinygltf::BufferView& indexView =
            model.bufferViews[indexAccessor.bufferView];

        const tinygltf::Buffer& indexBuffer =
            model.buffers[indexView.buffer];

        const unsigned char* data =
            indexBuffer.data.data()
            + indexView.byteOffset
            + indexAccessor.byteOffset;

        switch (indexAccessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
            const uint16_t* src =
                reinterpret_cast<const uint16_t*>(data);

            for (size_t i = 0; i < indexAccessor.count; ++i)
                indices3D.push_back(src[i]);

            break;
        }

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        {
            const uint32_t* src =
                reinterpret_cast<const uint32_t*>(data);

            indices3D.assign(src, src + indexAccessor.count);

            break;
        }

        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        {
            const uint8_t* src =
                reinterpret_cast<const uint8_t*>(data);

            for (size_t i = 0; i < indexAccessor.count; ++i)
                indices3D.push_back(src[i]);

            break;
        }

        default:
            throw std::runtime_error("Unsupported index type.");
        }
    }
}

int CGLBManager::GetMeshSize(IN int glbIndex){
    return model.meshes.size();//todo: add glbIndex to suppport multiple glb files
}


}//namespace


