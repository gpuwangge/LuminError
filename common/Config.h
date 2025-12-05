#pragma once

#define IN
#define OUT
#define INOUT

const int MAX_FRAMES_IN_FLIGHT = 2; //本来是2，交换链数量是3， 这种设计基于一个重要假设：一帧的渲染时间 ≤ 图像呈现间隔。当渲染速度快于显示器刷新率时：layer报错。所以暂时换成3.
const int MIPMAP_TEXTURE_COUNT = 7;

//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr