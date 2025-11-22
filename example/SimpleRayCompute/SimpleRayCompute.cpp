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
        //Must define storage buffer object here
        //If define in engine.h, it won't be initialized before engine use it.
        StructStorageBuffer_WindowSwap storageBufferObject_WindowSwap{};
        StructStorageBuffer_Material storageBufferObject_Material{};

        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) bool cameraInMotion = false;
            alignas(4) unsigned int triangleCount = 0;
            alignas(4) unsigned int materialCount = 0;
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

            //0 - 地面 - 粗糙塑料材质
            //storageBufferObject_Material.materials[0].albedo = glm::vec3(0.8, 0.8, 0.8);  // 浅灰色
            storageBufferObject_Material.materials[0].albedo = glm::vec3(0.0, 0.8, 0.8);  //test
            storageBufferObject_Material.materials[0].emissionColor = glm::vec3(0.0);
            storageBufferObject_Material.materials[0].transmissionColor = glm::vec3(1.0);
            storageBufferObject_Material.materials[0].metallic = 0.0;                // 非金属
            storageBufferObject_Material.materials[0].roughness = 0.9;               // 高粗糙度，漫反射为主
            storageBufferObject_Material.materials[0].alpha = 1.0;                   // 不透明
            storageBufferObject_Material.materials[0].emissionStrength = 0.0f;       // 不自发光
            storageBufferObject_Material.materials[0].reflectance = 0.04;            // 塑料的基础反射率
            storageBufferObject_Material.materials[0].specular = 0.5;
            storageBufferObject_Material.materials[0].ior = 1.45;                    // 塑料的折射率
            storageBufferObject_Material.materials[0].transmission = 0.0;            // 不透光

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
            double et = GameEngine->GetElapseTime();

            GameEngine->UploadComputeStorageBuffer_Material(GameEngine->GetCurrentFrame(), &storageBufferObject_Material, sizeof(StructStorageBuffer_Material));

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