#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleDepthImage
class TEST_CLASS_NAME: public CApplication{
//This test draws the depth image on the right side of the screen
//This test also sets up the light cameras for one of the lights, so the light can be observed from the camera's perspective.
//To change the observation from depth image to light shadowmap, change one line of code in observe.frag
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

		//objects[7].SetScaleRectangleXY(0.5, 0.5, 1, 1);
		objects[7].SetScaleRectangleXY(0, -1, 1, 1);
		//objects[7].SetRotation(0,0,180);
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				glm::vec3(0, lights[i].GetLightPosition().y,0) +
				glm::vec3(2.5 *cos(elapseTime * (i+1)), 0, 2.5 *sin(elapseTime * (i+1)))
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 
			//lightCamera[i].SetPosition(lights[i].GetLightPosition());
		}

		lightCameras[0].SetPosition(lights[0].GetLightPosition()); //set light camera to one of the lightball, the lightball should be not drawn in subpass 0.

		//lightCamera.SetPosition(lights[3].GetLightPosition());

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size()-1; i++) {
			if(i == 2) continue; //dont draw the light ball in shadowmap. the 5th object is the 3rd lightball. the 2rd object is the 0th lightball
			//objects[i].m_graphics_pipeline_id = 2;
			objects[i].Draw(2);
		}

		vkCmdNextSubpass(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], VK_SUBPASS_CONTENTS_INLINE);

		for(int i = 0; i < objects.size()-1; i++) {
			//objects[i].m_graphics_pipeline_id = 0;
			objects[i].Draw();
		}

		vkCmdNextSubpass(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], VK_SUBPASS_CONTENTS_INLINE);

		objects[objects.size()-1].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif