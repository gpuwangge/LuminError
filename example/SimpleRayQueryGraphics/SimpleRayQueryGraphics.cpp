/************
 * 
 * *********** */
#include "IGame.h"
#include "Utility.h"
namespace LuminError{
    class Game : public IGame {
        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i <  GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i, glm::vec3(1.5f * cos(et * 1.2), 1.5f * cos(et * 0.85), 1.5f * cos(et * 1)));
                GameEngine->SetObjectPosition(1+i, GameEngine->GetLightPosition(i));
            }
        }

        void Record() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();    
        }
    };
}
#include "Launcher.hpp"