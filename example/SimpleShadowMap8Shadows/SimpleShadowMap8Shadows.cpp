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
    struct SimpleShadowMap8Shadows : public IGame {
        void Initialize() override{
            game->SetRenderMode(RenderModes::GRAPHICS_SHADOWMAP);
        }

        void Update() override{
            double et = game->GetElapseTime();
            for(int i = 0; i < game->GetLightSize(); i++) {
                game->SetLightPosition(i,
                    glm::vec3(2.5 *cos(et * (i+1) * 0.5), game->GetLightPosition(i).y, 2.5 *sin(et * (i+1) * 0.5))
                );
                game->SetObjectPosition(2+i, game->GetLightPosition(i)); //object2<-light0's position; object3<-light1's position;
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
            for(int i = 0; i < 1; i++) { //only draw table and middle big sphere in shadowmap
                game->DrawObject(i, shadowmapPipelineIndex); 
            }
        }

        void RecordGraphicsCommandBuffer_RenderpassMainscene() override{
            game->DrawObjects();
            game->DrawTexts();    
        }
    };


    EXPORT_FACTORY_FOR(SimpleShadowMap8Shadows)
}