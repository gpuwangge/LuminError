/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */
#include "IGame.h"
#include "Utility.h"

namespace LuminError{
    struct MultiCubes : public IGame {
        void Update() override{
            double et = game->GetElapseTime();
            game->SetObjectVelocity(0, 0, 3*sin(et * 2), 0); 
            game->SetObjectAngularVelocity(1, 50, 0, 50);
            game->SetObjectVelocity(2, 0, 3*sin(et * 4), 0);
        }

        void Record() override{
            game->DrawObjects();
            game->DrawTexts();    
        }
    };


    EXPORT_FACTORY_FOR(MultiCubes)
}