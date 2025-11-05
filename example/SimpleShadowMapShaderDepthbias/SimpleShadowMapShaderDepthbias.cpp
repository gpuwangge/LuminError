/************
 * Implementation of Shadow Map
 * Simple scenario: A single light source casting one shadow on a plane
 * Use one renderpass, use two subpasses: one for shadowmap, one for main scene
 * Use software depth bias
 * *********** */
#include "IGame.h"
#include "Utility.h"
#include "TypeVertex.h"
namespace LuminError{
    class Game : public IGame {
        std::vector<Vertex3D> vertices3D = {
            { { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
            { { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }}
        };
        std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};

        void Initialize() override{
            GameEngine->CreateCustomModel3D(vertices3D, indices3D);
        }

        void PostInitialize() override{
            GameEngine->SetObjectScaleRectangleXY(3, 0.5, 0.5, 1, 1);
        }

        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i < GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i,
                    glm::vec3(0, 3.5 + sin(et * (i+1)),0)
                );
                GameEngine->SetObjectPosition(2+i, GameEngine->GetLightPosition(i));
                GameEngine->SetLightCameraPosition(i, GameEngine->GetLightPosition(i));
		    }
        }

        void Record() override{
            int objectCustomSize = GameEngine->GetCustomObjectSize();

            int lightDepthPipeline = 4;

            //first subpass: render shadowmap from light's perspective
            //4 custom objects:
            //0 - table
            //1 - bigball
            //2 - lightball (not render in this pass)
            //3 - depth image rectangle
            for(int i = 0; i < objectCustomSize-1; i++) {
                if(i == 2) continue; //dont draw the light ball in shadowmap. the 5th object is the 3rd lightball. the 2rd object is the 0th lightball
                GameEngine->DrawObject(i, lightDepthPipeline);
            }

            GameEngine->CmdNextSubpass();
             //second subpass: render main scene from camera's perspective
            GameEngine->DrawObjects(0, objectCustomSize-2);
            
            GameEngine->CmdNextSubpass();
            //third subpass: render depth image rectangle
            GameEngine->DrawObject(objectCustomSize-1);

            //render info panels (panesl must be drawn at the last; dont forget to set subpasses_subpass_id = 2 in yaml)
            GameEngine->DrawObjects(objectCustomSize, GameEngine->GetObjectSize()-1);
            GameEngine->DrawTexts(); 
        }
    };
}
#include "Launcher.hpp"