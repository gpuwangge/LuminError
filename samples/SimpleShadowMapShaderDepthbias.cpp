#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleShadowMapShaderDepthbias
class TEST_CLASS_NAME: public CApplication{
/*********
 * Implementation of Shadow Map
 * Simple scenario: A single light source casting one shadow on a plane
 * Use one renderpass, use two subpasses: one for shadowmap, one for main scene
 * Use software depth bias
 */
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 1, 3};


	void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D); //create the 0th custom model 3D (CUSTOM3D0)

		CApplication::initialize();

		//objectList[0].SetRotation(-135,0,45);
		//objectList[0].YawLeft(90,200);
		//objectList[0].RollLeft(90, 200);

		objects[3].SetScaleRectangleXY(0.5, 0.5, 1, 1);
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				//glm::vec3(lights[i].GetLightPosition().x, lights[i].GetLightPosition().y,lights[i].GetLightPosition().z)
				glm::vec3(0, 3.5 + sin(elapseTime * (i+1)),0)
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 
			lightCameras[i].SetPosition(lights[i].GetLightPosition());
		}
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size()-1; i++) {
			if(i == 2) continue; //dont draw the light ball in shadowmap
			//objects[i].m_graphics_pipeline_id = 2; 

			//depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor
			//vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], 10.0f, 0.0f, 10.0f);

			objects[i].Draw(2); //draw with graphics pipeline id=2 (which is shadowmap)
		}

		vkCmdNextSubpass(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], VK_SUBPASS_CONTENTS_INLINE);

		for(int i = 0; i < objects.size()-1; i++) {
			//if(i == 2) continue; //dont draw the light ball
			//objects[i].m_graphics_pipeline_id = 0; //same object, first subpass use gid=2, second subpass use gid=0
			objects[i].Draw();//draw objects with default graphics pipeline id (0)
		}

		vkCmdNextSubpass(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], VK_SUBPASS_CONTENTS_INLINE);

		objects[objects.size()-1].Draw(); //draw for observation, use default (1)
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif