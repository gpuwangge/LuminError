#pragma once
#include "utility.h"
#include <vulkan/vulkan.h>

//each line must be aligned to 16 bytes. In shader use vec4 instead of vec3
struct LightAttribute{
    alignas(16) glm::mat4 lightCameraProj;
    alignas(16) glm::mat4 lightCameraView;
    glm::vec4 lightPos;
    glm::vec4 lightDir;
    glm::vec4 lightColor; //RGBA
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float dimmerSwitch;
    float spotInnerAngle;
    float spotOuterAngle;
};

#define LIGHT_MAX 64
struct LightingUniformBufferObject {
    alignas(16) LightAttribute lights[LIGHT_MAX]; //support up to 256 lights: 256*32+16=8208 bytes. Normal uniform buffer size is 65536 bytes(64kb)
    alignas(16) glm::vec4 mainCameraPos;
    alignas(16) int lightNum; //number of lights in use, used to avoid loop

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;//not important, will be reset
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
};