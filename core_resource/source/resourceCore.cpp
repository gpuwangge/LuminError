#include "resourceCore.h"
#include "IGameEngine.h"
namespace LEResource{

void ResourceCore::SetApplication(LEGameEngine::IGameEngine* pApplication, LELog::ILogCore *logger_) {
    game = pApplication;
    logger = logger_;
}

void ResourceCore::SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_, VkQueue raytracingQueue_) {
    logicalDevice = logicalDevice_;
    physicalDevice = physicalDevice_;
    graphicsQueue = graphicsQueue_;
    raytracingQueue = raytracingQueue_;

    shaderManager.m_logicalDevice = logicalDevice_;

    textImageManager.m_logicalDevice = logicalDevice_;
    textImageManager.m_physicalDevice = physicalDevice_;
    textImageManager.m_graphicsQueue = graphicsQueue_;

    textureManager.m_logicalDevice = logicalDevice_;
    textureManager.m_physicalDevice = physicalDevice_;
    textureManager.m_graphicsQueue = graphicsQueue_;

    glbManager.m_logicalDevice = logicalDevice_;
    glbManager.m_physicalDevice = physicalDevice_;
    glbManager.m_raytracingQueue = raytracingQueue_;
    glbManager.textureManager = &textureManager;

    textureManager.logger = logger;
}

/**************************
 * GLB Resource
 * ***********************/
void ResourceCore::LoadGLB(IN const std::string glbName) {
    glbManager.LoadGLB(glbName);
}
void ResourceCore::LoadMesh(IN int meshIndex, IN int primitiveIndex, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D) {
    glbManager.LoadMesh(meshIndex, primitiveIndex, vertices3D, indices3D);
}

void ResourceCore::LoadTexture(VkCommandPool &commandPool, std::vector<VkSampler> &glbSamplers){
    glbManager.LoadTexture(commandPool, glbSamplers);
}

int ResourceCore::GetMeshSize(IN int glbIndex){
    return glbManager.GetMeshSize(glbIndex);
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
VkShaderModule& ResourceCore::GetRaytracingShaderModule(int index) { return shaderManager.raytShaderModules[index]; }
void ResourceCore::CreateShaderPushConstantRange(VkShaderStageFlagBits shaderStageFlagBits, uint32_t offset, uint32_t size) { shaderManager.CreatePushConstantRange(shaderStageFlagBits, offset, size); }
bool ResourceCore::GetShaderEnablePushConstant() { return shaderManager.bEnablePushConstant; }
VkPushConstantRange& ResourceCore::GetShaderPushConstantRange() { return shaderManager.pushConstantRange; }

bool ResourceCore::InitShaderSpirV(const std::string shaderName, VkShaderModule *pShaderModule) { return shaderManager.InitSpirVShader(shaderName, pShaderModule); }
void ResourceCore::CreateShader(const std::string shaderName, short shaderType) { shaderManager.CreateShader(shaderName, shaderType); }
void ResourceCore::DestroyShaderManager() {shaderManager.Destroy(); }


/**************************
 * Model Resource
 * ***********************/
void ResourceCore::CreateModelCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage){
    modelManager.CreateCustomModel3D(vertices3D, indices3D, isTextboxImage);
}
void ResourceCore::CreateModelCustomModel2D(std::vector<Vertex2D> &vertices2D) { modelManager.CreateCustomModel2D(vertices2D); }
void ResourceCore::CreateModelTextQuadModel(std::vector<TextQuadVertex> &vertices, std::vector<uint32_t> &indices) {
    modelManager.CreateTextQuadModel(vertices, indices);
}
void ResourceCore::LoadModelObj(IN const std::string modelName, OUT std::vector<Vertex3D> &vertices3D, OUT std::vector<uint32_t> &indices3D){
    modelManager.LoadObjModel(modelName, vertices3D, indices3D);
}

void* ResourceCore::GetModelCustomModel3DData(int index) { return modelManager.customModels3D[index].vertices.data(); }
void* ResourceCore::GetModelCustomModel2DData(int index) { return modelManager.customModels2D[index].vertices.data(); }
void* ResourceCore::GetModelTextQuadModelData(int index) { return modelManager.textQuadModels[index].vertices.data(); }
void* ResourceCore::GetModelTextboxImageModelData(int index) { return modelManager.textboxImageModels[index].vertices.data(); }

int ResourceCore::GetModelCustomModel3DSize(int index) { return modelManager.customModels3D[index].vertices.size(); }
int ResourceCore::GetModelCustomModel2DSize(int index) { return modelManager.customModels2D[index].vertices.size(); }
int ResourceCore::GetModelTextQuadModelSize(int index) { return modelManager.textQuadModels[index].vertices.size(); }
int ResourceCore::GetModelTextboxImageModelSize(int index) { return modelManager.textboxImageModels[index].vertices.size(); }

std::vector<glm::vec3> ResourceCore::GetModelCustomModel3DLength(int index) {
    std::vector<glm::vec3> r;
    r.push_back(modelManager.customModels3D[index].length);
    r.push_back(modelManager.customModels3D[index].lengthMin);
    r.push_back(modelManager.customModels3D[index].lengthMax);
    return r;
}
std::vector<glm::vec3> ResourceCore::GetModelCustomModel2DLength(int index) {
    std::vector<glm::vec3> r;
    r.push_back(modelManager.customModels2D[index].length);
    r.push_back(modelManager.customModels2D[index].lengthMin);
    r.push_back(modelManager.customModels2D[index].lengthMax);
    return r;
}
// glm::vec3 ResourceCore::GetModelTextQuadModelLength(int index) {
//     return glm::vec3(modelManager.textQuadModels[index].length, modelManager.textQuadModels[index].lengthMin, modelManager.textQuadModels[index].lengthMax);
// }
std::vector<glm::vec3> ResourceCore::GetModelTextboxImageModelLength(int index) {
    std::vector<glm::vec3> r;
    r.push_back(modelManager.textboxImageModels[index].length);
    r.push_back(modelManager.textboxImageModels[index].lengthMin);
    r.push_back(modelManager.textboxImageModels[index].lengthMax);
    return r;
}

std::vector<uint32_t>& ResourceCore::GetModelCustomModel3DIndices(int index) { return modelManager.customModels3D[index].indices; }
std::vector<uint32_t>& ResourceCore::GetModelCustomModel2DIndices(int index) { return modelManager.customModels2D[index].indices; }
std::vector<uint32_t>& ResourceCore::GetModelTextQuadModelIndices(int index)  { return modelManager.textQuadModels[index].indices; }
std::vector<uint32_t>& ResourceCore::GetModelTextboxImageModelIndices(int index)  { return modelManager.textboxImageModels[index].indices; }

std::vector<glm::vec3>& ResourceCore::GetModelLengths() { return modelManager.modelLengths; }
std::vector<glm::vec3>& ResourceCore::GetModelLengthsMax() { return modelManager.modelLengthsMax; }
std::vector<glm::vec3>& ResourceCore::GetModelLengthsMin()  { return modelManager.modelLengthsMin; }
glm::vec3& ResourceCore::GetModelLength(int index) { return modelManager.modelLengths[index]; }
glm::vec3& ResourceCore::GetModelLengthMax(int index) { return modelManager.modelLengthsMax[index]; }
glm::vec3& ResourceCore::GetModelLengthMin(int index)  { return modelManager.modelLengthsMin[index]; }

/**************************
 * Texture Resource
 * ***********************/
void ResourceCore::CreateNewTextureImageFromFile(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, int layoutType, 
    int miplevel, int sampler_id, VkFormat imageFormat, unsigned short bitPerTexelPerChannel, bool bCubemap){
    //std::cout<<"CreateNewTextureImageFromFile..."<<std::endl;
    //textureManager.CreateTextureImage(texturePath, usage, commandPool, miplevel, sampler_id, imageFormat, bitPerTexelPerChannel, bCubemap);
    int texture_index = textureManager.PushNewTextureImage(commandPool);
    void *texels = NULL;
    textureManager.GetTexelFromFile_SetupTextureImage(texture_index, texturePath, usage, miplevel, imageFormat, bitPerTexelPerChannel, texels);
    //std::cout<< "source texels = " << texels<<std::endl;
    //std::cout<<"Done GetTexelFromFile."<<std::endl;
    if(layoutType == 0)
        textureManager.GenerateTextureImageFromTexel(texture_index, sampler_id, bCubemap, texels, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    else if(layoutType == 1)
        textureManager.GenerateTextureImageFromTexel(texture_index, sampler_id, bCubemap, texels, VK_IMAGE_LAYOUT_GENERAL);
    //std::cout<<"Done GenerateTextureImageFromTexel."<<std::endl;
    //std::cout.flush();
    textureManager.STBI_Free_Image(texels); //when read from file, use stbi so need to free here
    //std::cout<<"Done CreateNewTextureImageFromFile."<<std::endl;
}
void ResourceCore::DestroyTextureManager(){
    textureManager.Destroy();
}
// CTextureImage& ResourceCore::GetTextureImage(int index){
//     return textureManager.textureImages[index];
// }
int ResourceCore::GetTextureImageSize() { return textureManager.textureImages.size(); }
VkImageView ResourceCore::GetTextureImageView(int index) { return textureManager.textureImages[index].m_textureImageBuffer.view; }
std::vector<VkImageView> ResourceCore::GetTextureImageViews() { //这里不要返回引用
    std::vector<VkImageView> views;
    views.reserve(textureManager.textureImages.size());
    for(int i = 0; i < textureManager.textureImages.size(); i++ ){
        views.push_back(textureManager.textureImages[i].m_textureImageBuffer.view);
    }

    return views;
}
int ResourceCore::GetTextureImageSamplerId(int index) { return textureManager.textureImages[index].m_sampler_id; }
void ResourceCore::GenerateMipmaps(int index) { textureManager.textureImages[index].generateMipmaps(); }
void ResourceCore::GenerateMipmaps(int index, std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage) {
    textureManager.textureImages[index].generateMipmaps(rainbowCheckerboardTexturePath, usage);
}

/**************************
 * Textimage Resource
 * ***********************/
void ResourceCore::CreateTextImage(void* texels, int width, int height, VkCommandPool commandPool, int samplerId){
    textImageManager.GenerateTextImageFromTexel(texels, width, height, commandPool, samplerId, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
// CTextureImage& ResourceCore::GetTextImage(int index){
//     return textImageManager.textureImages[index];
// }
void ResourceCore::DestroyTextImageManager(){
    textImageManager.Destroy();
}
int ResourceCore::GetTextImageSize() { return textImageManager.textureImages.size(); }
VkImageView ResourceCore::GetTextImageView(int index) { return textImageManager.textureImages[index].m_textureImageBuffer.view; }
int ResourceCore::GetTextImageSamplerId(int index) { return textImageManager.textureImages[index].m_sampler_id; }

}//namespace