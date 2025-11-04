#include "IGame.h"
namespace LuminError{
    class Game : public IGame {
        void Record() override {
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();
        }
    };
}
#include "launcher.hpp"