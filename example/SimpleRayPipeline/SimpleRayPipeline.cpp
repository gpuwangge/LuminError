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

        struct StructCustomUniformBuffer {
            alignas(4) int frameCount = 0;
            alignas(4) unsigned cameraInMotion = false;
            alignas(4) unsigned int lightCount = 0;
            alignas(4) unsigned int materialCount = 0;

            alignas(4) unsigned int renderMode;      // 0 = Whitted, 1 = Path Tracing, 2 = ReSTIR(未实现), 3 = Bidirectional(未实现)
            alignas(4) unsigned int spp;             // Samples Per Pixel
            alignas(4) unsigned int maxBounce;       // 最大反弹次数
            alignas(4) unsigned int accumulate;      // 0 = 不积累, 1 = 帧间积累
            alignas(4) unsigned int randomSeed;      // 可选，每次运行不同

            alignas(4) float rrProbability;   //RR（俄罗斯轮盘）
            alignas(4) unsigned enableNEE;
            alignas(4) unsigned useSky;
            alignas(4) float maxRadiance;
            alignas(4) unsigned int debugMode;

            alignas(4) unsigned int softShadowEnable; //for whitted style only
            alignas(4) unsigned int softShadowSampleNumber; //for whitted style only

            alignas(4) unsigned int maxReflectionDepth;
            alignas(4) unsigned int maxRefractionDepth;

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
            customUniformBufferObject.materialCount = GameEngine->GetMaterialSize(); //目前只能读出0，暂时不用这个东西
        
            customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_render_mode();
            customUniformBufferObject.spp = GameEngine->Get_feature_raytracing_pipeline_sampler_per_pixel();
            customUniformBufferObject.maxBounce = GameEngine->Get_feature_raytracing_pipeline_maximum_bounce();
            customUniformBufferObject.accumulate = GameEngine->Get_feature_raytracing_pipeline_accumulate();
            customUniformBufferObject.enableNEE = GameEngine->Get_feature_raytracing_pipeline_enableNEE();
            customUniformBufferObject.useSky = GameEngine->Get_feature_raytracing_pipeline_use_sky();
            customUniformBufferObject.maxRadiance = GameEngine->Get_feature_raytracing_pipeline_maximum_Radiance();
            customUniformBufferObject.debugMode = GameEngine->Get_feature_raytracing_pipeline_debug_mode();
            customUniformBufferObject.softShadowEnable = GameEngine->Get_feature_raytracing_pipeline_softShadowEnable();
            customUniformBufferObject.softShadowSampleNumber = GameEngine->Get_feature_raytracing_pipeline_softShadowSampleNumber();
            customUniformBufferObject.maxReflectionDepth = GameEngine->Get_feature_raytracing_pipeline_maxReflectionDepth();
            customUniformBufferObject.maxRefractionDepth = GameEngine->Get_feature_raytracing_pipeline_maxRefractionDepth();
        }

        void Update() override {
            customUniformBufferObject.cameraInMotion = GameEngine->GetCameraInMotion();
            if(GameEngine->GetCameraInMotion()) {
                accumulatedFrameCount = 0;
                customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_interactive_render_mode();
            }else {
                accumulatedFrameCount++;
                customUniformBufferObject.renderMode = GameEngine->Get_feature_raytracing_pipeline_render_mode();
            }

            customUniformBufferObject.frameCount = accumulatedFrameCount;
            //customUniformBufferObject.renderMode = 1;
            
            GameEngine->UploadRaytracingCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->PrintFPS(1.0f);
        }

        void RecordRaytracingCommandBuffer() override{
            GameEngine->RayTrace(800, 800, 1);
        }
    };
}
#include "Launcher.hpp"