#include "IGame.h"
namespace LuminError{
    struct SimpleSkybox : public IGame {
        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleSkybox)
}