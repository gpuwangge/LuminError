/************
 * This example is a program to sum all values in a 256 KB buffer.
 * Each value is 32 bits (4 bytes).
 * Total number of values = 256 KB / 4 B = 65536 values (64K values).
 * 
 * 8 workgroup(set in host)
 * 128 threads per workgroup(set in shader)
 * total 1024 threads (8 * 128 = 1024), each thread will compute 64 values (65536 / 1024 = 64)
 * 
 * Each workgroup will compute a partial sum of 128 values
 * and then the 8 partial sums will be combined to get the final result.
*************/

#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>
namespace LuminError{
    class Game : public IGame {
        //static const int BufferSize = 256 * 1024; // 256 KB
        static const int NUM_THREADS = 1024; //total number of threads
        static const int NUM_DATA = 65536;
        static const int NUM_WORKGROUP = 8; //number of threads per workgroup
        static const int KernelRunNumber = 1;

        struct StructStorageBuffer { 
            float inputData[NUM_DATA];
            float partialSums[NUM_WORKGROUP]; //shared 数组用于同一个 Workgroup 内的临时归约；partialSums 数组用于保存不同 Workgroup 的归约结果。
        };
        StructStorageBuffer storageBufferObject;

        bool bVerbose = true;
	    bool bVerify = true;

        void Initialize() override{
            GameEngine->SetComputeCustomSize(sizeof(StructStorageBuffer));
        }

        void PostInitialize() override{
            for(int i = 0; i < NUM_DATA; i++) storageBufferObject.inputData[i] = (float)rand() / (float)RAND_MAX;
            //if(bVerbose) GameEngine->LogContext("data: ", storageBufferObject.data, DIM);

            GameEngine->UploadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
        }

        void Update() override {
            static int counter = 1;
            if(counter==KernelRunNumber) GameEngine->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(NUM_WORKGROUP, 1, 1);
        }

        void PostUpdate() override {
            GameEngine->DeviceWaitIdle();

            float cpu_result = 0.0f;
            if(bVerify){
                GameEngine->LogContext("Begin verification... ");
                cpu_result = CPUSingleThread(NUM_DATA, storageBufferObject.inputData);
                //std::cout<<"cpu_result = "<<cpu_result<<std::endl;
                //GameEngine->LogContext("cpu_result: ", cpu_result);//todo: not work
            }

            if(bVerbose) GameEngine->DownloadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
            
            if(bVerify && bVerbose){
                for(int i = 0; i < NUM_WORKGROUP; i++) std::cout<<"gpu_result["<<i<<"] = "<<storageBufferObject.partialSums[i]<<std::endl;
                float gpu_result = 0.0f;
                for(int i = 0; i < NUM_WORKGROUP; i++) gpu_result += storageBufferObject.partialSums[i];
                std::cout<<"gpu_result = "<<gpu_result<<std::endl;
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