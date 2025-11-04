#include "IGame.h"
#include <iostream>
#include "utility.h"
#include <vulkan/vulkan.h>
namespace LuminError{
    class Game : public IGame {
        struct StructCustomUniformBuffer {
            glm::vec3 color;

            static VkDescriptorSetLayoutBinding GetBinding(){
                VkDescriptorSetLayoutBinding binding;
                binding.binding = 0;//not important, will be reset
                binding.descriptorCount = 1;
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                return binding;
            }
        };
        StructCustomUniformBuffer customUniformBufferObject{};

        void Initialize() override {
            GameEngine->SetGraphicsCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            GameEngine->SetGraphicsCustomBinding(static_cast<void*>(&binding));
        }

        void Update() override {
            double et = GameEngine->GetElapseTime();
            customUniformBufferObject.color = glm::vec4((sin(et*3) + 1.0f) / 2.0f, (cos(et*3) + 1.0f) / 2.0f, 0.0f, 1.0f);
            GameEngine->UploadGraphicsCustomUniformBuffer(GameEngine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));
        }

        void Record() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();
        }
    };
}
#include "launcher.hpp"