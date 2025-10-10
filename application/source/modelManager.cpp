#include "../include/modelManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../thirdParty/tiny_obj_loader.h"

CModelManager::CModelManager(){}
CModelManager::~CModelManager(){}

void CModelManager::CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage){
	CCustomModel3D model;
	model.vertices = vertices3D;
	model.indices = indices3D;

	float max_x = 0, max_y = 0, max_z = 0;
	float min_x = 0, min_y = 0, min_z = 0;
	for(int i = 0; i < vertices3D.size(); i++){
		max_x = std::max(max_x, vertices3D[i].pos.x);
		max_y = std::max(max_y, vertices3D[i].pos.y);
		max_z = std::max(max_z, vertices3D[i].pos.z);
		min_x = std::min(min_x, vertices3D[i].pos.x);
		min_y = std::min(min_y, vertices3D[i].pos.y);
		min_z = std::min(min_z, vertices3D[i].pos.z);
	}
	model.length = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
	model.lengthMin = glm::vec3(min_x, min_y, min_z);
	model.lengthMax = glm::vec3(max_x, max_y, max_z);

	if(!isTextboxImage) customModels3D.push_back(model);
	else 				textboxImageModels.push_back(model);
}

void CModelManager::CreateCustomModel2D(std::vector<Vertex2D> &vertices2D){
	CCustomModel2D model;
	model.vertices = vertices2D;

	float max_x = 0, max_y = 0, max_z = 0;
	float min_x = 0, min_y = 0, min_z = 0;
	for(int i = 0; i < vertices2D.size(); i++){
		max_x = std::max(max_x, vertices2D[i].pos.x);
		max_y = std::max(max_y, vertices2D[i].pos.y);
		min_x = std::min(min_x, vertices2D[i].pos.x);
		min_y = std::min(min_y, vertices2D[i].pos.y);
	}
	model.length = glm::vec3(max_x-min_x, max_y-min_y, 0);
	model.lengthMin = glm::vec3(min_x, min_y, 0);
	model.lengthMax = glm::vec3(max_x, max_y, 0);

	customModels2D.push_back(model);
}

void CModelManager::CreateTextQuadModel(std::vector<TextQuadVertex> &vertices, std::vector<uint32_t> &indices){
	CTextQuadModel model;
	model.vertices = vertices;
	//model.instanceData = instanceData;
	model.indices = indices;

	// float max_x = 0, max_y = 0, max_z = 0;
	// float min_x = 0, min_y = 0, min_z = 0;
	// for(int i = 0; i < vertices.size(); i++){
	// 	max_x = std::max(max_x, vertices[i].pos.x);
	// 	max_y = std::max(max_y, vertices[i].pos.y);
	// 	min_x = std::min(min_x, vertices[i].pos.x);
	// 	min_y = std::min(min_y, vertices[i].pos.y);
	// }
	// model.length = glm::vec3(max_x-min_x, max_y-min_y, 0); //text quad length is not important, decide to ignore it
	// model.lengthMin = glm::vec3(min_x, min_y, min_z);
	// model.lengthMax = glm::vec3(max_x, max_y, max_z);

	textQuadModels.push_back(model);
}


void CModelManager::LoadObjModel(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

#ifndef ANDROID
	std::string fullModelPath = MODEL_PATH + modelName;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullModelPath.c_str())) {
		fullModelPath = "models/" + modelName;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fullModelPath.c_str())) 
			throw std::runtime_error(warn + err);
	}
		
#else
	std::vector<uint8_t> fileBits;
	std::string fullModelPath = ANDROID_MODEL_PATH + modelName;
	CContext::GetHandle().androidFileManager.AssetReadFile(fullModelPath.c_str(), fileBits);
	std::istringstream in_stream(std::string(fileBits.begin(), fileBits.end()));
   	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &in_stream))
		throw std::runtime_error(warn + err);
#endif
	
	//if use unordered_map, need to implement size_t operator()(Vertex3D const& vertex) const. 
	//for some reason(c++ 11 stl not recognized in GLM) android version doesn't support <glm/gtx/hash.hpp>
	//implment custom hash function instead, checkout dataBuffer.hpp
	std::unordered_map<Vertex3D, uint32_t> uniqueVertices{};

	//vertices3D.clear();
	//indices3D.clear();
	
	float max_x = 0, max_y = 0, max_z = 0;
	float min_x = 0, min_y = 0, min_z = 0;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex3D vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			max_x = std::max(max_x, vertex.pos.x);
			max_y = std::max(max_y, vertex.pos.y);
			max_z = std::max(max_z, vertex.pos.z);
			min_x = std::min(min_x, vertex.pos.x);
			min_y = std::min(min_y, vertex.pos.y);
			min_z = std::min(min_z, vertex.pos.z);

			vertex.color = { 1.0f, 1.0f, 1.0f };

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices3D.size());
				vertices3D.push_back(vertex);
			}

			indices3D.push_back(uniqueVertices[vertex]);
		}
	}
	
	modelLengths.push_back(glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z));
	modelLengthsMin.push_back(glm::vec3(min_x, min_y, min_z));
	modelLengthsMax.push_back(glm::vec3(max_x, max_y, max_z));
}




