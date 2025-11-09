#pragma once

#define IN
#define OUT
#define INOUT

const int MAX_FRAMES_IN_FLIGHT = 2;
const int MIPMAP_TEXTURE_COUNT = 7;

#define OBJECT_TEXT_STRUCTURE_SIZE 256

//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr