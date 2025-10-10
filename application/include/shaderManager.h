#ifndef H_SHADERMANAGER
#define H_SHADERMANAGER

#include "common.h"
#include "context.h"

class CShaderManager final{
public:
    CShaderManager();
    ~CShaderManager();

    enum ShaderTypes{VERT, FRAG, COMP};


    //VkShaderModule vertShaderModule = VK_NULL_HANDLE;
	//VkShaderModule fragShaderModule = VK_NULL_HANDLE;
    //VkShaderModule compShaderModule = VK_NULL_HANDLE;
    std::vector<VkShaderModule> vertShaderModules;
    std::vector<VkShaderModule> fragShaderModules;
    std::vector<VkShaderModule> compShaderModules;

    //void CreateVertexShader(const std::string shaderName);
    //void CreateFragmentShader(const std::string shaderName);
    //void CreateComputeShader(const std::string shaderName);
    void CreateShader(const std::string shaderName, short shaderType);

    bool bEnablePushConstant = false;
    VkPushConstantRange pushConstantRange;
    template<typename T>
    void CreatePushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset){
        bEnablePushConstant = true;
        pushConstantRange.stageFlags = shaderStageFlagBits;
        pushConstantRange.offset = offset;
        pushConstantRange.size = sizeof(T);
    }

    void Destroy();

#ifndef ANDROID
    bool InitSpirVShader(const std::string shaderName, VkShaderModule *pShaderModule);
#else
    //insert a string after separator: "ab/c","wxj",'/'=>ab/wxjc"
    std::string InsertString(std::string originalString, std::string insertString, char separator);
    VkShaderModule createShaderModule(const std::vector<uint8_t> &code);
#endif
private:
    bool readFile(const std::string& filename, std::vector<char> &buffer);
};

#endif