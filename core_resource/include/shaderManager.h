#pragma once
#include "Foundation.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace LEResource{

class CShaderManager final{
public:
    CShaderManager() {}
    ~CShaderManager() {}

    VkDevice m_logicalDevice;

    std::vector<VkShaderModule> vertShaderModules;
    std::vector<VkShaderModule> fragShaderModules;
    std::vector<VkShaderModule> compShaderModules;
    std::vector<VkShaderModule> raytShaderModules;

    void CreateShader(const std::string shaderName, short shaderType);

    bool bEnablePushConstant = false;
    VkPushConstantRange pushConstantRange;
    // template<typename T>
    // void CreatePushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset){
    //     bEnablePushConstant = true;
    //     pushConstantRange.stageFlags = shaderStageFlagBits;
    //     pushConstantRange.offset = offset;
    //     pushConstantRange.size = sizeof(T);
    // }
    void CreatePushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset, uint32_t size);

    void Destroy();

    bool InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule);

private:
    bool readFile(const std::string& filename, std::vector<char> &buffer);
};

}//namespace
