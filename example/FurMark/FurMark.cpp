#include "IGame.h"
#include <iostream>
#include "utility.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeVertex.h"

namespace LuminError{
    struct FurMark : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

        struct CustomUniformBufferObject {
            glm::vec2 u_resolution;
	        float u_time;

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
        CustomUniformBufferObject customUBO{};

        void Initialize() override {
            game->CreateCustomModel3D(vertices3D, indices3D);
            game->SetGraphicsCustomSize(sizeof(CustomUniformBufferObject));
            VkDescriptorSetLayoutBinding binding = CustomUniformBufferObject::GetBinding();
            game->SetGraphicsCustomBinding(static_cast<void*>(&binding));
        }

        void Update() override {
            double et = game->GetElapseTime();
            customUBO.u_time = et;
            customUBO.u_resolution = glm::vec2(game->GetWindowWidth(), game->GetWindowHeight());
            game->UploadGraphicsCustomUniformBuffer(game->GetCurrentFrame(), &customUBO, sizeof(CustomUniformBufferObject));
        }

        void Record() override{
            for(int i = 0; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts();
        }
    };

    EXPORT_FACTORY_FOR(FurMark)
}