#pragma once
#include <optional>
#include <vulkan/vulkan.h>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//Note: GLM_FORCE_DEPTH_ZERO_TO_ONE is required for Vulkan, because Vulkan uses depth range [0, 1] instead of OpenGL's [-1, 1]

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

// #define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> graphicsAndComputeFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsAndComputeFamily.has_value() && graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

// Ray tracing / acceleration structure function pointers
// 这里的 extern 表示“这些变量在别处定义”。
extern PFN_vkGetBufferDeviceAddressKHR                fpGetBufferDeviceAddressKHR;
extern PFN_vkCreateAccelerationStructureKHR           fpCreateAccelerationStructureKHR;
extern PFN_vkDestroyAccelerationStructureKHR          fpDestroyAccelerationStructureKHR;
extern PFN_vkGetAccelerationStructureBuildSizesKHR    fpGetAccelerationStructureBuildSizesKHR;
extern PFN_vkGetAccelerationStructureDeviceAddressKHR fpGetAccelerationStructureDeviceAddressKHR;
extern PFN_vkCmdBuildAccelerationStructuresKHR        fpCmdBuildAccelerationStructuresKHR;
extern PFN_vkBuildAccelerationStructuresKHR           fpBuildAccelerationStructuresKHR;
extern PFN_vkCreateRayTracingPipelinesKHR             fpCreateRayTracingPipelinesKHR;
extern PFN_vkGetRayTracingShaderGroupHandlesKHR       fpGetRayTracingShaderGroupHandlesKHR;
extern PFN_vkCmdTraceRaysKHR                          fpCmdTraceRaysKHR;