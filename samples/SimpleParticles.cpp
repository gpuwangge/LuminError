#include "..\\vulkanFramework\\include\\application.h"
#include <random>
#define TEST_CLASS_NAME CSimpleParticles
class TEST_CLASS_NAME: public CApplication{
public:
	static const uint32_t PARTICLE_COUNT = 4096;//8192 will fail on Pixel 7 Pro

	struct CustomUniformBufferObject {
		float deltaTime = 1.0f;

		static VkDescriptorSetLayoutBinding GetBinding(){
			VkDescriptorSetLayoutBinding binding;
			binding.binding = 0;//not important, will be reset
			binding.descriptorCount = 1;
			binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			return binding;
		}
	};
	CustomUniformBufferObject customUniformBufferObject{};

	struct StructStorageBuffer {
		Particle particles[PARTICLE_COUNT];
	};
	StructStorageBuffer storageBufferObject;
	// struct StructStorageBufferOutput {
	// 	Particle particlesOut[PARTICLE_COUNT];
	// };
	//StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = false;
	
	std::vector<VkClearValue> clearValues{ {  0.0f, 0.0f, 0.0f, 1.0f  } };

	void initialize(){
		//appInfo.Object.Count = 1;//One object, that is the particle generator
		//appInfo.VertexBufferType = VertexStructureTypes::ParticleType;
		appInfo.RenderMode = CRenderer::COMPUTE_GRAPHICS;

		appInfo.Uniform.ComputeCustom.Size = sizeof(CustomUniformBufferObject);
		appInfo.Uniform.ComputeCustom.Binding = CustomUniformBufferObject::GetBinding();
		appInfo.Uniform.ComputeStorageBuffer.Size = sizeof(StructStorageBuffer);
		appInfo.Uniform.ComputeStorageBuffer.Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		CApplication::initialize();
		
		std::cout<<"init done."<<std::endl;
		//std::cout<<"renderer.graphicsCmdId="<<renderer.graphicsCmdId<<std::endl;

		//Initial Host data
		//Initialize particles
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
        // Initial particle positions on a circle

		//Need put particles to storageBufferObjectInput
		//Can write a loop, assign the values to storageBufferObjectInput
		//Or, create staging buffer, and call memcpy

        //std::vector<Particle> particles(PARTICLE_COUNT);
        //for (auto& particle : particles) {
		for(int i = 0; i < PARTICLE_COUNT; i++){
            float r = 0.25f * sqrt(rndDist(rndEngine));
            float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
            float x = r * cos(theta) * windowHeight / windowWidth;
            float y = r * sin(theta);
			storageBufferObject.particles[i].position = glm::vec2(x, y);
            storageBufferObject.particles[i].velocity = glm::normalize(glm::vec2(x,y)) * 0.001f;
            storageBufferObject.particles[i].color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
        }
		//VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;
		/*
		storageBufferObjectInput.M = DIM_M;
		storageBufferObjectInput.N = DIM_N;
		storageBufferObjectInput.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObjectInput.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObjectInput.MatB[i] = (float)rand() / (float)RAND_MAX;
		if(bVerbose) PRINT("A: ", storageBufferObjectInput.MatA, DIM_M*DIM_K);
		if(bVerbose) PRINT("B: ", storageBufferObjectInput.MatB, DIM_K*DIM_N);
		if(bVerbose) PRINT("");
		std::cout<<"Initialized A and B."<<std::endl;
		*/

		//Host >> Device
		// Copy initial particle data to all storage buffers
		// Alternative: create particle info in a buffer, and copybuffer to all storage buffers
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
			computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(i, storageBufferObject);
		//descriptors[1].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, durationTime, storageBufferObject);
		
		std::cout<<"Host >> Device done."<<std::endl;
		//std::cout<<"renderer.graphicsCmdId="<<renderer.graphicsCmdId<<std::endl;
	}

	void update(){
		//std::cout<<"renderer.graphicsCmdId="<<renderer.graphicsCmdId<<std::endl;
		//static int counter = 1;
		//if(counter==KernelRunNumber) NeedToExit = true;
		//counter++;
		customUniformBufferObject.deltaTime = deltaTime * 1000;
		//std::cout<<deltaTime<<std::endl;
		//customUniformBufferObject.color = {(sin(durationTime*3) + 1.0f) / 2.0f, (cos(durationTime*3) + 1.0f) / 2.0f, 0.0f, 1.0f};
		computeDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, customUniformBufferObject);

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		//this sample doesn't need Bind DescriptorSets
		//std::cout<<"renderer.graphicsCmdId="<<renderer.graphicsCmdId<<std::endl;
		objects[0].Draw(computeDescriptorManager.storageBuffers, 0, PARTICLE_COUNT);
	}

	void recordComputeCommandBuffer(){
		Dispatch(PARTICLE_COUNT/256,1,1);

		static int counter = 0;
		if(counter % 1000 == 0)std::cout<<"Dispatched Counter: "<<counter<<std::endl;
		counter++;
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		//if(bVerbose) memcpy(storageBufferObjectOutput.MatC, descriptors[1].storageBuffersMapped_2[renderer.currentFrame], sizeof(storageBufferObjectOutput.MatC));
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		//if(bVerbose) PRINT("C: ", storageBufferObjectOutput.MatC, DIM_M*DIM_N);

	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

