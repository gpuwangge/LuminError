#include "IGame.h"
#include "TypeVertex.h"
#include "TypeUniform.h"
#include "utility.h"

namespace LuminError{
    struct SimplePushConstant : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

        void Initialize() override{
            game->CreateCustomModel3D(vertices3D, indices3D);
        }

        void Record() override{
            double et = game->GetElapseTime();

            ModelPushConstants pushConstants;
            pushConstants.model = glm::rotate(glm::mat4(1.0f), (float)et * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            game->PushConstantToCommand(&pushConstants, 0);//pipeline0

            game->DrawObject(0);
            //game->DrawObjects();
            //game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimplePushConstant)
}