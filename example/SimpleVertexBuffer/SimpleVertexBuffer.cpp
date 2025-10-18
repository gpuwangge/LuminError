#include "IGame.h"
#include <iostream>
#include "TypeVertex.h"

namespace LuminError{
    struct SimpleVertexBuffer : public IGame {
        std::vector<Vertex2D> vertices = {
            { { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
            { { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
            { { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
        };

        void Initialize() override {
            game->CreateCustomModel2D(vertices);
        }


        void Record() override{
            game->DrawObject(0, -1, 3); //objectId=0, use default pipeline, draw 3 veritices
            game->DrawObjects(1, game->GetObjectSize()-1);
            game->DrawTexts();
        }
    };

    EXPORT_FACTORY_FOR(SimpleVertexBuffer)
}