#include "shaderManager.h"
#include <fstream>
#include "Config.h"

CShaderManager::CShaderManager(){
    //debugger = new CDebugger("../logs/shaderManager.log");
    //bEnablePushConstant = false;
}
CShaderManager::~CShaderManager(){
    //if (!debugger) delete debugger;
}


void CShaderManager::CreateShader(const std::string shaderName, short shaderType){
    VkShaderModule *pShaderModule;
    switch(shaderType){
        case VERT:{
            VkShaderModule newShaderModule;
            vertShaderModules.push_back(newShaderModule);
            pShaderModule = &vertShaderModules[vertShaderModules.size()-1];
        break;
        }
        case FRAG:{
            VkShaderModule newShaderModule;
            fragShaderModules.push_back(newShaderModule);
            pShaderModule = &fragShaderModules[fragShaderModules.size()-1];
        break;
        }
        case COMP:{
            //pShaderModule = &compShaderModule;
            VkShaderModule newShaderModule;
            compShaderModules.push_back(newShaderModule);
            pShaderModule = &compShaderModules[compShaderModules.size()-1];
        break;
        }
        default:
            throw std::runtime_error("Invalid shader type!");
        break;
    }
    
    bool bopen = InitSpirVShader(SHADER_PATH + shaderName, pShaderModule);
    if(!bopen) bopen = InitSpirVShader("shaders/" + shaderName, pShaderModule);
    if(!bopen) throw std::runtime_error("failed to open shader: "+shaderName);
 
}

bool CShaderManager::InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule){
    std::vector<char> shaderCode;
    bool bOpen = readFile(shaderName.c_str(), shaderCode);
    //std::cout<<shaderName<<std::endl;
    if(!bOpen) return false;

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(m_logicalDevice, &createInfo, PALLOCATOR, pShaderModule);
    //REPORT("vkCreateShaderModule");
    //debugger->writeMSG("Shader Module '%s' successfully loaded\n", shaderName.c_str());

    return true;
}


 bool CShaderManager::readFile(const std::string& filename, std::vector<char> &buffer) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
        //throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    //std::vector<char> buffer(fileSize);
    buffer.resize(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return true;
}

void CShaderManager::Destroy(){
    for(int i = 0; i < vertShaderModules.size(); i++) vkDestroyShaderModule(m_logicalDevice, vertShaderModules[i], nullptr);
    for(int i = 0; i < fragShaderModules.size(); i++) vkDestroyShaderModule(m_logicalDevice, fragShaderModules[i], nullptr);
    for(int i = 0; i < compShaderModules.size(); i++) vkDestroyShaderModule(m_logicalDevice, compShaderModules[i], nullptr);
}