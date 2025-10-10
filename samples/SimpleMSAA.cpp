#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleMSAA
class TEST_CLASS_NAME: public CApplication{
public:
    void initialize(){
		CApplication::initialize();
	}

	void update(){
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		objects[0].Draw();
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif