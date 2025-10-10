#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimplePhongLighting
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(glm::vec3(1.5f * cos(elapseTime * 1.2), 1.5f * cos(elapseTime * 0.85), 1.5f * cos(elapseTime * 1)));
			objects[1+i].SetPosition(lights[i].GetLightPosition()); 
		}

		//objectList[0].SetAngularVelocity(0, 50, 0);
		
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}		
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif