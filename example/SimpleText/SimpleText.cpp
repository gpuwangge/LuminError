#include "IGame.h"
namespace LuminError{
    struct SimpleText : public IGame {
        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleText)
}