#pragma once
#include <iostream>
#include "ILogCore.h"

namespace LEApplication{
    class IApplication;
}

namespace LEResource{
    class IResourceCore {
    public:
        virtual ~IResourceCore() = default;
        virtual void SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) = 0;
        virtual void SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_) = 0;

        /**************************
         * Shader Resource
         * ***********************/
        // virtual std::vector<VkShaderModule>& GetVertexShaderModules() = 0;
        // virtual std::vector<VkShaderModule>& GetFragmentShaderModules() = 0;
        // virtual std::vector<VkShaderModule>& GetComputeShaderModules() = 0;
        virtual VkShaderModule& GetVertexShaderModule(int index) = 0;
        virtual VkShaderModule& GetFragmentShaderModule(int index) = 0;
        virtual VkShaderModule& GetComputeShaderModule(int index) = 0;
        virtual bool GetShaderEnablePushConstant() = 0;
        virtual void CreateShaderPushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset, uint32_t size) = 0;
        virtual VkPushConstantRange& GetShaderPushConstantRange() = 0;

        virtual bool InitShaderSpirV(const std::string shaderName, VkShaderModule *pShaderModule) = 0;
        virtual void CreateShader(const std::string shaderName, short shaderType) = 0;
        virtual void DestroyShaderManager() = 0;

    protected:
        LEApplication::IApplication* game;

    }; //class

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
} //namespace
