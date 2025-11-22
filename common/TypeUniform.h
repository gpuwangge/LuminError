#pragma once
#include "utility.h"
#include "Foundation.h"
#include <cstring>
#include <vulkan/vulkan.h>
// #include <iostream>
// #include <chrono>

//Uniform Naming Rule: PipelineType_DescriptorType_Purpose
enum UniformTypes {
    GRAPHCIS_UNIFORMBUFFER_GLOBAL =                                     0x00000001,
    GRAPHCIS_UNIFORMBUFFER_OBJECT_DYNAMIC =                             0x00000002,
    GRAPHCIS_UNIFORMBUFFER_TEXT_DYNAMIC =                               0x00000004,
    GRAPHCIS_UNIFORMBUFFER_LIGHTING =                                   0x00000008,
    GRAPHCIS_UNIFORMBUFFER_CUSTOM =                                     0x00000010,
    GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE =                        0x00000020,
    GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE =                          0x00000040,  //for main camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE =                     0x00000080,  //for light camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS =   0x00000100,  //for light camera(Hardware depth bias, use two renderpass, dynamic depth bias)
    COMPUTE_UNIFORMBUFFER_GLOBAL =      0x00000200,
    COMPUTE_STORAGEBUFFER_WINDOWSWAP =  0x00000400,
    COMPUTE_STORAGEBUFFER_MATERIAL =    0x00000800,
    COMPUTE_STORAGEBUFFER_TRIANGLE =    0x00001000,
    COMPUTE_STORAGEBUFFER_SPHERE =      0x00002000,
    COMPUTE_UNIFORMBUFFER_CUSTOM =      0x00004000,
    COMPUTE_STORAGEBUFFER_CUSTOMSWAP =  0x00008000,
    COMPUTE_STORAGEIMAGE_TEXTURE =      0x00010000,
    COMPUTE_STORAGEIMAGE_SWAPCHAIN =    0x00020000
};

struct alignas(16) SphereInfo{
    alignas(16) glm::vec3 position;
    alignas(4) float radius;
    alignas(4) int material_id;
    alignas(4) float padding[2];
};  //total size: 16+4+4+4*2=32 bytes

static constexpr size_t SPHERE_SIZE = 64;//assume max 64 materials for now
struct StructStorageBuffer_Sphere{
    SphereInfo spheres[SPHERE_SIZE];
};



struct alignas(16) MaterialInfo{
    alignas(16) glm::vec3 albedo;
    alignas(16) glm::vec3 emissionColor;
    alignas(16) glm::vec3 transmissionColor;
    alignas(4) float metallic;
    alignas(4) float roughness;
    alignas(4) float alpha;
    alignas(4) float emissionStrength;
    alignas(4) float reflectance;
    alignas(4) float specular;
    alignas(4) float ior;
    alignas(4) float transmission;
    //alignas(4) float padding[44]; //44*4=176 bytes 
}; //total size: 16*3+4*8=80, need+176=256 bytes but no need?

static constexpr size_t MATERIAL_SIZE = 64;//assume max 64 materials for now
struct StructStorageBuffer_Material{
    MaterialInfo materials[MATERIAL_SIZE];
};

static constexpr size_t WINDOW_SIZE = WINDOW_WIDTH * WINDOW_HEIGHT;  
struct StructStorageBuffer_WindowSwap {
    //static constexpr size_t WIDTH = 800; // 属于类，不属于对象,不占内存
    //static constexpr size_t HEIGHT = 800;
    glm::vec4 pixels[WINDOW_SIZE]; //do not initialize here, to reduce compiling cost。唯一占内存的数据; size: 800*800*16=10,240,000 bytes = ~10 MB
    StructStorageBuffer_WindowSwap() {
        // std::cout<<"StructStorageBuffer_WindowSwap constructor called - Time: " 
        //      << std::chrono::steady_clock::now().time_since_epoch().count() 
        //      << std::endl;
        std::memset(pixels, 0, sizeof(pixels)); 
    }
};


struct StructGraphicsGlobalUniformBuffer {
    alignas(16) glm::mat4 mainCameraView; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraViewInverse; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraProj; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraProjInverse; //16*4=64 bytes
    alignas(16) glm::vec3 mainCameraPos;       // 12 bytes，need 16 bytes alignment
    alignas(4) float tanHalfFovY;          // 4 bytes
    alignas(4) float aspect;               // 4 bytes
    alignas(4) float padding[58];          // 4 × 58 = 232 bytes
    //alignas(4) float padding[14];
    // Sum: 64*4 + 16 + 4 + 4 + 232 = 512 bytes, the size is defined in Config.h

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
};

struct StructComputeGlobalUniformBuffer {
    alignas(16) glm::mat4 mainCameraView; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraViewInverse; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraProj; //16*4=64 bytes
    alignas(16) glm::mat4 mainCameraProjInverse; //16*4=64 bytes
    alignas(16) glm::vec3 mainCameraPos;       // 12 bytes，need 16 bytes alignment
    alignas(4) float tanHalfFovY;          // 4 bytes
    alignas(4) float aspect;               // 4 bytes
    alignas(4) float padding[58];          // 4 × 58 = 232 bytes
    //alignas(4) float padding[14];
    // Sum: 64*4 + 16 + 4 + 4 + 232 = 512 bytes, the size is defined in Config.h

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        return binding;
    }
};

//!In graphicsDescriptor.cpp and rendererCore.cpp, assume Data is 256 bytes. If change size here, need adjust those sourcefiles
struct ObjectTextData{
    alignas(16) glm::mat4 model; //16*4=64 bytes
    alignas(4) bool identityCameraProj; // 4 byte
    alignas(4) bool identityCameraView; // 4 byte
    alignas(4) bool padding_bool[2];       // 8 bytes
    alignas(16) glm::vec4 padding[11];     // 16 × 11 = 176 bytes
    // sum: 64 + 16 + 176 = 256 bytes 
};

#define OBJECT_TEXT_NUM 256
struct StructObjectUniformBuffer { //used in object.h/cpp and gameEngine_initialization.cpp
    //for now, support two groups of Data. Each draw only use one Data matrices. Use offset to access.
    //Each Data is to be aligned to be 256 bytes
    //Support up to 256 objects. buffer size is 256*256(TODO: update to 320) = 65536 bytes; Buffer range is 256 bytes(for each object)
    //65536 bytes = 64 kilo bytes
    ObjectTextData data[OBJECT_TEXT_NUM];
    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
public:
    StructObjectUniformBuffer(){}
    ~StructObjectUniformBuffer(){}
};

struct StructTextUniformBuffer { //used in textManager.h/cpp and gameEngine_initialization.cpp
    ObjectTextData data[OBJECT_TEXT_NUM];
    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
public:
    StructTextUniformBuffer(){}
    ~StructTextUniformBuffer(){}
};

struct ModelPushConstants{
    glm::mat4 model;
};

struct IntPushConstants{
    int value;
};

