#ifndef H_COMMON
#define H_COMMON

//this macro is already defined somewhere in Android
//#define ANDROID



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






//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr

#include <vulkan/vulkan.h>

#endif