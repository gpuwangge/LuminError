#include "IGame.h"
namespace LuminError{
    struct SimpleMSAA : public IGame {
        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleMSAA)
}