/************
 * Implementation of Shadow Map
 * Simple scenario: multiple lights casting multiple shadows on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 * Use push constant to pass shadowmap renderpass index to vertex shader
 * *********** */
#include "IGame.h"
#include "Utility.h"
#include "Enum.h"
#include "TypeUniform.h"
namespace LuminError{
    class Game : public IGame {
        void Initialize() override{
            GameEngine->SetRenderMode(RenderModes::GRAPHICS_SHADOWMAP);
        }

        void PostInitialize() override{
            GameEngine->SetMainCameraSensitivity(20.0f);

            for(int i = 0; i < 8; i++) GameEngine->SetLightCameraFocusObjectId(i, 1);
            for(int i = 8; i < 16; i++) GameEngine->SetLightCameraFocusObjectId(i, 2);
            //for(int i = 16; i < 24; i++) game->SetLightCameraFocusObjectId(i, 3);
            //for(int i = 24; i < 32; i++) game->SetLightCameraFocusObjectId(i, 4);
        }

        void Update() override{
            double et = GameEngine->GetElapseTime();

            glm::vec3 pos1 = GameEngine->GetObjectPosition(1);  //big spheres
            glm::vec3 pos2 = GameEngine->GetObjectPosition(2);
            glm::vec3 pos3 = GameEngine->GetObjectPosition(3);
            glm::vec3 pos4 = GameEngine->GetObjectPosition(4);

            for(int i = 0; i < GameEngine->GetLightSize(); i++){
                if(i < 8){
                    GameEngine->SetLightPosition(i,
                        glm::vec3(pos1.x + 2.5 *cos(et * (i+1) * 0.25), GameEngine->GetLightPosition(i).y,  pos1.z + 2.5 *sin(et * (i+1) * 0.25)));
                }else if(i < 16){
                    GameEngine->SetLightPosition(i,
                        glm::vec3(pos2.x + 2.5 *cos(et * (i+1-8) * 0.25), GameEngine->GetLightPosition(i).y,  pos2.z + 2.5 *sin(et * (i+1-8) * 0.25)));
                }
                // else if (i < 24){
                //     lights[i].SetLightPosition(glm::vec3(pos3.x + 2.5 *cos(et * (i+1-16) * 0.25), lights[i].GetLightPosition().y,  pos3.z + 2.5 *sin(et * (i+1-16) * 0.25)));
                // }else if (i < 32){
                //     lights[i].SetLightPosition(glm::vec3(pos4.x + 2.5 *cos(et * (i+1-24) * 0.25), lights[i].GetLightPosition().y,  pos4.z + 2.5 *sin(et * (i+1-24) * 0.25)));
                // }
                GameEngine->SetLightCameraPosition(i, GameEngine->GetLightPosition(i));
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) override{
            int shadowmapPipelineIndex = 2;

            IntPushConstants pushConstants;
            pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
            GameEngine->PushConstantToCommand(&pushConstants, shadowmapPipelineIndex);

            GameEngine->CmdSetDepthBias(1.25f, 0.0f, 6.0f);

            //object0: middle big sphere
            //object1: table
            //object2(removed): small light sphere0 (light0), because both light cameras are at this position, it should not be drawn
            //object3(removed): small light sphere1 (light1)
            //...
            for(int i = 0; i < 5; i++) {
                GameEngine->DrawObject(i, shadowmapPipelineIndex); 
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassMainscene() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();    
        }
    };
}
#include "launcher.hpp"