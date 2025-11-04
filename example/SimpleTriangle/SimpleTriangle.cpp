#include "IGame.h"
namespace LuminError{
    class Game : public IGame {
        void Record() override {
            engine->DrawObjects();
            engine->DrawTexts();
        }
    };
}
#include "launcher.hpp"