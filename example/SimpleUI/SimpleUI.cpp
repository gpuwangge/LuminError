/************
 * This sample is to test render 2D UI and 3D Object in a single scene
 * *********** */

#include "IGame.h"
#include "TypeVertex.h"

namespace LuminError{
    struct SimpleUI : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};
        
        void Initialize() override {
            game->CreateCustomModel3D(vertices3D, indices3D);
        }

        void PostInitialize() override {
            game->SetObjectScaleRectangleXY(0, -1, -1, 0, 0);
            game->SetObjectScaleRectangleXY(3, 0.5, 0.5, 1, 1);
        }

        void Update() override {
            double et = game->GetElapseTime();
            game->SetObjectAngularVelocity(1, 50, 0, 50);
            game->SetObjectVelocity(2, glm::vec3(0, 3*sin(et * 2), 0));
        }

        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleUI)
}