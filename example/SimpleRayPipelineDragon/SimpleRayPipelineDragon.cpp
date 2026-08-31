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
        int accumulatedFrameCount = 0;
        bool bLastIsRenderMode = false;

        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) unsigned cameraInMotion = false;
            alignas(4) unsigned int renderMode;      // 0 = Whitted, 1 = Path Tracing, 2 = ReSTIR(未实现), 3 = Bidirectional(未实现)

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
        }

        void Update() override {
            if(GameEngine->GetRaytracingRenderMode()){
                if(bLastIsRenderMode == false) {
                    accumulatedFrameCount = 0;
                    bLastIsRenderMode = true;
                }
                accumulatedFrameCount++;
                customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_render_mode();
            }else{
                customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
                if(GameEngine->GetCameraInMotion()) {
                    accumulatedFrameCount = 0;
                    customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_interactive_move_mode();
                }else {
                    if(bLastIsRenderMode == true) {
                        accumulatedFrameCount = 0;
                        bLastIsRenderMode = false;
                    }
                    accumulatedFrameCount++;
                    customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_interactive_still_mode();
                }
            }
            customUniformBufferObject.frameCount = accumulatedFrameCount;
            GameEngine->UploadRaytracingCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->PrintFPS(1.0f);
        }

        void RecordRaytracingCommandBuffer() override{
            GameEngine->RayTrace(800, 800, 1);
        }
    };
}
#include "Launcher.hpp"