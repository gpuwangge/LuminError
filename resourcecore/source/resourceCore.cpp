#include "resourceCore.h"
#include "IApplication.h"

namespace LEResource{
void ResourceCore::SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) {
    game = pApplication;
    logger = logger_;
    //std::cout<<"ResourceCore::SetApplication() Test"<<std::endl;
    //logger->Log("ResourceCore::SetApplication() Test");
}

void ResourceCore::SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_){
    logicalDevice = logicalDevice_;
    physicalDevice = physicalDevice_;
    graphicsQueue = graphicsQueue_;

    shaderManager.m_logicalDevice = logicalDevice_;
}

/**************************
 * Shader Resource
 * ***********************/
// std::vector<VkShaderModule>& ResourceCore::GetVertexShaderModules() { return shaderManager.vertShaderModules; }
// std::vector<VkShaderModule>& ResourceCore::GetFragmentShaderModules() { return shaderManager.fragShaderModules; }
// std::vector<VkShaderModule>& ResourceCore::GetComputeShaderModules() { return shaderManager.compShaderModules; }
VkShaderModule& ResourceCore::GetVertexShaderModule(int index) { return shaderManager.vertShaderModules[index]; }
VkShaderModule& ResourceCore::GetFragmentShaderModule(int index) { return shaderManager.fragShaderModules[index]; }
VkShaderModule& ResourceCore::GetComputeShaderModule(int index) { return shaderManager.compShaderModules[index]; }
void ResourceCore::CreateShaderPushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset, uint32_t size) { shaderManager.CreatePushConstantRange(shaderStageFlagBits, offset, size); }
bool ResourceCore::GetShaderEnablePushConstant() { return shaderManager.bEnablePushConstant; }
VkPushConstantRange& ResourceCore::GetShaderPushConstantRange() { return shaderManager.pushConstantRange; }

bool ResourceCore::InitShaderSpirV(const std::string shaderName, VkShaderModule *pShaderModule) { return shaderManager.InitSpirVShader(shaderName, pShaderModule); }
void ResourceCore::CreateShader(const std::string shaderName, short shaderType) { shaderManager.CreateShader(shaderName, shaderType); }
void ResourceCore::DestroyShaderManager() {shaderManager.Destroy(); }

}//namespace