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

        void Update() override{
            double et = GameEngine->GetElapseTime();
            for(int i = 0; i < GameEngine->GetLightSize(); i++) {
                GameEngine->SetLightPosition(i,
                    glm::vec3(2.5 *cos(et * (i+1)), GameEngine->GetLightPosition(i).y, 2.5 *sin(et * (i+1)))
                );
                GameEngine->SetObjectPosition(2+i, GameEngine->GetLightPosition(i));
                GameEngine->SetLightCameraPosition(i, GameEngine->GetLightPosition(i));
		    }
        }

        void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) override{
            int shadowmapPipelineIndex = 2;

            IntPushConstants pushConstants;
            pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
            GameEngine->PushConstantToCommand(&pushConstants, shadowmapPipelineIndex);

            GameEngine->CmdSetDepthBias(1.25f, 0.0f, 6.0f);

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
#include "launcher.hpp"