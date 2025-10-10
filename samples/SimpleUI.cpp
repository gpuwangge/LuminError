/************
 * This sample is to test render 2D UI and 3D Object in a single scene
 * *********** */

#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleUI

//CApplication CSkyvision::*m_app;
class TEST_CLASS_NAME: public CApplication{
public:
	std::vector<Vertex3D> vertices3D = {
		{ { -0.5f, -0.5f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, -0.5f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { 0.5f, 0.5f, 0.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }},
		{ { -0.5f, 0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } ,{ 0.0f, 0.0f, 1.0f }}
	};
	std::vector<uint32_t> indices3D = { 0, 1, 2, 2, 3, 0};

	//TODO: need simply draw and update different cubes
	// class CCube : public CObject{
	// public:
	// 	CCube(){} //TODO: merge load model/texture and record action into CObject...
	// };

	// CCube cubes[2];
	
    void initialize(){
		modelManager.CreateCustomModel3D(vertices3D, indices3D); //create the 0th custom model 3D (CUSTOM3D0)
		CApplication::initialize();

		//objectList[0].SetPosition(-0.5,-0.5,0.0);
		//objectList[0].SetScale(1, 2, 1);
		//objectList[0].SetScaleRectangleXY(0, 0, 1, 1);
		objects[0].SetScaleRectangleXY(-1, -1, 0, 0);
		//objectList[0].SetPosition(-1, -1,0.1);
		objects[3].SetScaleRectangleXY(0.5, 0.5, 1, 1);
	}

	void update(){
		//objectList[0].SetVelocity(0, sin(durationTime * 2), 0);
		//objectList[0].SetAngularVelocity(0,0,50); //rotation around z axis
		objects[1].SetAngularVelocity(50,0,50); //rotation around x and z axis
		objects[2].SetVelocity(0, 3*sin(elapseTime * 2), 0);
		//objectList[3].SetVelocity(3*sin(durationTime * 2), 0, 0);
		CApplication::update();
	}

	void recordGraphicsCommandBuffer_renderpassMainscene(){
		//objectList[0].Draw();
		//objectList[1].Draw();
		//objectList[3].Draw();
		for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	}

	// ~TEST_CLASS_NAME(){
	// 	cubes[0].CleanUp();
	// 	cubes[1].CleanUp();
	// }
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif