#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <cassert>
#include "TypeUniform.h"
#include "ILogCore.h"

// =========================
// Geometry Types
// =========================

struct Triangle {
    glm::vec3 v0, v1, v2;
    Triangle() : v0(0), v1(0), v2(0) {}
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
        : v0(a), v1(b), v2(c) {}
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB()
        : min(std::numeric_limits<float>::max()),
          max(std::numeric_limits<float>::lowest()) {}

    AABB(const glm::vec3& mi, const glm::vec3& ma)
        : min(mi), max(ma) {}

    void Expand(const glm::vec3& p) {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }
    void Expand(const AABB& b) {
        Expand(b.min);
        Expand(b.max);
    }

    glm::vec3 Diagonal() const { return max - min; }

    float SurfaceArea() const {
        glm::vec3 d = Diagonal();
        return 2.f * (d.x*d.y + d.x*d.z + d.y*d.z);
    }

    bool IsValid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }
};


// =========================
// BVH Primitive Info
// =========================
struct SimplePrimitiveInfo {
    int orig_index;
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;
    glm::vec3 centroid;

    SimplePrimitiveInfo()
        : orig_index(-1), bbox_min(0), bbox_max(0), centroid(0) {}

    SimplePrimitiveInfo(int id, const AABB& b, const glm::vec3& c)
        : orig_index(id), bbox_min(b.min), bbox_max(b.max), centroid(c) {}
};


// =========================
// BVH Node
// =========================
// struct BVHNode {
//     glm::vec3 bbox_min;
//     glm::vec3 bbox_max;
//     int left;
//     int right;
//     int tri_start;
//     int tri_count;

//     BVHNode()
//         : bbox_min(0), bbox_max(0),
//           left(-1), right(-1),
//           tri_start(-1), tri_count(0) {}
// };


// =========================
// BVH Builder
// =========================
class BVHBuilder {
public:
    BVHBuilder(std::vector<Triangle>& tris, std::vector<BVHNode>& nodes, std::vector<int> &triangleReorderIndices, int leaf_size = 4)
        : triangles(tris), nodes(nodes), triangleReorderIndices(triangleReorderIndices), max_leaf_size(leaf_size)
    {}

    bool Build(LELog::ILogCore *logger = NULL);

private:
    std::vector<Triangle>& triangles;
    std::vector<SimplePrimitiveInfo> primitives;
    std::vector<BVHNode>& nodes;
    std::vector<int> &triangleReorderIndices; 
    int max_leaf_size;

    AABB ComputeTriangleAABB(const Triangle& tri);
    int BuildRecursive(int start, int count, int depth, LELog::ILogCore *logger = NULL);
};

// =========================
// BVH Validation
// =========================
void ValidateBVH(const std::vector<BVHNode>& nodes, int tri_count, LELog::ILogCore *logger = NULL);


// =========================
// BVH Test Cases
// =========================
void CreateTestCase1(std::vector<Triangle>& tris);
void CreateTestCase2(std::vector<Triangle>& tris, bool bVerbose = false);