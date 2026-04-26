#include "bvhBuilder.h"

#define LOGSPACE 4

AABB BVHBuilder::ComputeTriangleAABB(const Triangle& tri) {
    AABB box;
    box.Expand(tri.v0);
    box.Expand(tri.v1);
    box.Expand(tri.v2);

    // avoid zero-size AABB
    const float eps = 0.001f;
    glm::vec3 d = box.Diagonal();
    for (int i = 0; i < 3; i++) {
        if (d[i] < eps) {
            box.min[i] -= eps * 0.5f;
            box.max[i] += eps * 0.5f;
        }
    }
    return box;
}

bool BVHBuilder::Build(LELog::ILogCore *logger) {
	try {
		//std::cout << "== BVH Build Start ==\n";
		if(logger) logger->Log("======BVH Build Start=====");

		primitives.clear();
		primitives.reserve(triangles.size());

		//std::cout << "Generating primitive info...\n";
		if(logger) logger->Log("Generating primitive info...");

		for (int i = 0; i < triangles.size(); i++) {
			AABB b = ComputeTriangleAABB(triangles[i]);
			glm::vec3 c = (triangles[i].v0 + triangles[i].v1 + triangles[i].v2) * (1.f / 3.f);

			if(logger){
				logger->Log("Tri {} AABB min=[{}, {}, {}] max=[{}, {}, {}] centroid=[{}, {}, {}]",
					i, 
					b.min.x, b.min.y, b.min.z,
					b.max.x, b.max.y, b.max.z,
					c.x, c.y, c.z);
			} 
			// std::cout << "Tri " << i
			// 			<< " AABB min(" << b.min.x << "," << b.min.y << "," << b.min.z << ")"
			// 			<< " max(" << b.max.x << "," << b.max.y << "," << b.max.z << ")"
			// 			<< " centroid(" << c.x << "," << c.y << "," << c.z << ")\n";

			primitives.emplace_back(i, b, c);
		}

		nodes.clear();
		nodes.reserve(triangles.size() * 2);
		triangleReorderIndices.reserve(triangles.size());
		for(int i = 0; i < triangles.size(); i++) triangleReorderIndices.push_back(i);

		int root_index = BuildRecursive(0, (int)primitives.size(), 0, logger);
		if(logger){
			logger->Log("=====BVH Build Finished=====\nRoot node = {}\nTotal Nodes: {}\n", 
            	root_index, nodes.size());
		}
		// std::cout << "BVH Build Finished. Root node = " << root_index
		// 			<< "\nTotal Nodes: " << nodes.size() << "\n";

		for(int i = 0; i< primitives.size(); i++) triangleReorderIndices[i] = primitives[i].orig_index;

		return true;
	}
	catch (const std::exception& e) {
		std::cout << "BVH Build Exception: " << e.what() << "\n";
		return false;
	}
	catch (...) {
		std::cout << "BVH Build Unknown Exception.\n";
		return false;
	}
}


int BVHBuilder::BuildRecursive(int start, int count, int depth, LELog::ILogCore *logger) {
	if (count <= 0) return -1;

	if(logger) {
		std::string space = "";
		for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
		logger->Log(space+"[Depth {}] BuildRecursive start={} count={}", depth, start, count);
	}
	// std::cout << std::string(depth * 2, ' ')
	// 			<< "[Depth " << depth << "] BuildRecursive start=" << start
	// 			<< " count=" << count << "\n";

	// compute node bbox
	AABB box;
	for (int i = start; i < start + count; i++) {
		box.Expand(primitives[i].bbox_min);
		box.Expand(primitives[i].bbox_max);
	}
	if (!box.IsValid()) {
		std::cout << "Invalid AABB detected!\n";
		return -1;
	}

	// create node index
	int parent_node_index = nodes.size();
	nodes.emplace_back();

	// leaf condition
	if (count <= max_leaf_size) {
		BVHNode& leaf = nodes[parent_node_index];
		leaf.bbox_min = box.min;
		leaf.bbox_max = box.max;
		leaf.tri_start = start;
		leaf.tri_count = count;

		if(logger){
			std::string space = "";
			for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
			logger->Log(space+"Leaf created: tri_start={} tri_count={}", start, count);
		}
		// std::cout << std::string(depth * 2, ' ')
		// 			<< "Leaf created: tri_start=" << start
		// 			<< " tri_count=" << count << "\n";
		return parent_node_index;
	}

	// choose split axis
	glm::vec3 extent = box.Diagonal();
	int axis = (extent.x >= extent.y && extent.x >= extent.z) ? 0 :
				(extent.y >= extent.z) ? 1 : 2;

	if(logger){
		std::string space = "";
		for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
		logger->Log(space+"Split axis = {} (extent=[{}, {}, {}])", axis, extent.x, extent.y, extent.z);
	} 
	// std::cout << std::string(depth * 2, ' ')
	// 			<< "Split axis = " << axis << " (extent=" << extent.x << "," << extent.y << "," << extent.z << ")\n";

	auto begin = primitives.begin() + start;
	auto end = begin + count;

	std::sort(begin, end, [&](const SimplePrimitiveInfo& a, const SimplePrimitiveInfo& b) {
		return a.centroid[axis] < b.centroid[axis];
	});
	
	if(logger){
		std::string space = "";
		for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
		logger->Log(space+"Centroids after sorting:");
	// std::cout << std::string(depth * 2, ' ')
	// 			<< "Centroids after sorting:";

		std::string s = "";
		for (int i = start; i < start + count; i++) {
			s += std::to_string(primitives[i].centroid[axis]);
			s += " ";
			//logger->Log(" {}", primitives[i].centroid[axis]);
		}
		logger->Log(space + s);
	}
	if(logger){
		std::string space = "";
		for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
		logger->Log(space+"Original indices after sorting:");
		std::string s = "";
		for (int i = start; i < start + count; i++) {
			s += std::to_string(primitives[i].orig_index);
			s += " ";
		}
		logger->Log(space + s);
	}


	// for (int i = start; i < start + count; i++)
	// 	if(bVerbose) std::cout << " " << primitives[i].centroid[axis];
	// if(bVerbose) std::cout << "\n";

	int mid = start + count / 2;

	int left_child_node_index = BuildRecursive(start, mid - start, depth + 1, logger);
	int right_child_node_index = BuildRecursive(mid, start + count - mid, depth + 1, logger);

	// compute final bbox from children
	AABB fbox;
	if (left_child_node_index != -1)
		fbox.Expand(AABB(nodes[left_child_node_index].bbox_min, nodes[left_child_node_index].bbox_max));
	if (right_child_node_index != -1)
		fbox.Expand(AABB(nodes[right_child_node_index].bbox_min, nodes[right_child_node_index].bbox_max));

	BVHNode& node = nodes[parent_node_index];
	node.bbox_min = fbox.min;
	node.bbox_max = fbox.max;
	node.left = left_child_node_index;
	node.right = right_child_node_index;

	if(logger){
		std::string space = "";
		for(int i = 0; i < depth*LOGSPACE; i++) space+=" ";
		logger->Log(space+"Internal node created: left={} right={}", left_child_node_index, right_child_node_index);
	} 
	//std::cout << std::string(depth * 2, ' ')
	//			<< "Internal node created: left=" << left_child_node_index << " right=" << right_child_node_index << "\n";
	return parent_node_index;
}


void ValidateBVH(const std::vector<BVHNode>& nodes, int tri_count, LELog::ILogCore *logger) {
    //std::cout << "\n== BVH Validation ==\n";
	if(logger) logger->Log("=====BVH Validation=====");

    if (nodes.empty()) {
        //std::cout << "ERROR: empty BVH\n";
		if(logger) logger->Log("empty BVH\n");
        return;
    }

    std::vector<bool> covered(tri_count, false);
    int sum_leaf_tris = 0;

    for (int i = 0; i < nodes.size(); i++) {
        const BVHNode& n = nodes[i];

        if(logger){
			logger->Log("Node {}: bbox_min = ({}, {}, {}), bbox_max = ({}, {}, {})",
				i,
				n.bbox_min.x, n.bbox_min.y, n.bbox_min.z,
				n.bbox_max.x, n.bbox_max.y, n.bbox_max.z);
		} 
		// std::cout << "Node " << i << ": bbox("
        //           << n.bbox_min.x << "," << n.bbox_min.y << "," << n.bbox_min.z << ") - ("
        //           << n.bbox_max.x << "," << n.bbox_max.y << "," << n.bbox_max.z << ")";

        if (n.tri_count > 0)
            if(logger) logger->Log("  [Leaf: start={} count={}]", n.tri_start, n.tri_count);
			//std::cout << "  [Leaf: start=" << n.tri_start << " count=" << n.tri_count << "]";

        //if(logger) logger->Log("\n");
		//std::cout << "\n";

        if (n.tri_count > 0) {
            sum_leaf_tris += n.tri_count;
            for (int t = n.tri_start; t < n.tri_start + n.tri_count; t++) {
                if (t >= 0 && t < tri_count) covered[t] = true;
            }
        }
    }

    bool ok = true;
    for (int i = 0; i < tri_count; i++) {
        if (!covered[i]) {
            //std::cout << "Missing tri " << i << "\n";
			if(logger) logger->Log("Missing tri {}\n", i);
            ok = false;
        }
    }

    if (ok) if(logger) logger->Log("All triangles covered.");
	//std::cout << "All triangles covered.\n";
    if(logger) logger->Log("Sum of triangles in leaves = {}", sum_leaf_tris);
	//std::cout << "Sum of triangles in leaves = " << sum_leaf_tris << "\n";

    // bbox hierarchy check
    for (int i = 0; i < nodes.size(); i++) {
        const BVHNode& n = nodes[i];
        if (n.left != -1) {
            for (int c : { n.left, n.right }) {
                if (c != -1) {
                    const BVHNode& ch = nodes[c];
                    for (int ax = 0; ax < 3; ax++) {
                        if (ch.bbox_min[ax] < n.bbox_min[ax] - 1e-4f ||
                            ch.bbox_max[ax] > n.bbox_max[ax] + 1e-4f) {
							if(logger) logger->Log("BBox error at node {} child {}\n", i, c);
                            //std::cout << "BBox error at node " << i << " child " << c << "\n";
                            ok = false;
                        }
                    }
                }
            }
        }
    }

    if (ok) if(logger) logger->Log("Hierarchy bbox OK.");
	//std::cout << "Hierarchy bbox OK.\n";
    if(logger) logger->Log("=====BVH Validation Finished=====\n");
	//std::cout << "== End BVH Validation ==\n";
}

// =========================
// Example main()
// =========================

void CreateTestCase1(std::vector<Triangle>& tris) {
	tris.emplace_back(glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0));
	tris.emplace_back(glm::vec3(2,2,0), glm::vec3(3,2,0), glm::vec3(2,3,0));
	tris.emplace_back(glm::vec3(-1,-1,0), glm::vec3(-2,-1,0), glm::vec3(-1,-2,0));
}

void CreateTestCase2(std::vector<Triangle>& tris, bool bVerbose) {
	tris.reserve(20);

    // helper lambda：生成一个随机三角形
    auto makeTri = [&](float cx, float cy, float scale) {
        float dx1 = ((rand() % 100) / 100.f - 0.5f) * scale;
        float dy1 = ((rand() % 100) / 100.f - 0.5f) * scale;
        float dx2 = ((rand() % 100) / 100.f - 0.5f) * scale;
        float dy2 = ((rand() % 100) / 100.f - 0.5f) * scale;
        float dx3 = ((rand() % 100) / 100.f - 0.5f) * scale;
        float dy3 = ((rand() % 100) / 100.f - 0.5f) * scale;

        return Triangle(
            glm::vec3(cx + dx1, cy + dy1, 0),
            glm::vec3(cx + dx2, cy + dy2, 0),
            glm::vec3(cx + dx3, cy + dy3, 0)
        );
    };

    // 放置在不同区域
    srand(1234); // 固定随机种子，便于调试

    // cluster 1：左下
    for (int i=0; i<5; i++) tris.push_back(makeTri(-5, -5, 2.0f));

    // cluster 2：右上
    for (int i=0; i<5; i++) tris.push_back(makeTri(5, 5, 2.5f));

    // cluster 3：右下
    for (int i=0; i<5; i++) tris.push_back(makeTri(5, -4, 2.0f));

    // cluster 4：中间区域
    for (int i=0; i<5; i++) tris.push_back(makeTri(0, 0, 3.0f));

    // 输出测试数据
	if(bVerbose) {
    std::cout << "===== Test Triangles (20) =====\n";
		for (int i = 0; i < tris.size(); i++) {
			const auto& t = tris[i];
			std::cout << "Tri " << i << ": "
					<< "(" << t.v0.x << "," << t.v0.y << "), "
					<< "(" << t.v1.x << "," << t.v1.y << "), "
					<< "(" << t.v2.x << "," << t.v2.y << ")\n";
		}
	}
}

// int main() {
//     std::vector<Triangle> tris;
// 	CreateTestCase2(tris, false);

//     std::vector<BVHNode> nodes;
//     BVHBuilder builder(tris, nodes, 1);

//     builder.Build(false);
//     ValidateBVH(nodes, tris.size(), false);

//     return 0;
// }
