#include "IGame.h"
#include <iostream>
#include "utility.h"
#include <vulkan/vulkan.h>

namespace LuminError{
    struct SimpleUniformBuffer : public IGame {
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
            game->SetGraphicsCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            game->SetGraphicsCustomBinding(static_cast<void*>(&binding));
        }

        void Update() override {
            double et = game->GetElapseTime();
            customUniformBufferObject.color = glm::vec4((sin(et*3) + 1.0f) / 2.0f, (cos(et*3) + 1.0f) / 2.0f, 0.0f, 1.0f);
            game->UploadGraphicsCustomUniformBuffer(game->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));
        }

        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };

    EXPORT_FACTORY_FOR(SimpleUniformBuffer)
}