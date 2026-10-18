#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>
namespace LuminError{
    class Game : public IGame {
        static const int DIM = 16;
        static const int DIM_M = DIM;
        static const int DIM_K = DIM;
        static const int DIM_N = DIM;
        static const int KernelRunNumber = 1;

        struct StructStorageBuffer {
            unsigned int M;
            unsigned int K;
            unsigned int N;
            float MatA[DIM_M * DIM_K];
            float MatB[DIM_K * DIM_N];
            float MatC[DIM_M * DIM_N];
        };
        StructStorageBuffer storageBufferObject;

        bool bVerbose = true;
	    bool bVerify = true;

        void Initialize() override{
            GameEngine->SetComputeCustomSize(sizeof(StructStorageBuffer));
        }

        void PostInitialize() override{
            storageBufferObject.M = DIM_M;
            storageBufferObject.N = DIM_N;
            storageBufferObject.K = DIM_K;
            for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
            for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;
            if(bVerbose) GameEngine->LogContext("A: ", storageBufferObject.MatA, DIM_M*DIM_K);
            if(bVerbose) GameEngine->LogContext("B: ", storageBufferObject.MatB, DIM_K*DIM_N);

            GameEngine->UploadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
        }

        void Update() override {
            static int counter = 1;
            if(counter==KernelRunNumber) GameEngine->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            GameEngine->ComputeDispatch(1, 1, 1);
        }

        void PostUpdate() override {
            GameEngine->DeviceWaitIdle();

            float cpu_result0 = 0.0f;
            if(bVerify){
                GameEngine->LogContext("Begin verification... ");
                float cpu_result[DIM_M*DIM_N];
                CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObject.MatA, storageBufferObject.MatB, cpu_result, DIM_M*DIM_N);
                GameEngine->LogContext("cpu_C: ", cpu_result, DIM_M*DIM_N);
                cpu_result0 = cpu_result[0];
            }

            if(bVerbose) GameEngine->DownloadComputeStorageBuffer(GameEngine->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); 
            
            if(bVerify){
                
                std::cout<<"gpu_result[0] = "<<storageBufferObject.MatC[0]<<std::endl;
                std::cout<<"cpu_result[0] = "<<cpu_result0<<std::endl;
                std::cout<<"Please manually check validation. "<<std::endl; //TODO: automate verification

                GameEngine->LogContext("gpu C: ", storageBufferObject.MatC, DIM_M*DIM_N);
                GameEngine->LogContext("gpu A: ", storageBufferObject.MatA, DIM_M*DIM_N);
                GameEngine->LogContext("gpu B: ", storageBufferObject.MatB, DIM_M*DIM_N);
            }
        }

        void CPUSingleThreadMatMul(int M, int N, int K, float *matrixA, float *matrixB, float *outputMatrix, int sampleNum){
            std::cout<<"Running CPU single thread mat mul for verification purpose..."<<std::endl;
            int count = 0;
            int printDelta = sampleNum / 1;
            for(int n = 0; n < N; n++){ //col
                for(int m = 0; m < M; m++){ //row： fill row first, allign with kernel
                    //column major matrix multiplication
                    outputMatrix[n*M + m] = 0;
                    for(int k = 0; k < K; k++){
                        outputMatrix[n*M + m] += matrixA[k*M + m] * matrixB[n*K + k];
                    }

                    count++;
                    if(count % printDelta == 0){
                        float completeRate = (count * 100.0)/sampleNum ;
                        GameEngine->LogContext("Completed: {}%", completeRate);
                        std::cout<<"Completed: "<<completeRate<<"%"<<" (sampleNum="<<sampleNum<<", count="<<count<<")"<<std::endl;
                    }
                    
                    if(count >= sampleNum) return;
                }
            }
        }
    };
}
#include "Launcher.hpp"