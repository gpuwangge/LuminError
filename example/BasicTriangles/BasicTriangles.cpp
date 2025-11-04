#include "IGame.h"
#include <iostream>
#include "utility.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "TypeVertex.h"
namespace LuminError{
    struct Game : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

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
            engine->CreateCustomModel3D(vertices3D, indices3D);
            engine->SetGraphicsCustomSize(sizeof(StructCustomUniformBuffer));
            VkDescriptorSetLayoutBinding binding = StructCustomUniformBuffer::GetBinding();
            engine->SetGraphicsCustomBinding(static_cast<void*>(&binding));
        }

        void Update() override {
            double et = engine->GetElapseTime();
            customUniformBufferObject.color = {(sin(et) + 1.0f) / 2.0f, 0.0f, (cos(et) + 1.0f) / 2.0f};
            engine->UploadGraphicsCustomUniformBuffer(engine->GetCurrentFrame(), &customUniformBufferObject, sizeof(StructCustomUniformBuffer));
            engine->SetObjectVelocity(0, 
                0.5 * sin(et * 2), 
                0.5 * sin(et * 2), 
                0.5 * sin(et * 2)
            );
        }

        void Record() override{
            engine->DrawObjects();
            engine->DrawTexts();
        }
    };
}
#include "launcher.hpp"