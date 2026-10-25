/************
 * Enable ray query in graphics pipeline. 
 * Shoot rays from object to light source to test shadow.
 * *********** */
#include "IGame.h"
#include "Utility.h"
namespace LuminError{
    class Game : public IGame {
        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i <  GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i, 
                    1.0f * cos(3.14159f + et * 0.8), 
                    1.25,//0.5f * cos(et * 1)+1, 
                    1.0f * sin(3.14159f + et * 0.8)
                    );
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