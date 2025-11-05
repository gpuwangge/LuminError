/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */
#include "IGame.h"
#include "Utility.h"
namespace LuminError{
    class Game : public IGame {
        void Update() override{
            double et = GameEngine->GetElapseTime();
            GameEngine->SetObjectVelocity(0, 0, 3*sin(et * 2), 0); 
            GameEngine->SetObjectAngularVelocity(1, 50, 0, 50);
            GameEngine->SetObjectVelocity(2, 0, 3*sin(et * 4), 0);
        }

        void Record() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();    
        }
    };
}
#include "Launcher.hpp"