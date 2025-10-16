/************
 * 
 * *********** */
#include "IGame.h"
#include "Utility.h"

namespace LuminError{
    struct SimplePhongLighting : public IGame {
        void Update() override{
            double et = game->GetElapseTime();
            for(int i = 0; i <  game->GetLightSize(); i++) {
                game->SetLightPosition(i, glm::vec3(1.5f * cos(et * 1.2), 1.5f * cos(et * 0.85), 1.5f * cos(et * 1)));
                game->SetObjectPosition(1+i, game->GetLightPosition(i));
            }
        }

        void Record() override{
            game->DrawObjects();
            game->DrawTexts();    
        }
    };


    EXPORT_FACTORY_FOR(SimplePhongLighting)
}