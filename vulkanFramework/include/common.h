#ifndef H_COMMON
#define H_COMMON

//this macro is already defined somewhere in Android
//#define ANDROID

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//Note: GLM_FORCE_DEPTH_ZERO_TO_ONE is required for Vulkan, because Vulkan uses depth range [0, 1] instead of OpenGL's [-1, 1]

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

// #define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>

#include <SDL3_ttf/SDL_ttf.h>
#include <locale>
#include <codecvt>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <cstring>
#include <iomanip>
#include <sstream>

#include <optional>
#include <array>
#include <vector>
#include <set>
#include <unordered_map>

#include <algorithm>
#include <chrono>

#include <memory>

//not sure usefulness...
// #include <stdexcept>
// #include <cstdlib>
// #include <cstdint>
// #include <limits>

const uint32_t WINDOW_WIDTH = 800; //1024;//1080;
const uint32_t WINDOW_HEIGHT = 800; //1280;//2400;
const int MAX_FRAMES_IN_FLIGHT = 2;

const int MIPMAP_TEXTURE_COUNT = 7;

#define IN
#define OUT
#define INOUT

//#define TEXTURE_PATH "../textures/"
#define TEXTURE_PATH "../../androidSandbox/app/src/main/assets/textures/"
//#define MODEL_PATH "../models/"
#define MODEL_PATH "../../androidSandbox/app/src/main/assets/models/"
//#define SHADER_PATH "../shaders/"
#define SHADER_PATH "../../androidSandbox/app/src/main/shaders/"
#define ANDROID_TEXTURE_PATH "textures/"
#define ANDROID_MODEL_PATH "models/"
#define ANDROID_SHADER_PATH "shaders/"

//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr

#include <vulkan/vulkan.h>

#endif