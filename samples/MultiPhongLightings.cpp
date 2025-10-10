#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CMultiPhongLightings
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	} 

	void update(){
		for(int i = 0; i < lights.size(); i++) {
			lights[i].SetLightPosition(
				glm::vec3(0, lights[i].GetLightPosition().y,0) +
				glm::vec3(2.5 *cos(elapseTime * (i+1)), 0, 2.5 *sin(elapseTime * (i+1)))
			);
			objects[2+i].SetPosition(lights[i].GetLightPosition()); 
		}

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}		
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif