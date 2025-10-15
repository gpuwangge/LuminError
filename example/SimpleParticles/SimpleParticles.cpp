#include "IGame.h"
#include <iostream>
#include "utility.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeVertex.h"
#include "Enum.h"
#include <random>
#include <chrono>
#include "Config.h"

namespace LuminError{
    struct SimpleParticles : public IGame {
        static const uint32_t PARTICLE_COUNT = 4096;//8192 will fail on Pixel 7 Pro

        struct StructCustomUniformBuffer {
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
        StructCustomUniformBuffer customUniformBufferObject{};

        struct StructStorageBuffer {
            Particle particles[PARTICLE_COUNT];
        };
        StructStorageBuffer storageBufferObject;
        // struct StructStorageBufferOutput {
        // 	Particle particlesOut[PARTICLE_COUNT];
        // };
        //StructStorageBufferOutput storageBufferObjectOutput;	

        void Initialize() override {
            game->SetRenderMode(RenderModes::COMPUTE_GRAPHICS);
            game->SetComputeCustomSize(sizeof(StructCustomUniformBuffer));
            //std::cout<<"sizeof(StructCustomUniformBuffer)="<<sizeof(StructCustomUniformBuffer)<<std::endl;
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            game->SetComputeCustomBinding(static_cast<void*>(&binding));
            game->SetComputeStorageBufferSize(sizeof(StructStorageBuffer));
            //std::cout<<"sizeof(StructStorageBuffer)="<<sizeof(StructStorageBuffer)<<std::endl;
            game->SetComputeStorageBufferUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        }

        void PostInitialize() override{
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
                float x = r * cos(theta) * game->GetWindowHeight() / game->GetWindowWidth();
                float y = r * sin(theta);
                storageBufferObject.particles[i].position = glm::vec2(x, y);
                storageBufferObject.particles[i].velocity = glm::normalize(glm::vec2(x,y)) * 0.001f;
                storageBufferObject.particles[i].color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
            }
            //Host >> Device
            // Copy initial particle data to all storage buffers
            // Alternative: create particle info in a buffer, and copybuffer to all storage buffers
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
                //computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(i, storageBufferObject);
                game->UploadComputeStorageBuffer(i, &storageBufferObject, sizeof(StructStorageBuffer));
            //descriptors[1].updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, durationTime, storageBufferObject);
            
            std::cout<<"Host >> Device done."<<std::endl;
            //std::cout<<"renderer.graphicsCmdId="<<renderer.graphicsCmdId<<std::endl;
        }

        void Update() override {
            customUniformBufferObject.deltaTime = game->GetDeltaTime() * 1000;
		    game->UploadComputeCustomUniformBuffer(game->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));
        }

        void Record() override {
            game->DrawParticlesFromStorageBuffer(0, PARTICLE_COUNT);
            for(int i = 1; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts();
        }

        void RecordComputeCommandBuffer() override {
            game->ComputeDispatch(PARTICLE_COUNT/256, 1, 1);

            static int counter = 0;
            if(counter % 1000 == 0)std::cout<<"Dispatched Counter: "<<counter<<std::endl;
            counter++;
        }

        void postUpdate(){
            game->DeviceWaitIdle();
        }
    };

    EXPORT_FACTORY_FOR(SimpleParticles)
}