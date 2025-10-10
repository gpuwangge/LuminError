#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMapHWDepthbias
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: A single light source casting one shadow on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 * Use push constant to pass shadowmap renderpass index to vertex shader
 */
public:
	void initialize(){
		appInfo.RenderMode = renderer.GRAPHICS_SHADOWMAP; //two renderpasses: shadowmap and main scene
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				//glm::vec3(lights[i].GetLightPosition().x, lights[i].GetLightPosition().y,lights[i].GetLightPosition().z)
				glm::vec3(2.5 *cos(elapseTime * (i+1)), lights[i].GetLightPosition().y, 2.5 *sin(elapseTime * (i+1)))
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 

			lightCameras[i].SetPosition(lights[i].GetLightPosition());
		}
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassShadowmap(int renderpassIndex){
		IntPushConstants pushConstants;
        pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
        renderer.PushConstantToCommand<IntPushConstants>(pushConstants, renderProcess.graphicsPipelineLayouts[0], shaderManager.pushConstantRange); //pipeline0 is for shadowmap

		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 1.75f); (self-shadowing)
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 2.0f, 0.0f, 2.5f);
		//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 2.0f, 0.01f, 2.5f); //need enable clamp
		vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 6.0f);

		for(int i = 0; i < objects.size()-1; i++) {
			//if(i == 2) continue; //dont draw the light ball in shadowmap
			objects[i].Draw(0); //pipeline0 is for shadowmap
		}
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) 
			objects[i].Draw(); //use object's default pipeline#. In this case it is pipeline1
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif