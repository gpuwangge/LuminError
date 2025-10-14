/************
 * This sample is to test render mutiple cubes in a single scene
 * (modified from simpleMSAA, shader to use multiCubes, model to use cube.obj)
 * *********** */
#include "IGame.h"
#include "Utility.h"

namespace LuminError{
    struct MultiPhongLightings : public IGame {
        void Update() override{
            double et = game->GetElapseTime();
            for(int i = 0; i < game->GetLightSize(); i++) {
                game->SetLightPosition(i,
				    glm::vec3(0, game->GetLightPosition(i).y, 0) +
				    glm::vec3(2.5 *cos(et * (i+1)), 0, 2.5 *sin(et * (i+1)))
			);
            game->SetObjectPosition(2+i, game->GetLightPosition(i));
		}
        }

        void Record() override{
            for(int i = 0; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts();    
        }
    };


    EXPORT_FACTORY_FOR(MultiPhongLightings)
}