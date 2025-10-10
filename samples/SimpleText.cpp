#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleText
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
		textManager.Draw();
	}	
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif