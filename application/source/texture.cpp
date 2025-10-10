#include "../include/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../thirdParty/stb_image.h"


/*******************
*	Texture Manager: to manage a vector of CTextureImages
********************/
CTextureManager::CTextureManager(){
	//std::cout<<"CTextureManager::CTextureManager()"<<std::endl;
	//textureImages.resize(1);
#ifndef ANDROID
    logManager.setLogFile("textureManager.log");
#endif	
}
CTextureManager::~CTextureManager(){
	//std::cout<<"CTextureManager::~CTextureManager()"<<std::endl;
}

//The main entrance to create texture image
void CTextureManager::CreateTextureImage(const std::string texturePath, VkImageUsageFlags usage, VkCommandPool &commandPool, 
		int miplevel, int sampler_id, VkFormat imageFormat, unsigned short bitPerTexelPerChannel, bool bCubemap){
	//auto startTextureTime = std::chrono::high_resolution_clock::now();
	TimePoint startTimePoint = now();

	CTextureImage textureImage;
	textureImage.m_imageFormat = imageFormat;
	//textureImage.bEnableMipMap = bEnableMipmap; 
	//textureImage.bEnableCubemap = bCubemap;
	textureImage.m_mipLevels = miplevel;
	textureImage.m_usage = usage;
	textureImage.m_pCommandPool = &commandPool;
	assert((bitPerTexelPerChannel == 8) || (bitPerTexelPerChannel == 16)); //bitPerTexelPerChannel is default 8
	textureImage.m_texBptpc = bitPerTexelPerChannel;

	textureImage.GetTexels(texturePath);

	if(!bCubemap){//General texture image
		textureImage.CreateTextureImage(); 
		textureImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
	}else{//Cubemap texture image
		textureImage.CreateTextureImage_cubemap();
		textureImage.CreateImageView_cubemap(VK_IMAGE_ASPECT_COLOR_BIT);
	}

	textureImage.m_sampler_id = sampler_id;

	textureImages.push_back(textureImage);
	//textureImages[0].CreateTextureImage("texture.jpg", usage, renderer.commandPool);
	//textureImages[0].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);

	//auto endTextureTime = std::chrono::high_resolution_clock::now();
    //auto durationTime = std::chrono::duration<float, std::chrono::seconds::period>(endTextureTime - startTextureTime).count()*1000;
	TimePoint endTimePoint = now();
	double durationTime = millisecondsBetween(startTimePoint, endTimePoint);

	logManager.print("Load Texture Image %s", texturePath);
	//logManager.print("\tenable mipmap: %d", textureImage.bEnableMipMap);
	logManager.print("\tenable miplevels: %d", (int)textureImage.m_mipLevels);
	logManager.print("\tuse sampler: %d", (int)textureImage.m_sampler_id);
	logManager.print("\tenable cubemap: %d", bCubemap);
	logManager.print("\tcost %f milliseconds", (float)durationTime);
    //std::cout<<"Load Texture '"<< (*textureNames)[i].first <<"' cost: "<<durationTime<<" milliseconds"<<std::endl;
}

void CTextureManager::Destroy(){
	//std::cout<<"CTextureManager::Destroy()"<<std::endl;
	for(int i = 0; i < textureImages.size(); i++) textureImages[i].Destroy();
}


/*******************
*	Text Manager: to manage a vector of CTextureImages
********************/

CTextImageManager::CTextImageManager(){
	//std::cout<<"CTextureManager::CTextureManager()"<<std::endl;
	//textureImages.resize(1);
// #ifndef ANDROID
//     logManager.setLogFile("textureManager.log");
// #endif	
}
CTextImageManager::~CTextImageManager(){
	//std::cout<<"CTextureManager::~CTextureManager()"<<std::endl;
}
void CTextImageManager::Destroy(){
	//std::cout<<"CTextureManager::Destroy()"<<std::endl;
	for(int i = 0; i < textureImages.size(); i++) textureImages[i].Destroy();
}

/*******************
*	Texture Image: Basic
********************/
CTextureImage::CTextureImage(){
#ifndef ANDROID
    m_imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
#else
	imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
#endif
	m_dstTexChannels = STBI_rgb_alpha;
	//debugger = new CDebugger("../logs/texture.log");
}
CTextureImage::~CTextureImage(){
	//if (!debugger) delete debugger;
}
void CTextureImage::Destroy(){
    m_textureImageBuffer.destroy();
}

/*******************
*	Texture Image: Load
********************/
void CTextureImage::GetTexels(const std::string texturePath) {
	//m_pCommandPool = &commandPool;
	//m_usage = usage;
	//Step 1: prepare staging buffer with texture pixels inside
	int inputTexChannels; //not really useful
	m_texChannels = m_dstTexChannels; //set channel to output channel number

#ifndef ANDROID
	std::string fullTexturePath = TEXTURE_PATH + texturePath;
	for(short i = 0; i < 2; i++){ //look for texture in 2 locations
		if(m_texBptpc == 16){
			m_pTexels = stbi_load_16(fullTexturePath.c_str(), &m_texWidth, &m_texHeight, &inputTexChannels, m_dstTexChannels);
			//std::cout<<"Load 48bpt texture."<<std::endl;
			//std::cout<<"texWidth = "<<texWidth<<", texHeight = "<<texHeight<<std::endl;
		}else{
			m_pTexels = stbi_load(fullTexturePath.c_str(), &m_texWidth, &m_texHeight, &inputTexChannels, m_dstTexChannels);
			//std::cout<<"Load 24bpt texture."<<std::endl;
			//std::cout<<"texWidth = "<<texWidth<<", texHeight = "<<texHeight<<std::endl;
		}if(m_pTexels) break;
		fullTexturePath = "textures/" + texturePath; 
	}
	if (!m_pTexels) throw std::runtime_error("failed to load texture image!");
	//std::cout<<"texWidth: "<<texWidth<<", texHeight: "<<texHeight<<", texChannels: "<<texChannels<<std::endl;
#else
	//TODO: need support bitPerTexelPerChannel == 16
	std::vector<uint8_t> fileBits;
	std::string fullTexturePath = ANDROID_TEXTURE_PATH + texturePath;
	CContext::GetHandle().androidFileManager.AssetReadFile(fullTexturePath.c_str(), fileBits);
	if(bitPerTexelPerChannel == 16)
		texels = stbi_load_16_from_memory(fileBits.data(), fileBits.size(), &texWidth, &texHeight, &texChannels, dstTexChannels);//stbi_uc
	else
		texels = stbi_load_from_memory(fileBits.data(), fileBits.size(), &texWidth, &texHeight, &texChannels, dstTexChannels);//stbi_uc
	
#endif
	PRINT("CreateTextureImage: Load texels as %d bits per texel per channel", m_texBptpc);
	//CreateTextureImage(texels, usage, textureImageBuffer, dstTexChannels, bitPerTexelPerChannel); 
}

/*******************
*	Texture Image: Create
********************/
static unsigned short frac_float16(unsigned short fp16){
	//return fp16;
	// PRINT("Call frac_float16()");

	const int unorm_correction = -16;
 	int lz = (int) std::log2(fp16);

 	char exp = lz + 15 + unorm_correction;
  	int mantissa = ((fp16 << (16 - lz)) - 65536 + 32) >> 6;

	if (exp < 1){ //flush denormal
 		exp = 0;
   		mantissa = 0;
    }

	short value;
	value = (exp << 10) + mantissa;
	return value;
 	
}

void CTextureImage::CreateTextureImage(bool useSTBI) {
	//texWidth/=6;//test
	VkDeviceSize imageSize = m_texWidth * m_texHeight * m_texChannels * m_texBptpc/8; 
// #ifndef ANDROID	
	// std::cout<<"m_texWidth: "<<m_texWidth<<" texels"<<std::endl;
	// std::cout<<"m_texHeight: "<<m_texHeight<<" texels"<<std::endl;
	// std::cout<<"m_texChannels: "<<m_texChannels<<" "<<std::endl;
	// std::cout<<"m_texBptpc: "<<m_texBptpc<<" bit per texel per channel"<<std::endl;
 	// std::cout<<"imageSize: "<<imageSize<<" bytes"<<std::endl;
// #else
// 	LOGI("imageSize: %d bytes", imageSize);
// #endif	
	PRINT("CreateTextureImage: imageSize: %d bytes", (int)imageSize);
	PRINT("CreateTextureImage: texWidth: %d texels", (int)m_texWidth);
	PRINT("CreateTextureImage: texHeight: %d texels", (int)m_texHeight);

	if(m_imageFormat == VK_FORMAT_R16G16B16A16_SFLOAT){
// #ifndef ANDROID	
// 		std::cout<<"imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT"<<std::endl;
// #else
// 		LOGI("imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT");
// #endif	
		PRINT("CreateTextureImage: imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT");		
		int texelNumber = m_texWidth * m_texHeight * m_texChannels;
		for(int i = 0; i < texelNumber; i++) ((uint16_t*)m_pTexels)[i] = frac_float16(((uint16_t*)m_pTexels)[i]);
	}

	//mipLevels = bEnableMipMap ? (static_cast<uint32_t>(std::floor(std::log2(std::max(m_texWidth, m_texHeight)))) + 1) : 1;
	//std::cout<<"m_mipLevels: "<<m_mipLevels<<std::endl;

	CWxjBuffer stagingBuffer;
	VkResult result = stagingBuffer.init(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	stagingBuffer.fill(m_pTexels);

	if(useSTBI) stbi_image_free(m_pTexels);

	//Step 2: create(allocate) image buffer
	//std::cout<<"Creating texture imagebuffer..."<<std::endl;
	m_textureImageBuffer.createImage(m_texWidth, m_texHeight, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, m_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false,
		VK_IMAGE_LAYOUT_UNDEFINED);
	//std::cout<<"Done creating texture imagebuffer."<<std::endl;

	//Step 3: copy stagingBuffer(pixels) to imageBuffer(empty)
	//To perform the copy, need change imageBuffer's layout: undefined->transferDST->shader-read-only 
	//If the image is mipmap enabled, keep the transferDST layout (it will be mipmaped anyway)
	//(transfer image in non-DST layout is not optimal???)
	if(m_mipLevels == 1){
		transitionImageLayout(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
 		copyBufferToImage(stagingBuffer.buffer, m_textureImageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
      	transitionImageLayout(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ///!!!!
	}else{
		transitionImageLayout(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer.buffer, m_textureImageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
	}

	stagingBuffer.DestroyAndFree();
}

void CTextureImage::CreateImageView(VkImageAspectFlags aspectFlags){
    m_textureImageBuffer.createImageView(m_imageFormat, aspectFlags, m_mipLevels, false);
}

/*******************
*	Texture Image: Transition
********************/
void CTextureImage::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
	//barrier.subresourceRange.layerCount = 6; //for cubemap
	//std::cout<<"DEBUG: CTextureImage::transitionImageLayout"<<std::endl;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {///!!!!
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void CTextureImage::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}


/*******************
*	Texture Image: Create(Cubemap)
********************/
void CTextureImage::CreateTextureImage_cubemap() {
	//texWidth/=6;//test
	VkDeviceSize imageSize = m_texWidth * m_texHeight * m_texChannels * m_texBptpc/8; 
// #ifndef ANDROID	
// 	std::cout<<"imageSize: "<<imageSize<<" bytes"<<std::endl;
// #else
// 	LOGI("imageSize: %d bytes", imageSize);
// #endif	
	PRINT("CreateTextureImage: imageSize: %d bytes", (int)imageSize);
	PRINT("CreateTextureImage: texWidth: %d texels", (int)m_texWidth);
	PRINT("CreateTextureImage: texHeight: %d texels", (int)m_texHeight);

	if(m_imageFormat == VK_FORMAT_R16G16B16A16_SFLOAT){
// #ifndef ANDROID	
// 		std::cout<<"imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT"<<std::endl;
// #else
// 		LOGI("imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT");
// #endif	
		PRINT("CreateTextureImage: imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT");		
		int texelNumber = m_texWidth * m_texHeight * m_texChannels;
		for(int i = 0; i < texelNumber; i++) ((uint16_t*)m_pTexels)[i] = frac_float16(((uint16_t*)m_pTexels)[i]);
	}

	//mipLevels = bEnableMipMap ? (static_cast<uint32_t>(std::floor(std::log2(std::max(m_texWidth, m_texHeight)))) + 1) : 1;

	CWxjBuffer stagingBuffer;
	VkResult result = stagingBuffer.init(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	stagingBuffer.fill(m_pTexels);

	stbi_image_free(m_pTexels);

	//Step 2: create(allocate) image buffer
	//std::cout<<"Creating texture image(cubemap)..."<<std::endl;
	m_textureImageBuffer.createImage(m_texWidth, m_texHeight, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, m_usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true,
		VK_IMAGE_LAYOUT_UNDEFINED);

	//Step 3: copy stagingBuffer(pixels) to imageBuffer(empty)
	//To perform the copy, need change imageBuffer's layout: undefined->transferDST->shader-read-only 
	//If the image is mipmap enabled, keep the transferDST layout (it will be mipmaped anyway)
	//(transfer image in non-DST layout is not optimal???)
	if(m_mipLevels == 1){
		transitionImageLayout_cubemap(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
 		copyBufferToImage_cubemap(stagingBuffer.buffer, m_textureImageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
      	transitionImageLayout_cubemap(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ///!!!!
	}else{
		transitionImageLayout_cubemap(m_textureImageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage_cubemap(stagingBuffer.buffer, m_textureImageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
	}

	stagingBuffer.DestroyAndFree();
}

void CTextureImage::CreateImageView_cubemap(VkImageAspectFlags aspectFlags){
    m_textureImageBuffer.createImageView(m_imageFormat, aspectFlags, m_mipLevels, true);
}

/*******************
*	Texture Image: Transition(Cubemap)
********************/
void CTextureImage::transitionImageLayout_cubemap(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    //barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.layerCount = 6; //for cubemap
	//std::cout<<"DEBUG: CTextureImage::transitionImageLayout"<<std::endl;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {///!!!!
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void CTextureImage::copyBufferToImage_cubemap(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy regions[6];
	memset(regions, 0, sizeof(regions));

	/* Horizontal Skybox Format
	* right,left,up,bottom,front,back
	* When changing skybox format, remember to change image enxtend in imageBuffer.cpp as well!
	*/
	// for(int i = 0; i < 6; i++){
	// 	regions[i].bufferOffset = i * (width / 6) * 4;// is the offset in bytes from the start of the buffer object where the image data is copied from or to
	// 	regions[i].bufferRowLength = width; //specify in texels a subregion of a larger two- or three-dimensional image in buffer
	// 	regions[i].bufferImageHeight = height;
	// 	regions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //imageSubresource is a VkImageSubresourceLayers used to specify the specific image subresources of the image used for the source or destination image data.
	// 	regions[i].imageSubresource.mipLevel = 0;
	// 	regions[i].imageSubresource.baseArrayLayer = i;
	// 	regions[i].imageSubresource.layerCount = 1;
	// 	regions[i].imageOffset = { 0, 0, 0 }; //selects the initial x, y, z offsets in texels of the sub-region of the source or destination image data.
	// 	regions[i].imageExtent = { //is the size in texels of the image to copy in width, height and depth.
	// 		width/6,
	// 		height,
	// 		1
	// 	};
	// }


	/* Standard Skybox Format
	*			up
	*	left	front	right	back
	*			down
	*/
	unsigned int extend_width = width / 4;
	unsigned int extend_height = height / 3;
	for(int i = 0; i < 6; i++){
		regions[i].bufferRowLength = width; //specify in texels a subregion of a larger two- or three-dimensional image in buffer
		regions[i].bufferImageHeight = height;
		regions[i].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //imageSubresource is a VkImageSubresourceLayers used to specify the specific image subresources of the image used for the source or destination image data.
		regions[i].imageSubresource.mipLevel = 0;
		regions[i].imageSubresource.baseArrayLayer = i;
		regions[i].imageSubresource.layerCount = 1;
		regions[i].imageOffset = { 0, 0, 0 }; //selects the initial x, y, z offsets in texels of the sub-region of the source or destination image data.
		regions[i].imageExtent = { //is the size in texels of the image to copy in width, height and depth.
			extend_width,
			extend_height,
			1
		};
	}
	int unit_width = extend_width * 4;
	int unit_height = extend_height * 4;
	regions[0].bufferOffset = 1 * unit_height * unit_width + 2 * unit_width; 	//right
	regions[1].bufferOffset = 1 * unit_height * unit_width; 					//left
	regions[2].bufferOffset = 0 * unit_height * unit_width + 1 * unit_width; 	//up
	regions[3].bufferOffset = 2 * unit_height * unit_width + 1 * unit_width; 	//bottom
	regions[4].bufferOffset = 1 * unit_height * unit_width + 1 * unit_width; 	//front
	regions[5].bufferOffset = 1 * unit_height * unit_width + 3 * unit_width; 	//back

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6, regions);

    endSingleTimeCommands(commandBuffer);
}


/*******************
*	Texture Image: Command Utility
********************/
VkCommandBuffer CTextureImage::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = *m_pCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(CContext::GetHandle().GetLogicalDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CTextureImage::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(CContext::GetHandle().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(CContext::GetHandle().GetGraphicsQueue());

    vkFreeCommandBuffers(CContext::GetHandle().GetLogicalDevice(), *m_pCommandPool, 1, &commandBuffer);
}


/*******************
*	Texture Image: Mipmap
********************/
void CTextureImage::generateMipmaps(){
    if(m_mipLevels <= 1) return;
    generateMipmapsCore(m_textureImageBuffer.image);
}

void CTextureImage::generateMipmapsCore(VkImage image, bool bCreateTempTexture, bool bCreateMixTexture, std::array<CWxjImageBuffer, MIPMAP_TEXTURE_COUNT> *textureImageBuffers_mipmaps) {
	//bMix: default is false
	//textureImageBuffers_mipmaps: default is NULL
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(CContext::GetHandle().GetPhysicalDevice(), m_imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = m_texWidth;
	int32_t mipHeight = m_texHeight;

	for (uint32_t i = 1; i < m_mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		if (bCreateMixTexture) {
			int j = i > MIPMAP_TEXTURE_COUNT ? MIPMAP_TEXTURE_COUNT : i;
			vkCmdBlitImage(commandBuffer,
				(*textureImageBuffers_mipmaps)[j-1].image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);
		}else {
			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);
		}

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		if(bCreateTempTexture) barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		else barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = m_mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	if(bCreateTempTexture) barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	else barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void CTextureImage::CreateTextureImage_rainbow_mipmap(void* texels, VkImageUsageFlags usage, CWxjImageBuffer &imageBuffer) {
	VkDeviceSize imageSize = m_texWidth * m_texHeight * m_texChannels * m_texBptpc/8; 

	if(m_imageFormat == VK_FORMAT_R16G16B16A16_SFLOAT){
		PRINT("CreateTextureImage: imageFormat: VK_FORMAT_R16G16B16A16_SFLOAT");		
		int texelNumber = m_texWidth * m_texHeight * m_texChannels;
		for(int i = 0; i < texelNumber; i++) ((uint16_t*)texels)[i] = frac_float16(((uint16_t*)texels)[i]);
	}

	//mipLevels = bEnableMipMap ? (static_cast<uint32_t>(std::floor(std::log2(std::max(m_texWidth, m_texHeight)))) + 1) : 1;

	CWxjBuffer stagingBuffer;
	VkResult result = stagingBuffer.init(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	stagingBuffer.fill(texels);

	stbi_image_free(texels);

	//std::cout<<"Creating texture image(rainbow mipmap)..."<<std::endl;
	imageBuffer.createImage(m_texWidth, m_texHeight, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false,
		VK_IMAGE_LAYOUT_UNDEFINED);

	if(m_mipLevels == 1){
		transitionImageLayout(imageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
 		copyBufferToImage(stagingBuffer.buffer, imageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
      	transitionImageLayout(imageBuffer.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ///!!!!
	}else{
		transitionImageLayout(imageBuffer.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer.buffer, imageBuffer.image, static_cast<uint32_t>(m_texWidth), static_cast<uint32_t>(m_texHeight));
	}

	stagingBuffer.DestroyAndFree();
}

void CTextureImage::generateMipmaps(std::string rainbowCheckerboardTexturePath, VkImageUsageFlags usage){ //rainbow mipmaps case
    if(m_mipLevels <= 1) return;

	std::array<CWxjImageBuffer, MIPMAP_TEXTURE_COUNT> tmpTextureBufferForRainbowMipmaps;//create temp mipmaps
	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {//fill temp mipmaps
		int texChannels;
#ifndef ANDROID
		std::string fullTexturePath = TEXTURE_PATH + rainbowCheckerboardTexturePath + std::to_string(i) + ".png";
		void *texels = stbi_load(fullTexturePath.c_str(), &m_texWidth, &m_texHeight, &texChannels, m_dstTexChannels);
#else
		//TODO: need support 16 bptpc
		std::vector<uint8_t> fileBits;
		std::string fullTexturePath = ANDROID_TEXTURE_PATH + rainbowCheckerboardTexturePath + std::to_string(i) + ".png";
		CContext::GetHandle().androidFileManager.AssetReadFile(fullTexturePath.c_str(), fileBits);
		uint8_t* texels = stbi_load_from_memory(fileBits.data(), fileBits.size(), &texWidth, &texHeight, &texChannels, 4);
#endif
		//CreateTextureImage_rainbow_mipmap(texels, usage, tmpTextureBufferForRainbowMipmaps[i], m_dstTexChannels, 8);
		CreateTextureImage_rainbow_mipmap(texels, usage, tmpTextureBufferForRainbowMipmaps[i]);
        generateMipmapsCore(tmpTextureBufferForRainbowMipmaps[i].image, true);
	}
	//Generate mipmaps for image, using tmpTextureBufferForRainbowMipmaps
	generateMipmapsCore(m_textureImageBuffer.image, false, true, &tmpTextureBufferForRainbowMipmaps);
	//Clean up
	for (int i = 0; i < MIPMAP_TEXTURE_COUNT; i++) {
        tmpTextureBufferForRainbowMipmaps[i].destroy();
	}
}

