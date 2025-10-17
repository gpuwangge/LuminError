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
    struct SimpleShadowMap16Shadows : public IGame {
        void Initialize() override{
            game->SetRenderMode(RenderModes::GRAPHICS_SHADOWMAP);
        }

        void PostInitialize() override{
            game->SetMainCameraSensitivity(20.0f);

            for(int i = 0; i < 8; i++) game->SetLightCameraFocusObjectId(i, 1);
            for(int i = 8; i < 16; i++) game->SetLightCameraFocusObjectId(i, 2);
            //for(int i = 16; i < 24; i++) game->SetLightCameraFocusObjectId(i, 3);
            //for(int i = 24; i < 32; i++) game->SetLightCameraFocusObjectId(i, 4);
        }

        void Update() override{
            double et = game->GetElapseTime();

            glm::vec3 pos1 = game->GetObjectPosition(1);  //big spheres
            glm::vec3 pos2 = game->GetObjectPosition(2);
            glm::vec3 pos3 = game->GetObjectPosition(3);
            glm::vec3 pos4 = game->GetObjectPosition(4);

            for(int i = 0; i < game->GetLightSize(); i++){
                if(i < 8){
                    game->SetLightPosition(i,
                        glm::vec3(pos1.x + 2.5 *cos(et * (i+1) * 0.25), game->GetLightPosition(i).y,  pos1.z + 2.5 *sin(et * (i+1) * 0.25)));
                }else if(i < 16){
                    game->SetLightPosition(i,
                        glm::vec3(pos2.x + 2.5 *cos(et * (i+1-8) * 0.25), game->GetLightPosition(i).y,  pos2.z + 2.5 *sin(et * (i+1-8) * 0.25)));
                }
                // else if (i < 24){
                //     lights[i].SetLightPosition(glm::vec3(pos3.x + 2.5 *cos(et * (i+1-16) * 0.25), lights[i].GetLightPosition().y,  pos3.z + 2.5 *sin(et * (i+1-16) * 0.25)));
                // }else if (i < 32){
                //     lights[i].SetLightPosition(glm::vec3(pos4.x + 2.5 *cos(et * (i+1-24) * 0.25), lights[i].GetLightPosition().y,  pos4.z + 2.5 *sin(et * (i+1-24) * 0.25)));
                // }
                game->SetLightCameraPosition(i, game->GetLightPosition(i));
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) override{
            int shadowmapPipelineIndex = 2;

            IntPushConstants pushConstants;
            pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
            game->PushConstantToCommand(&pushConstants, shadowmapPipelineIndex);

            game->CmdSetDepthBias(1.25f, 0.0f, 6.0f);

            //object0: middle big sphere
            //object1: table
            //object2(removed): small light sphere0 (light0), because both light cameras are at this position, it should not be drawn
            //object3(removed): small light sphere1 (light1)
            //...
            for(int i = 0; i < 5; i++) {
                game->DrawObject(i, shadowmapPipelineIndex); 
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassMainscene() override{
            game->DrawObjects();
            game->DrawTexts();    
        }
    };


    EXPORT_FACTORY_FOR(SimpleShadowMap16Shadows)
}