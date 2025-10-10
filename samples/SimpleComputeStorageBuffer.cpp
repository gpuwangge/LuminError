#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CSimpleComputeStorageBuffer
class TEST_CLASS_NAME: public CApplication{
//A sample to test storage buffer (for compute)	
//This sample write 1, 2, 3, 4, 5 to storage buffer and read back
//storage buffer can be used for gemm
public:
	const int KernelRunNumber = 5;
	struct StructStorageBuffer {
		glm::vec4 data;
	};
	StructStorageBuffer storageBufferObject;

	void initialize(){
		appInfo.RenderMode = renderer.COMPUTE;
		appInfo.Uniform.ComputeStorageBuffer.Size = 1; //this can be any non-zero number, in order to enable storage buffer
		appInfo.Uniform.ComputeStorageBuffer.Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		CApplication::initialize();
	}

	void update(){
		static int counter = 1;
		//Initial Host data
		PRINT("update(): write counter = %d to the device, frame = %d", counter, renderer.currentFrame);
		storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};

		//Host >> Device
		computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, storageBufferObject);
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;

		if(counter==KernelRunNumber) NeedToPause = true; 
		counter++;

		CApplication::update(); //update time
	}

	void recordComputeCommandBuffer(){
		//In this test, there is no need to dispatch(and no need to bind descriptor).
		//The storage buffer will work by itself.
	}

	void postUpdate(){
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		//Device >> Host
		float data[4] = {0};
		//std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
		memcpy(data, computeDescriptorManager.storageBuffersMapped[renderer.currentFrame], sizeof(data));

		PRINT("compute() read data: ", data, 4);
		std::cout<<"compute() read data: "<<data[0]<<", "<<data[1]<<", "<<data[2]<<", "<<data[3]<<", "<<std::endl;
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

