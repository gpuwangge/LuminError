/************
 * 
 * *********** */
#include "IGame.h"
#include "Utility.h"
#include <vulkan/vulkan.h>

namespace LuminError{
    struct SimpleEnvironmentmap : public IGame {
        struct StructCustomUniformBuffer {
            glm::vec3 cameraPos;

            static VkDescriptorSetLayoutBinding GetBinding(){
                VkDescriptorSetLayoutBinding binding;
                binding.binding = 0;//not important, will be reset
                binding.descriptorCount = 1;
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                return binding;
            }
        };
        StructCustomUniformBuffer customUniformBufferObject{};

        void Initialize() override {
            game->SetGraphicsCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            game->SetGraphicsCustomBinding(&binding);
        }

        void Update() override{
            customUniformBufferObject.cameraPos = game->GetMainCameraPosition();
            game->UploadGraphicsCustomUniformBuffer(game->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            game->SetObjectVelocity(7, glm::cross(game->GetObjectPosition(7) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            game->SetObjectVelocity(1, glm::cross(game->GetObjectPosition(1) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            game->SetObjectVelocity(2, glm::cross(game->GetObjectPosition(2) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            game->SetObjectVelocity(3, glm::cross(game->GetObjectPosition(3) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            game->SetObjectVelocity(4, glm::cross(game->GetObjectPosition(4) - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
            game->SetObjectVelocity(5, glm::cross(game->GetObjectPosition(5) - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
        }

        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };


    EXPORT_FACTORY_FOR(SimpleEnvironmentmap)
}