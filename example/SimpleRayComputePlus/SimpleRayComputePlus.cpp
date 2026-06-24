/************
 * Implement Ray Tracking using compute pipeline
 * Use SSBO(Shader Storage Buffer Object, or compute storage uniform) as input (and internal output)
 * Use swapchain image as final output
 ************ */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"
#include "TypeUniform.h"
//#include <iostream>
namespace LuminError{
    class Game : public IGame {
        //Must define storage buffer object here
        //If define in engine.h, it won't be initialized before engine use it.
        StructStorageBuffer_WindowSwap storageBufferObject_WindowSwap{};

        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) bool cameraInMotion = false;
            alignas(4) unsigned int triangleCount = 0;
            alignas(4) unsigned int materialCount = 0;

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

        void Initialize() override {
            GameEngine->SetComputeCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetComputeCustomBinding(static_cast<void*>(&binding));

            customUniformBufferObject.triangleCount = 5;
            customUniformBufferObject.materialCount = 1;
        }

        void PostInitialize() override{
            //the code works even without this? put it here to be sure
            GameEngine->UploadComputeStorageBuffer_WindowSwap(GameEngine->GetCurrentFrame(), &storageBufferObject_WindowSwap, sizeof(StructStorageBuffer_WindowSwap)); //initialize to zeros
        }
        void Update() override {
            //double et = GameEngine->GetElapseTime();

            //std::cout<<"in motion: "<<(GameEngine->GetCameraInMotion() ? "true" : "false")<<std::endl;
            customUniformBufferObject.frameCount = GameEngine->GetFrameCount();
            customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
		    GameEngine->UploadComputeCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->PrintFPS(1.0f);
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(50, 50, 1);
        }
    };
}
#include "Launcher.hpp"