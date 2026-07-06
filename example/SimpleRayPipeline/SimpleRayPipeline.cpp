/* ***********
 * This test draws on the swapchain images, with the use of storage image(Bind0)
 * The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
 * Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
 * Thus no graphics pipeline is needed here
 * Use ray tracing, as is TLAS(bind1)
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"

namespace LuminError{
    class Game : public IGame {
        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) bool cameraInMotion = false;
            alignas(4) unsigned int lightCount = 0;
            alignas(4) unsigned int materialCount = 0;

            static VkDescriptorSetLayoutBinding GetBinding(){
                VkDescriptorSetLayoutBinding binding;
                binding.binding = 0;//not important, will be reset
                binding.descriptorCount = 1;
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR; //VK_SHADER_STAGE_COMPUTE_BIT;
                return binding;
            }
        };
        StructCustomUniformBuffer customUniformBufferObject{};

        void Initialize() override {
            GameEngine->SetRaytracingCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetRaytracingCustomBinding(static_cast<void*>(&binding));
            customUniformBufferObject.lightCount = GameEngine->GetRTLightSize();
            customUniformBufferObject.materialCount = 1; //TODO
        }

        void Update() override {
            customUniformBufferObject.frameCount = GameEngine->GetFrameCount();
            customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
            GameEngine->UploadRaytracingCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->PrintFPS(1.0f);
        }

        void RecordRaytracingCommandBuffer() override{
            GameEngine->RayTrace(800, 800, 1);
        }
    };
}
#include "Launcher.hpp"