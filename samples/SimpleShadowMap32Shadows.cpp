#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMap32Shadows
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: multiple lights casting multiple shadows on a plane
 * Use two renderpasses: one for shadowmap, one for main scene
 * Use hardware depth bias (vkCmdSetDepthBias)
 * Use push constant to pass shadowmap renderpass index to vertex shader
 */
public:

	void initialize(){
		appInfo.RenderMode = renderer.GRAPHICS_SHADOWMAP; //two renderpasses: shadowmap and main scene

		CApplication::initialize();

		mainCamera.SetRotationSensitivity(20.0f);

		for(int i = 0; i < 8; i++) lightCameras[i].focusObjectId = 1;
		for(int i = 8; i < 16; i++) lightCameras[i].focusObjectId = 2;
		for(int i = 16; i < 24; i++) lightCameras[i].focusObjectId = 3;
		for(int i = 24; i < 32; i++) lightCameras[i].focusObjectId = 4;
	} 

	void update(){
		glm::vec3 pos1 = objects[1].Position; //big spheres
		glm::vec3 pos2 = objects[2].Position;
		glm::vec3 pos3 = objects[3].Position;
		glm::vec3 pos4 = objects[4].Position;

		for(int i = 0; i < lights.size(); i++){
			if(i < 8){
				lights[i].SetLightPosition(glm::vec3(pos1.x + 2.5 *cos(elapseTime * (i+1) * 0.25), lights[i].GetLightPosition().y,  pos1.z + 2.5 *sin(elapseTime * (i+1) * 0.25)));
			}else if(i < 16){
				lights[i].SetLightPosition(glm::vec3(pos2.x + 2.5 *cos(elapseTime * (i+1-8) * 0.25), lights[i].GetLightPosition().y,  pos2.z + 2.5 *sin(elapseTime * (i+1-8) * 0.25)));
			}else if (i < 24){
				lights[i].SetLightPosition(glm::vec3(pos3.x + 2.5 *cos(elapseTime * (i+1-16) * 0.25), lights[i].GetLightPosition().y,  pos3.z + 2.5 *sin(elapseTime * (i+1-16) * 0.25)));
			}else if (i < 32){
				lights[i].SetLightPosition(glm::vec3(pos4.x + 2.5 *cos(elapseTime * (i+1-24) * 0.25), lights[i].GetLightPosition().y,  pos4.z + 2.5 *sin(elapseTime * (i+1-24) * 0.25)));
			}
			lightCameras[i].SetPosition(lights[i].GetLightPosition());
		}

		CApplication::update();
		//std::cout<<"mainCamera.focusObjectId: "<<mainCamera.focusObjectId<<std::endl;
	}

	void recordGraphicsCommandBuffer_renderpassShadowmap(int renderpassIndex){
		IntPushConstants pushConstants;
        pushConstants.value = renderpassIndex; //pass shadowmap renderpass index to device
        renderer.PushConstantToCommand<IntPushConstants>(pushConstants, renderProcess.graphicsPipelineLayouts[0], shaderManager.pushConstantRange); //pipeline0 is for shadowmap
		
		vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 1.25f, 0.0f, 6.0f);

		//object0: table
		//object1: middle big sphere
		//object2(removed): small light sphere0 (light0), because both light cameras are at this position, it should not be drawn
		//object3(removed): small light sphere1 (light1)
		for(int i = 0; i < objects.size(); i++) {
			//if(i == 2) continue; //dont draw object2(light0) in shadowmap
			//if(i == 3) continue; //dont draw object3(light1) in shadowmap
			objects[i].Draw(0); //pipeline0 is for shadowmap
		}
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) 
			objects[i].Draw(); //use object's default pipeline#. In this case it is pipeline2
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif