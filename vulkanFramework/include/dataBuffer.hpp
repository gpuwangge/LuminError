#ifndef H_DATABUFFER
#define H_DATABUFFER
#include "common.h"
#include "context.h"
#include <map>


//Uniform Naming Rule: PipelineType_DescriptorType_Purpose
enum UniformTypes {
    GRAPHCIS_UNIFORMBUFFER_CUSTOM =                 0x00000001, 
    GRAPHCIS_UNIFORMBUFFER_LIGHTING =               0x00000002,
    GRAPHCIS_UNIFORMBUFFER_MVP =                    0x00000004, //assume app uses one: MVP or VP
    GRAPHCIS_UNIFORMBUFFER_TEXT_MVP =               0x00000008,
    GRAPHCIS_UNIFORMBUFFER_VP =                     0x00000010,
    GRAPHCIS_COMBINEDIMAGESAMPLER_TEXTUREIMAGE =    0x00000020,
    GRAPHCIS_COMBINEDIMAGESAMPLER_DEPTHIMAGE =      0x00000040,  //for main camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE = 0x00000080,  //for light camera
    GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS = 0x00000100,  //for light camera(Hardware depth bias, use two renderpass, dynamic depth bias)
    //GRAPHCIS_COMBINEDIMAGESAMPLER_LIGHTDEPTHIMAGE_HARDWAREDEPTHBIAS2 = 0x00000100,

    COMPUTE_UNIFORMBUFFER_CUSTOM =   0x00000200,
    COMPUTE_STORAGEBUFFER_DOUBLE =   0x00000400,
    COMPUTE_STORAGEIMAGE_TEXTURE =   0x00000800,
    COMPUTE_STORAGEIMAGE_SWAPCHAIN = 0x00001000
};

// enum RenderPassTypes {
//     MAINSCENE = 0,
//     MAINSCENE2,
//     SHADOWMAP
// };

//each line must be aligned to 16 bytes. In shader use vec4 instead of vec3
struct LightAttribute{
    alignas(16) glm::mat4 lightCameraProj;
    alignas(16) glm::mat4 lightCameraView;
    glm::vec4 lightPos;
    glm::vec4 lightDir;
    glm::vec4 lightColor; //RGBA
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;
    float dimmerSwitch;
    float spotInnerAngle;
    float spotOuterAngle;
};

//this class to create graphics descript layer(static), and non-texture descripter set(static), create sampler(static)
//texture descripter leave to CObject
//also provide function to update non-texture uniform(static) (TODO)


#define LIGHT_MAX 64
struct LightingUniformBufferObject {
    alignas(16) LightAttribute lights[LIGHT_MAX]; //support up to 256 lights: 256*32+16=8208 bytes. Normal uniform buffer size is 65536 bytes(64kb)
    alignas(16) glm::vec4 mainCameraPos;
    alignas(16) int lightNum; //number of lights in use, used to avoid loop

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;//not important, will be reset
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
};

struct TextureAttributeInfo{
    std::string name;
    int miplevel;
    int samplerid;
    bool enableCubemap;
};

enum VertexStructureTypes{
    NoType,
    TwoDimension,
    ThreeDimension,
    ParticleType,
    TextQuad
};


struct Vertex3D {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; 
		bindingDescription.stride = sizeof(Vertex3D);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;  //binding is in use in vertex shader(default is using 0)
		attributeDescriptions[0].location = 0; //location is in use in vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex3D, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex3D, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex3D, texCoord);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex3D, normal);

		return attributeDescriptions;
	}

	bool operator==(const Vertex3D& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
	}
};
// namespace std {
// 	template<> struct hash<Vertex3D> { 
// 		size_t operator()(Vertex3D const& vertex) const {
// 			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
// 		}
// 	};
// }

struct TextQuadVertex {
    glm::vec2 pos;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; 
		bindingDescription.stride = sizeof(TextQuadVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TextQuadVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TextQuadVertex, uv);
		return attributeDescriptions;
	}

	bool operator==(const TextQuadVertex& other) const {
		return pos == other.pos && uv == other.uv;
	}    
};

struct TextInstanceData{
    glm::vec2 offset;
    glm::vec3 color;
    glm::vec4 uvRect; //uv range of a specific char in  atlas
    glm::vec2 scale;

    static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1; 
		bindingDescription.stride = sizeof(TextInstanceData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 1; //here use 1 instead of 0, because binding0 is for per vertex data; binding1 is for per instance data
		attributeDescriptions[0].location = 2;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(TextInstanceData, offset);

		attributeDescriptions[1].binding = 1;
		attributeDescriptions[1].location = 3;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(TextInstanceData, color);

		attributeDescriptions[2].binding = 1;
		attributeDescriptions[2].location = 4;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(TextInstanceData, uvRect);

        attributeDescriptions[3].binding = 1;
		attributeDescriptions[3].location = 5;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(TextInstanceData, scale);
		return attributeDescriptions;
	}

	bool operator==(const TextInstanceData& other) const {
		return offset == other.offset && color == other.color && uvRect == other.uvRect && scale == other.scale;
	}    
};

namespace std {
	//Custom hash function operator() for Vertex3D
	//operator(): input is Vertex3D type hash key, output is std::size_t hash value
	//There are different ways to implement this, choose a method for high quality hash function
	void inline custom_hash_combine(size_t &seed, size_t hash){
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}
	template<> struct hash<Vertex3D> { 
		size_t operator()(Vertex3D const& vertex) const {
            //code from vulkan tutorial, removed this because of android compitability issue
			//return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
            
			//return 0; 
			//explain: this will disable hash algorithm. code still works, but not optimized

			size_t seed = 0;
			custom_hash_combine(seed, hash<float>()(vertex.pos.x));
			custom_hash_combine(seed, hash<float>()(vertex.pos.y));
			custom_hash_combine(seed, hash<float>()(vertex.pos.z));
			custom_hash_combine(seed, hash<float>()(vertex.color.x));
			custom_hash_combine(seed, hash<float>()(vertex.color.y));
			custom_hash_combine(seed, hash<float>()(vertex.color.z));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.x));
			custom_hash_combine(seed, hash<float>()(vertex.texCoord.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.x));
			custom_hash_combine(seed, hash<float>()(vertex.normal.y));
			custom_hash_combine(seed, hash<float>()(vertex.normal.z));
			return seed;
		}
	};
}

struct Vertex2D {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2D, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2D, color);

        return attributeDescriptions;
    }

    bool operator==(const Vertex2D& other) const {
        return pos == other.pos && color == other.color;
    }
};

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
};

class CWxjBuffer final{
public:
    CWxjBuffer(): m_size(0){}
    ~CWxjBuffer(){}

    VkResult init(IN VkDeviceSize requiredSize, VkBufferUsageFlags usage) {
        //HERE_I_AM("Init05DataBuffer");
        //Step1:Create Buffer(create buffer)
        VkResult result = VK_SUCCESS;

        VkBufferCreateInfo  vbci;
        vbci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vbci.pNext = nullptr;
        vbci.flags = 0;
        vbci.size = requiredSize;
        vbci.usage = usage;
        vbci.queueFamilyIndexCount = 0;
        vbci.pQueueFamilyIndices = (const uint32_t *)nullptr;
        vbci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	// can only use CONCURRENT if .queueFamilyIndexCount > 0
        result = vkCreateBuffer(CContext::GetHandle().GetLogicalDevice(), IN &vbci, PALLOCATOR, OUT &buffer);
        //REPORT("vkCreateBuffer");

        //Step 2:allocate memory(create deviceMemory in gpu)
        VkMemoryRequirements			vmr;
        vkGetBufferMemoryRequirements(CContext::GetHandle().GetLogicalDevice(), IN buffer, OUT &vmr);		// fills vmr
        //if (Verbose){
        //fprintf(debugger->FpDebug, "Buffer vmr.size = %lld\n", vmr.size);
        //fprintf(debugger->FpDebug, "Buffer vmr.alignment = %lld\n", vmr.alignment);
        //fprintf(debugger->FpDebug, "Buffer vmr.memoryTypeBits = 0x%08x\n", vmr.memoryTypeBits);
        //fflush(debugger->FpDebug);
        //}
         m_size = vmr.size;//vmr.size is different than the input requiredSize, because of alignment reason, vmr.size can be larger

        VkMemoryAllocateInfo			vmai;
        vmai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vmai.pNext = nullptr;
        vmai.allocationSize = vmr.size; 
        vmai.memoryTypeIndex = FindMemoryThatIsHostVisible(vmr.memoryTypeBits);
        //VkDeviceMemory				vdm;
        result = vkAllocateMemory(CContext::GetHandle().GetLogicalDevice(), IN &vmai, PALLOCATOR, OUT &deviceMemory);
       
        //REPORT("vkAllocateMemory");
        //deviceMemory = vdm;

        //Step 3: bind memory(bind buffer and deviceMemory)
        result = vkBindBufferMemory(CContext::GetHandle().GetLogicalDevice(), buffer, IN deviceMemory, 0);		// 0 is the offset
        //REPORT("vkBindBufferMemory");

        return result;
    }

    VkResult fill(IN void * data) {
        //Step 4:copy memory(copy data into deviceMemory)
        void * pGpuMemory;
        vkMapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory, 0, VK_WHOLE_SIZE, 0, &pGpuMemory);	// 0 and 0 are offset and flags
        memcpy(pGpuMemory, data, (size_t)m_size);
        vkUnmapMemory(CContext::GetHandle().GetLogicalDevice(), IN deviceMemory);
        return VK_SUCCESS;
    }

    void DestroyAndFree(){
        if(m_size != 0){
            vkDestroyBuffer(CContext::GetHandle().GetLogicalDevice(), buffer, nullptr);
            vkFreeMemory(CContext::GetHandle().GetLogicalDevice(), deviceMemory, nullptr);
        }
    }

    VkBuffer		buffer;
    VkDeviceMemory		deviceMemory;

private:
	VkDeviceSize		m_size;

    int FindMemoryByFlagAndType(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits) {
        VkPhysicalDeviceMemoryProperties	vpdmp;
        vkGetPhysicalDeviceMemoryProperties(CContext::GetHandle().GetPhysicalDevice(), OUT &vpdmp);//instance->pickedPhysicalDevice->get()->getHandle()
        for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++) {
            VkMemoryType vmt = vpdmp.memoryTypes[i];
            VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
            if ((memoryTypeBits & (1 << i)) != 0) {
                if (((vmpf & memoryFlagBits) && (vmpf & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0){
                    //fprintf(debugger->FpDebug, "Found given memory flag (0x%08x) and type (0x%08x): i = %d\n", memoryFlagBits, memoryTypeBits, i);
                    return i;
                }
            }
        }

        //fprintf(debugger->FpDebug, "Could not find given memory flag (0x%08x) and type (0x%08x)\n", memoryFlagBits, memoryTypeBits);
        throw  std::runtime_error("Could not find given memory flag and type");
    }

    int FindMemoryThatIsHostVisible(uint32_t memoryTypeBits) {
        return FindMemoryByFlagAndType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memoryTypeBits);
    }


};

struct MVPData{
    alignas(16) glm::mat4 model; //16*4=64 bytes
	alignas(16) glm::mat4 mainCameraProj; //16*4=64 bytes
    //alignas(16) glm::mat4 lightCameraProj; //16*4=64 bytes, TODO: alignment?
    alignas(16) glm::mat4 mainCameraView; //16*4=64 bytes
    //alignas(16) glm::mat4 padding; //: MVP size is 192 bytes, but require a multiple of device limit minUniformBufferOffsetAlignment 256.
    //alignas(16) glm::mat4 lightCameraView;
    //Each element of pDynamicOffsets which corresponds to a descriptor binding with type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC must be a multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment
    alignas(16) glm::mat4 padding0; 
    alignas(16) glm::mat4 padding1;
    alignas(16) glm::mat4 padding2;
    alignas(16) glm::mat4 padding3;
    alignas(16) glm::mat4 padding4;
};  

#define MVP_NUM 256
struct MVPUniformBufferObject {
	//MVPData *mvpData; //dynamic doesn't work

    //for now, support two groups of mvpData. Each draw only use one mvpData matrices. Use offset to access.
    //Each mvpData is to be aligned to be 256 bytes
    //Support up to 256 (MVP) objects. buffer size is 256*256(TODO: update to 320) = 65536 bytes; Buffer range is 256 bytes(for each object)
    //65536 bytes = 64 kilo bytes
    MVPData mvpData[MVP_NUM];

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }

public:
    MVPUniformBufferObject(){}

    void init(int mvpCount){
       // mvpData = new MVPData[mvpCount];
        //std::cout<<"Created mvpData, mvpCount = "<<mvpCount<<std::endl;
    }

    ~MVPUniformBufferObject(){
        //if(mvpData) delete mvpData;
    }
};

struct TextMVPUniformBufferObject {
    MVPData mvpData[MVP_NUM];
    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return binding;
    }
public:
    TextMVPUniformBufferObject(){}
    ~TextMVPUniformBufferObject(){}
};

struct VPUniformBufferObject {
	//alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;

    static VkDescriptorSetLayoutBinding GetBinding(){
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        return binding;
    }
};

struct ModelPushConstants{
    glm::mat4 model;
};

struct IntPushConstants{
    int value;
};

// Struct to hold glyph texture data
struct GlyphTexture {
    //glm::vec2 pos;
    //glm::vec2 advance_size; //large
    //glm::vec2 real_size; //small
    glm::vec2 size;
    //SDL_Rect texelRect; // The position and size of the glyph within the texture atlas
    glm::vec4 uvRect; // normalized UV coordinates
    //int bearingX;      // Left bearing
    //int bearingY;      // Top bearing
    int advance;   // The horizontal distance to the next glyph
};


#endif