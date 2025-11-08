#include "IGame.h"
#include "TypeVertex.h"
#include "TypeUniform.h"
#include "utility.h"
namespace LuminError{
    class Game : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

        void Initialize() override{
            GameEngine->CreateCustomModel3D(vertices3D, indices3D);
        }

        void Record() override{
            double et = GameEngine->GetElapseTime();

            ModelPushConstants pushConstants;
            pushConstants.model = glm::rotate(glm::mat4(1.0f), (float)et * glm::radians(100.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            GameEngine->PushConstantToCommand(&pushConstants, 0);//pipeline0

            GameEngine->DrawObject(0);
            //game->DrawObjects();
            //game->DrawTexts();
        }
    };
}
#include "Launcher.hpp"