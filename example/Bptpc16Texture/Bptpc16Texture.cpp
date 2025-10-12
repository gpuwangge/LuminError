#include "IGame.h"
#include "TypeVertex.h"

namespace LuminError{
    struct Bptpc16Texture : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 1.0f, -1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 1.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

        void Initialize() override {
            game->CreateCustomModel3D(vertices3D, indices3D);
        }

        void Record() override{
            for(int i = 0; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts(); 
        }
    };

    EXPORT_FACTORY_FOR(Bptpc16Texture)
}