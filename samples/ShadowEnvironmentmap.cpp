#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CShadowEnvironmentmap
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	}

	void update(){
		// for(int i = 0; i < lightList.size(); i++) {
		// }

		objects[1].SetVelocity(glm::cross(objects[1].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[2].SetVelocity(glm::cross(objects[2].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[3].SetVelocity(glm::cross(objects[3].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[4].SetVelocity(glm::cross(objects[4].Position - glm::vec3(0,0,0), glm::vec3(0,0,1)));
		objects[5].SetVelocity(glm::cross(objects[5].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));
		objects[6].SetVelocity(glm::cross(objects[6].Position - glm::vec3(0,0,0), glm::vec3(0.5,0.5,0)));

		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif