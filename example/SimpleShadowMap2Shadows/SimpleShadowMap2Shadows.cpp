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
        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i < GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i,
                    glm::vec3(2.5 *cos(et * (i+1) * 0.5), GameEngine->GetLightPosition(i).y, 2.5 *sin(et * (i+1) * 0.5))
                );
                //game->SetObjectPosition(2+i, game->GetLightPosition(i)); //object2<-light0's position; object3<-light1's position;
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
            for(int i = 0; i < 2; i++) { //only draw table and middle big sphere in shadowmap
                GameEngine->DrawObject(i, shadowmapPipelineIndex); 
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassMainscene() override{
            GameEngine->DrawObjects();
            GameEngine->DrawTexts();    
        }
    };
}
#include "Launcher.hpp"