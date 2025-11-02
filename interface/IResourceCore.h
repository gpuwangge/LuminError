#pragma once
#include <iostream>
#include "ILogCore.h"
#include <string>
#include "Config.h"
#include "TypeVertex.h"
#include "TypeText.h"

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

        /**************************
         * Model Resource
         * ***********************/
        virtual void CreateModelCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage = false) = 0;
        virtual void CreateModelCustomModel2D(std::vector<Vertex2D> &vertices2D) = 0;
        virtual void CreateModelTextQuadModel(std::vector<TextQuadVertex> &vertices, std::vector<uint32_t> &indices) = 0;
        virtual void LoadModelObj(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) = 0;

        virtual void* GetModelCustomModel3DData(int index) = 0;
        virtual void* GetModelCustomModel2DData(int index) = 0;
        virtual void* GetModelTextQuadModelData(int index) = 0;
        virtual void* GetModelTextboxImageModelData(int index) = 0;

        virtual int GetModelCustomModel3DSize(int index) = 0;
        virtual int GetModelCustomModel2DSize(int index) = 0;
        virtual int GetModelTextQuadModelSize(int index) = 0;
        virtual int GetModelTextboxImageModelSize(int index) = 0;

        virtual std::vector<glm::vec3> GetModelCustomModel3DLength(int index) = 0;
        virtual std::vector<glm::vec3> GetModelCustomModel2DLength(int index) = 0;
        //virtual glm::vec3 GetModelTextQuadModelLength(int index) = 0;
        virtual std::vector<glm::vec3> GetModelTextboxImageModelLength(int index) = 0;

        virtual std::vector<uint32_t>& GetModelCustomModel3DIndices(int index) = 0;
        virtual std::vector<uint32_t>& GetModelCustomModel2DIndices(int index) = 0;
        virtual std::vector<uint32_t>& GetModelTextQuadModelIndices(int index) = 0;
        virtual std::vector<uint32_t>& GetModelTextboxImageModelIndices(int index) = 0;

        virtual std::vector<glm::vec3>& GetModelLengths() = 0;
        virtual std::vector<glm::vec3>& GetModelLengthsMax() = 0;
        virtual std::vector<glm::vec3>& GetModelLengthsMin() = 0;
        virtual glm::vec3& GetModelLength(int index) = 0;
        virtual glm::vec3& GetModelLengthMax(int index) = 0;
        virtual glm::vec3& GetModelLengthMin(int index) = 0;

    protected:
        LEApplication::IApplication* game;

    }; //class

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
} //namespace
