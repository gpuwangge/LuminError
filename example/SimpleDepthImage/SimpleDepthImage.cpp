/************
 * This test draws the depth image on the right side of the screen
 * This test also sets up the light cameras for one of the lights, so the light can be observed from the camera's perspective.
 * To change the observation from depth image to light shadowmap, change one line of code in observe.frag

 * *********** */
#include "IGame.h"
#include "Utility.h"
#include "TypeVertex.h"
#include <iostream>

namespace LuminError{
    struct SimpleDepthImage : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

        void Initialize() override {
            game->CreateCustomModel3D(vertices3D, indices3D);
        }

        void PostInitialize() override {
            game->SetObjectScaleRectangleXY(7, 0, -1, 1, 1);
        }

        void Update() override {
            double et = game->GetElapseTime();
            for(int i = 0; i < game->GetLightSize(); i++) {
                game->SetLightPosition(i,
				    glm::vec3(0, game->GetLightPosition(i).y, 0) +
				    glm::vec3(2.5 *cos(et * (i+1)), 0, 2.5 *sin(et * (i+1)))
			    );
                game->SetObjectPosition(2+i, game->GetLightPosition(i));
		    }
            game->SetLightCameraPosition(0, game->GetLightPosition(0));//set light camera to one of the lightball, the lightball should be not drawn in subpass 0.
        }

        void Record() override{
            int objectCustomSize = game->GetCustomObjectSize();
            int objectSize = game->GetObjectSize();

            for(int i = 0; i < objectCustomSize-1; i++) {
                if(i == 2) continue; //dont draw the light ball in shadowmap. the 5th object is the 3rd lightball. the 2rd object is the 0th lightball
                game->DrawObject(i, 4);
            }

            game->CmdNextSubpass();
            
            for(int i = 0; i < objectCustomSize-1; i++)  game->DrawObject(i);

            game->CmdNextSubpass();
            
            game->DrawObject(objectCustomSize-1);


            for(int i = objectCustomSize; i < objectSize; i++) game->DrawObject(i);
            game->DrawTexts();
        }
    };


    EXPORT_FACTORY_FOR(SimpleDepthImage)
}