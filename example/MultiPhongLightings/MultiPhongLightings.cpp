/************
 * 
 * *********** */
#include "IGame.h"
#include "Utility.h"
namespace LuminError{
    class Game : public IGame {
        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i < GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i,
				    glm::vec3(0, GameEngine->GetLightPosition(i).y, 0) +
				    glm::vec3(2.5 *cos(et * (i+1)), 0, 2.5 *sin(et * (i+1)))
			    );
                GameEngine->SetObjectPosition(2+i, GameEngine->GetLightPosition(i));
		    }
        }

        void Record() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();    
        }
    };
}
#include "launcher.hpp"