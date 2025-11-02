#pragma once
#include "IResourceCore.h"
#include "shaderManager.h"

namespace LEResource{
    class ResourceCore final : public IResourceCore{
    public:
        ResourceCore(){}
        ~ResourceCore(){}
        void SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) override;
        void SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_) override;

        /**************************
         * Shader Resource
         * ***********************/
        //std::vector<VkShaderModule>& GetVertexShaderModules() override;
        //std::vector<VkShaderModule>& GetFragmentShaderModules() override;
        //std::vector<VkShaderModule>& GetComputeShaderModules() override;
        VkShaderModule& GetVertexShaderModule(int index) override;
        VkShaderModule& GetFragmentShaderModule(int index) override;
        VkShaderModule& GetComputeShaderModule(int index) override;
        bool GetShaderEnablePushConstant() override;
        void CreateShaderPushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset, uint32_t size) override;
        VkPushConstantRange& GetShaderPushConstantRange() override;

        bool InitShaderSpirV(const std::string shaderName, VkShaderModule *pShaderModule) override;
        void CreateShader(const std::string shaderName, short shaderType) override;
        void DestroyShaderManager() override;

    private:
        LELog::ILogCore *logger = NULL;
        VkDevice logicalDevice = NULL;
        VkPhysicalDevice physicalDevice = NULL;
        VkQueue graphicsQueue = NULL;

        CShaderManager shaderManager;
    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace