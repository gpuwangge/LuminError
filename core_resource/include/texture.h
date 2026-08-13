#pragma once
#include "timer.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeImageBuffer.h"
#include <array>
#include "Config.h"
#include "ILogCore.h"

namespace LEResource{

class CTextureImage final{
public:
    /*******************
    *	Texture Image: Basic
    ********************/
    CTextureImage();
    ~CTextureImage();
    void Destroy();

    VkQueue m_graphicsQueue;

    void SetDevice(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkQueue graphicsQueue){
        m_textureImageBuffer.logicalDevice = logicalDevice;
	    m_textureImageBuffer.physicalDevice = physicalDevice;
        m_graphicsQueue = graphicsQueue;
    }

    /*******************
    *	Texture Image: Load
    ********************/
    void GetTexelsFromFile(const std::string texturePath, void *&texel); //, VkImageUsageFlags usage, VkCommandPool &commandPool, unsigned short bitPerTexelPerChannel = 8
    //void GetTexelsFromMemory(void *texel);

    /*******************
    *	Texture Image: Create
    ********************/
    void CreateTextureImage(void* texels);
    void CreateImageView(VkImageAspectFlags aspectFlags);
    
    /*******************
    *	Texture Image: Transition
    ********************/
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    /*******************
    *	Texture Image: Create(Cubemap)
    ********************/
    void CreateTextureImage_cubemap(void* texels);
    void CreateImageView_cubemap(VkImageAspectFlags aspectFlags);

    /*******************
    *	Texture Image: Transition(Cubemap)
    ********************/
    void copyBufferToImage_cubemap(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout_cubemap(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    /*******************
    *	Texture Image: Command Utility
    ********************/
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    /*******************
    *	Texture Image: Mipmap
    ********************/
    void CreateTextureImage_rainbow_mipmap(void* texels, VkImageUsageFlags usage, CWxjImageBuffer &imageBuffer);
    void generateMipmaps(); //create normal mipmap
    void generateMipmaps(std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage); //create mix mipmaps
    void generateMipmapsCore(VkImage image, bool bCreateTempTexture = false, bool bCreateMixTexture = false, std::array<CWxjImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps = NULL);


    /*******************
    *	Resource for Texture
    ********************/
    int32_t m_texWidth, m_texHeight;
    VkFormat m_imageFormat;
    short m_dstTexChannels; //use by stbi_load()
    //std::string texturePath = "";
	//VkSampler textureSampler;
	CWxjImageBuffer m_textureImageBuffer;
	//VkImageView textureImageView;

    VkCommandPool *m_pCommandPool;
    //void* m_pTexels;
    VkImageUsageFlags m_usage;
    int m_texChannels; //8 or 16
    unsigned short m_texBptpc; //bit per texel per channel

    uint32_t m_mipLevels = 1; //1 means no mipmap
    uint32_t m_sampler_id = 0;
    //bool bEnableMipMap = false;
    //bool bEnableCubemap = false; //when object register, need this information to handle view matrix so the ModelMat has not translate
};


class CTextureManager{
public:
    std::vector<CTextureImage> textureImages;
    unsigned int textureImageCount = 0;

    CTextureManager() {}
    ~CTextureManager() {}

    LELog::ILogCore *logger = NULL;
    void SetLogger(LELog::ILogCore *logger_){logger = logger_;}

    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkQueue m_graphicsQueue;

    // void CreateTextureImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, 
    //     int miplevel, int sampler_id, VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB, unsigned short bitPerTexelPerChannel = 8, bool bCubemap = false);
    int PushNewTextureImage(VkCommandPool &commandPool);
    void GetTexelFromFile_SetupTextureImage(int imageIndex, const std::string texturePath, 
        VkImageUsageFlags usage, int miplevel, VkFormat imageFormat, unsigned short bitPerTexelPerChannel, void *&texels);
    void SetupTextureImage(int imageIndex, uint32_t width, uint32_t height, 
        VkImageUsageFlags usage, int miplevel, VkFormat imageFormat, unsigned short bitPerTexelPerChannel);
    void GenerateTextureImageFromTexel(int imageIndex, int sampler_id, bool bCubemap, void *texels);

    void STBI_Free_Image(void *texels);
    void Destroy();
};


class CTextImageManager{
public:
    std::vector<CTextureImage> textureImages;

    //CLogManager logManager;
    //unsigned int textureImageCount = 0;
    CTextImageManager() {}
    ~CTextImageManager() {}
    // void CreateTextureImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, 
    //     int miplevel, int sampler_id, VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB, unsigned short bitPerTexelPerChannel = 8, bool bCubemap = false);

    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkQueue m_graphicsQueue;

    void GenerateTextImageFromTexel(void* texels, int width, int height, VkCommandPool commandPool, int samplerId);

    void Destroy();
};

}//namespace
