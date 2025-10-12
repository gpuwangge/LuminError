#pragma once
#include "utility.h"
#include <vulkan/vulkan.h>
#include <array>

struct TextQuadVertex {
    glm::vec2 pos;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; 
		bindingDescription.stride = sizeof(TextQuadVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TextQuadVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TextQuadVertex, uv);
		return attributeDescriptions;
	}

	bool operator==(const TextQuadVertex& other) const {
		return pos == other.pos && uv == other.uv;
	}    
};

struct TextInstanceData{
    glm::vec2 offset;
    glm::vec3 color;
    glm::vec4 uvRect; //uv range of a specific char in  atlas
    glm::vec2 scale;

    static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1; 
		bindingDescription.stride = sizeof(TextInstanceData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 1; //here use 1 instead of 0, because binding0 is for per vertex data; binding1 is for per instance data
		attributeDescriptions[0].location = 2;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TextInstanceData, offset);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 3;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TextInstanceData, color);

		attributeDescriptions[2].binding = 1;
		attributeDescriptions[2].location = 4;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(TextInstanceData, uvRect);

        attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 5;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(TextInstanceData, scale);
		return attributeDescriptions;
	}

	bool operator==(const TextInstanceData& other) const {
		return offset == other.offset && color == other.color && uvRect == other.uvRect && scale == other.scale;
	}    
};

// Struct to hold glyph texture data
struct GlyphTexture {
    //glm::vec2 pos;
    //glm::vec2 advance_size; //large
    //glm::vec2 real_size; //small
    glm::vec2 size;
    //SDL_Rect texelRect; // The position and size of the glyph within the texture atlas
    glm::vec4 uvRect; // normalized UV coordinates
    //int bearingX;      // Left bearing
    //int bearingY;      // Top bearing
    int advance;   // The horizontal distance to the next glyph
};
