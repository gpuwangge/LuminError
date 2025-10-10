#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleTriangle
//a blue triangle on the screen
class TEST_CLASS_NAME: public CApplication{
public:
	void initialize(){
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		//for drawing triangle
		//objects[0].Draw(0, 3); //draw 3 vertices;

		//for drawing panels and triangle
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
		textManager.Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

