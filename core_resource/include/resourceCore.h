#pragma once
#include "IResourceCore.h"
#include "shaderManager.h"
#include "modelManager.h"
#include "texture.h"
#include "glbManager.h"

namespace LEResource{
    class ResourceCore final : public IResourceCore{
    public:
        ResourceCore(){}
        ~ResourceCore(){}
        void SetApplication(LEGameEngine::IGameEngine* pApplication, LELog::ILogCore *logger_) override;
        void SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_, VkQueue raytracingQueue_) override;

        /**************************
         * GLB Resource
         * ***********************/
        void LoadGLB(IN const std::string glbName) override;
        void LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) override;
        int GetMeshSize(IN int glbIndex) override;

        /**************************
         * Shader Resource
         * ***********************/
        //std::vector<VkShaderModule>& GetVertexShaderModules() override;
        //std::vector<VkShaderModule>& GetFragmentShaderModules() override;
        //std::vector<VkShaderModule>& GetComputeShaderModules() override;
        VkShaderModule& GetVertexShaderModule(int index) override;
        VkShaderModule& GetFragmentShaderModule(int index) override;
        VkShaderModule& GetComputeShaderModule(int index) override;
        VkShaderModule& GetRaytracingShaderModule(int index) override;
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

        /**************************
         * Texture Resource
         * ***********************/
        void CreateNewTextureImageFromFile(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, 
            int miplevel, int sampler_id, VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB, unsigned short bitPerTexelPerChannel = 8, bool bCubemap = false) override;
        void DestroyTextureManager() override;
        //CTextureImage& GetTextureImage(int index) override;
        int GetTextureImageSize() override;
        VkImageView GetTextureImageView(int index) override;
        int GetTextureImageSamplerId(int index) override;
        void GenerateMipmaps(int index) override;
        void GenerateMipmaps(int index, std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage) override;

         /**************************
         * Textimage Resource
         * ***********************/
        void CreateTextImage(void* texels, int width, int height, VkCommandPool commandPool, int samplerId) override;
        //CTextureImage& GetTextImage(int index) override;
        void DestroyTextImageManager() override;
        int GetTextImageSize() override;
        VkImageView GetTextImageView(int index) override;
        int GetTextImageSamplerId(int index) override;

    private:
        LELog::ILogCore *logger = NULL;
        VkDevice logicalDevice = NULL;
        VkPhysicalDevice physicalDevice = NULL;
        VkQueue graphicsQueue = NULL;
        VkQueue raytracingQueue = NULL;

        CShaderManager shaderManager;
        CModelManager modelManager;
        CGLBManager glbManager;

        CTextureManager textureManager; //in texture.h
        CTextImageManager textImageManager; //in texture.h

    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace