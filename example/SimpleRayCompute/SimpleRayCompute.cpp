/************
 * Implement Ray Tracking using compute pipeline
 * Use SSBO(Shader Storage Buffer Object, or compute storage uniform) as input (and internal output)
 * Use swapchain image as final output
 ************ */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"
#include <cstring>
namespace LuminError{
    class Game : public IGame {
        struct StructStorageBuffer {
            static constexpr size_t WIDTH = 800;
            static constexpr size_t HEIGHT = 800;
            static constexpr size_t SIZE = WIDTH * HEIGHT;
            glm::vec4 data[SIZE]; //do not initialize here, to reduce compiling cost
            StructStorageBuffer() { std::memset(data, 0, sizeof(data)); }
        };
        StructStorageBuffer storageBufferObject{};

        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) bool cameraInMotion = false;
            alignas(4) int padding1 = 0;
            alignas(4) int padding2 = 0;
            //alignas(16) glm::vec3 spherePos0 = glm::vec3(0,0,0);
            //alignas(16) glm::vec3 spherePos1 = glm::vec3(0,0,0);


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

        void PreInitialize() override {
            GameEngine->SetSwapchainImageSize(MAX_FRAMES_IN_FLIGHT);
            GameEngine->EnableComputeSwapChainImage(true);
	    }

        void Initialize() override {
            GameEngine->SetRenderMode(RenderModes::COMPUTE_SWAPCHAIN);

            GameEngine->SetComputeStorageBufferSize(sizeof(StructStorageBuffer));
            GameEngine->SetComputeStorageBufferUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

            GameEngine->SetComputeCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetComputeCustomBinding(static_cast<void*>(&binding));
        }

        void PostInitialize() override{
            GameEngine->UploadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); //initialize to zeros
        }

        void Update() override {
            double et = GameEngine->GetElapseTime();

            //storageBufferObject.data = {1.0f, 2.0f, 3.0f, 4.0f};
            //GameEngine->UploadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject));

            customUniformBufferObject.frameCount = GameEngine->GetFrameCount();
            customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
            //customUniformBufferObject.spherePos0 = glm::vec3(1.1*cos(et * 0.5), 0.2, 1.1*sin(et * 0.5)-1);
            //customUniformBufferObject.spherePos1 = glm::vec3(0.5*sin(0.5+et * 0.75), 1.0, 0.5*cos(et * 0.75)-1);
		    GameEngine->UploadComputeCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(50, 50, 1);
        }
    };
}
#include "Launcher.hpp"