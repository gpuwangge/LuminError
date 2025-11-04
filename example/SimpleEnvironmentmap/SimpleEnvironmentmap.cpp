/************
 * 
 * *********** */
#include "IGame.h"
#include "Utility.h"
#include <vulkan/vulkan.h>
namespace LuminError{
    class Game : public IGame {
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
            GameEngine->SetGraphicsCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetGraphicsCustomBinding(&binding);
        }

        void Update() override{
            customUniformBufferObject.cameraPos = GameEngine->GetMainCameraPosition();
            GameEngine->UploadGraphicsCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));

            GameEngine->SetObjectVelocity(7, glm::cross(GameEngine->GetObjectPosition(7) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            GameEngine->SetObjectVelocity(1, glm::cross(GameEngine->GetObjectPosition(1) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            GameEngine->SetObjectVelocity(2, glm::cross(GameEngine->GetObjectPosition(2) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            GameEngine->SetObjectVelocity(3, glm::cross(GameEngine->GetObjectPosition(3) - glm::vec3(0,0,0), glm::vec3(0,0,1)));
            GameEngine->SetObjectVelocity(4, glm::cross(GameEngine->GetObjectPosition(4) - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
            GameEngine->SetObjectVelocity(5, glm::cross(GameEngine->GetObjectPosition(5) - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
        }

        void Record() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();
        }
    };
}
#include "launcher.hpp"