// Assume a compute shader with 256 threads per workgroup.
// Each value is 32 bits (4 bytes).
// Write a program to sum all values in a 4 KB buffer.
// Total number of values = 4 KB / 4 B = 1024 values.

#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>
namespace LuminError{
    class Game : public IGame {
        static const int DIM = 1024; //total number of threads
        static const int KernelRunNumber = 1;

        struct StructStorageBuffer { 
            float data[1024];
            float partialSums[4]; //shared 数组用于同一个 Workgroup 内的临时归约；partialSums 数组用于保存不同 Workgroup 的归约结果。
        };
        StructStorageBuffer storageBufferObject;

        bool bVerbose = true;
	    bool bVerify = true;

        void Initialize() override{
            GameEngine->SetComputeCustomSize(sizeof(StructStorageBuffer));
        }

        void PostInitialize() override{
            for(int i = 0; i < DIM; i++) storageBufferObject.data[i] = (float)rand() / (float)RAND_MAX;
            if(bVerbose) GameEngine->LogContext("data: ", storageBufferObject.data, DIM);

            GameEngine->UploadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
        }

        void Update() override {
            static int counter = 1;
            if(counter==KernelRunNumber) GameEngine->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(4, 1, 1);
        }

        void PostUpdate() override {
            GameEngine->DeviceWaitIdle();

            float cpu_result = 0.0f;
            if(bVerify){
                GameEngine->LogContext("Begin verification... ");
                cpu_result = CPUSingleThread(DIM, storageBufferObject.data);
                //std::cout<<"cpu_result = "<<cpu_result<<std::endl;
                //GameEngine->LogContext("cpu_result: ", cpu_result);//todo: not work
            }

            if(bVerbose) GameEngine->DownloadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
            
            if(bVerify){
                std::cout<<"gpu_result[0] = "<<storageBufferObject.partialSums[0]<<std::endl;
                std::cout<<"gpu_result[1] = "<<storageBufferObject.partialSums[1]<<std::endl;
                std::cout<<"gpu_result[2] = "<<storageBufferObject.partialSums[2]<<std::endl;
                std::cout<<"gpu_result[3] = "<<storageBufferObject.partialSums[3]<<std::endl;
                float sum = storageBufferObject.partialSums[0] + storageBufferObject.partialSums[1] + storageBufferObject.partialSums[2] + storageBufferObject.partialSums[3];
                std::cout<<"gpu_result = "<<sum<<std::endl;
                std::cout<<"cpu_result = "<<cpu_result<<std::endl;
            }
        }

        float CPUSingleThread(int N, float *buffer){
            std::cout<<"Running CPU single thread for verification purpose..."<<std::endl;
            float result = 0.0f;
            for(int i = 0; i < N; i++) result += buffer[i];
            return result;
        }
    };
}
#include "Launcher.hpp"