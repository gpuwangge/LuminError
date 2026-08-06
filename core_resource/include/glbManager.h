#pragma once
#include <tiny_gltf.h>
#include "TypeVertex.h"

namespace LEResource{

class CGLBManager final{
public:
    CGLBManager() {}
    ~CGLBManager() {}

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string warn;
    std::string err;

    void LoadGLB(const std::string& filename);
    void LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D);

    int GetMeshSize(IN int glbIndex);
};

}//namespace