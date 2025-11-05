#include "IGame.h"
#include <iostream>
#include "TypeVertex.h"
namespace LuminError{
    class Game : public IGame {
        std::vector<Vertex2D> vertices = {
            { { 0.0f, -0.5f},{ 1.0f, 0.0f, 0.0f }},
            { { 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f }},
            { { -0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f }}		
        };

        void Initialize() override {
            GameEngine->CreateCustomModel2D(vertices);
        }


        void Record() override{
            GameEngine->DrawObject(0, -1, 3); //objectId=0, use default pipeline, draw 3 veritices
            GameEngine->DrawObjects(1, GameEngine->GetObjectSize()-1);
            GameEngine->DrawTexts();
        }
    };
}
#include "Launcher.hpp"