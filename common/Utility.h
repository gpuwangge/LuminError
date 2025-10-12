#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//Note: GLM_FORCE_DEPTH_ZERO_TO_ONE is required for Vulkan, because Vulkan uses depth range [0, 1] instead of OpenGL's [-1, 1]

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

// #define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>