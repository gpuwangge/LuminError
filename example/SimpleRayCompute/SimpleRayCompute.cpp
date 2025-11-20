/************
 * Implement Ray Tracking using compute pipeline
 * Use SSBO(Shader Storage Buffer Object, or compute storage uniform) as input (and internal output)
 * Use swapchain image as final output
 ************ */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"
#include "TypeUniform.h"
namespace LuminError{
    class Game : public IGame {
        
        StructStorageBuffer_WindowSwap storageBufferObject_WindowSwap{};

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

            //GameEngine->SetComputeStorageBufferSize_WindowSwap(sizeof(StructStorageBuffer_WindowSwap));
            //GameEngine->SetComputeStorageBufferUsage_WindowSwap(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

            GameEngine->SetComputeCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetComputeCustomBinding(static_cast<void*>(&binding));
        }

        void PostInitialize() override{
            GameEngine->UploadComputeStorageBuffer_WindowSwap(GameEngine->GetCurrentFrame(), &storageBufferObject_WindowSwap, sizeof(storageBufferObject_WindowSwap)); //initialize to zeros
        }

        void Update() override {
            double et = GameEngine->GetElapseTime();

            customUniformBufferObject.frameCount = GameEngine->GetFrameCount();
            customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
            //customUniformBufferObject.spherePos0 = glm::vec3(1.1*cos(et * 0.5), 0.2, 1.1*sin(et * 0.5)-1);
            //customUniformBufferObject.spherePos1 = glm::vec3(0.5*sin(0.5+et * 0.75), 1.0, 0.5*cos(et * 0.75)-1);
		    GameEngine->UploadComputeCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->PrintFPS(1.0f);
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(50, 50, 1);
        }
    };
}
#include "Launcher.hpp"