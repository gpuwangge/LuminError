/************
 * A sample to test storage buffer (for compute)	
 * This sample write 1, 2, 3, 4, 5 to storage buffer and read back
 * storage buffer can be used for gemm
 * 这个test跟particle system一样使用Pingpong buffer(但这并不是必须的，只是测试pingpong buffer的功能)
 * 对于GEMM，即使每一帧都计算，只要每帧仍然是读取 A、B、写入独立的 C，而不是把上一帧结果作为下一帧输入，就没有必要使用 ping-pong
 * 所以pingpong buffer目前只用在本例和particle system中
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>
namespace LuminError{
    class Game : public IGame {
        const int KernelRunNumber = 5;

        struct StructStorageBuffer {
            glm::vec4 data;
        };
        StructStorageBuffer storageBufferObject;

        void Initialize() override{
            GameEngine->SetComputeStorageBufferSize_CustomSwap(sizeof(StructStorageBuffer));
            GameEngine->SetComputeStorageBufferUsage_CustomSwap(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }

        void Update() override {
            static int counter = 1;
            //Initial Host data
            GameEngine->LogContext("update(): write counter = %d to the device, frame = %d", counter, GameEngine->GetCurrentFrame());
            storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};

            //Host >> Device
            //computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, storageBufferObject);
            GameEngine->UploadComputeStorageBuffer_CustomSwap(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject));
            //std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;

            if(counter==KernelRunNumber) GameEngine->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            //In this test, there is no need to dispatch(and no need to bind descriptor).
		    //The storage buffer will work by itself.
        }

        void PostUpdate() override {
            GameEngine->DeviceWaitIdle();

            //Device >> Host
            float data[4] = {0};
            //std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
            //memcpy(data, computeDescriptorManager.storageBuffersMapped[renderer.currentFrame], sizeof(data));
            ///game->DownloadComputeStorageBuffer(game->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject));
            GameEngine->DownloadComputeStorageBuffer_CustomSwap(GameEngine->GetCurrentFrame(), &data, sizeof(data));
            GameEngine->LogContext("compute() read data: ", data, 4);
            std::cout<<"compute() read data: "<<data[0]<<", "<<data[1]<<", "<<data[2]<<", "<<data[3]<<", "<<std::endl;

        }

    };
}
#include "Launcher.hpp"