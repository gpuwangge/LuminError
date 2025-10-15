#include "IGame.h"
namespace LuminError{
    struct SimpleMipmap : public IGame {
        void Record() override{
            game->DrawObjects();
            game->DrawTexts();
        }
    };
    EXPORT_FACTORY_FOR(SimpleMipmap)
}