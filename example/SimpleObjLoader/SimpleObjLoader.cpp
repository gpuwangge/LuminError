#include "IGame.h"
namespace LuminError{
    struct SimpleObjLoader : public IGame {
        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleObjLoader)
}