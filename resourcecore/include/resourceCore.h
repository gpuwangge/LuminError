#pragma once
#include "IResourceCore.h"
#include "shaderManager.h"
#include "modelManager.h"

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

        /**************************
         * Model Resource
         * ***********************/
        void CreateModelCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage = false) override;
        void CreateModelCustomModel2D(std::vector<Vertex2D> &vertices2D) override;
        void CreateModelTextQuadModel(std::vector<TextQuadVertex> &vertices, std::vector<uint32_t> &indices) override;
        void LoadModelObj(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) override;

        void* GetModelCustomModel3DData(int index) override;
        void* GetModelCustomModel2DData(int index) override;
        void* GetModelTextQuadModelData(int index) override;
        void* GetModelTextboxImageModelData(int index) override;

        int GetModelCustomModel3DSize(int index) override;
        int GetModelCustomModel2DSize(int index) override;
        int GetModelTextQuadModelSize(int index) override;
        int GetModelTextboxImageModelSize(int index) override;

        std::vector<glm::vec3> GetModelCustomModel3DLength(int index) override;
        std::vector<glm::vec3> GetModelCustomModel2DLength(int index) override;
        //glm::vec3 GetModelTextQuadModelLength(int index) override;
        std::vector<glm::vec3> GetModelTextboxImageModelLength(int index) override;

        std::vector<uint32_t>& GetModelCustomModel3DIndices(int index) override;
        std::vector<uint32_t>& GetModelCustomModel2DIndices(int index) override;
        std::vector<uint32_t>& GetModelTextQuadModelIndices(int index) override;
        std::vector<uint32_t>& GetModelTextboxImageModelIndices(int index) override;

        std::vector<glm::vec3>& GetModelLengths() override;
        std::vector<glm::vec3>& GetModelLengthsMax() override;
        std::vector<glm::vec3>& GetModelLengthsMin() override;
        glm::vec3& GetModelLength(int index) override;
        glm::vec3& GetModelLengthMax(int index) override;
        glm::vec3& GetModelLengthMin(int index) override;


    private:
        LELog::ILogCore *logger = NULL;
        VkDevice logicalDevice = NULL;
        VkPhysicalDevice physicalDevice = NULL;
        VkQueue graphicsQueue = NULL;

        CShaderManager shaderManager;
        CModelManager modelManager;
    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace