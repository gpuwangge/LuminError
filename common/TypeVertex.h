#pragma once
#include "utility.h"
#include <vulkan/vulkan.h>
#include <array>

struct Vertex3D {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; 
        bindingDescription.stride = sizeof(Vertex3D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;  //binding is in use in vertex shader(default is using 0)
        attributeDescriptions[0].location = 0; //location is in use in vertex shader
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex3D, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex3D, normal);

        return attributeDescriptions;
    }

    bool operator==(const Vertex3D& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
    }
};

struct Vertex2D {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2D, color);

        return attributeDescriptions;
    }

    bool operator==(const Vertex2D& other) const {
        return pos == other.pos && color == other.color;
    }
};


namespace std {
	//Custom hash function operator() for Vertex3D
	//operator(): input is Vertex3D type hash key, output is std::size_t hash value
	//There are different ways to implement this, choose a method for high quality hash function
	void inline custom_hash_combine(size_t &seed, size_t hash){
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}
	template<> struct hash<Vertex3D> { 
		size_t operator()(Vertex3D const& vertex) const {
            //code from vulkan tutorial, removed this because of android compitability issue
			//return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
            
			//return 0; 
			//explain: this will disable hash algorithm. code still works, but not optimized

			size_t seed = 0;
			custom_hash_combine(seed, hash<float>()(vertex.pos.x));
			custom_hash_combine(seed, hash<float>()(vertex.pos.y));
			custom_hash_combine(seed, hash<float>()(vertex.pos.z));
			custom_hash_combine(seed, hash<float>()(vertex.color.x));
			custom_hash_combine(seed, hash<float>()(vertex.color.y));
			custom_hash_combine(seed, hash<float>()(vertex.color.z));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.x));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.x));
			custom_hash_combine(seed, hash<float>()(vertex.normal.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.z));
			return seed;
		}
	};
}


struct TextureAttributeInfo{
    std::string name;
    int miplevel;
    int samplerid;
    bool enableCubemap;
};

enum VertexStructureTypes{
    NoType,
    TwoDimension,
    ThreeDimension,
    ParticleType,
    TextQuad
};


struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
};

