#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>

namespace LuminError{
    struct GemmCompute : public IGame {
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
        StructStorageBuffer storageBufferObject; //input and output use the same storage. Issue?

        bool bVerbose = true;
	    bool bVerify = true;

        void Initialize() override{
            game->SetRenderMode(RenderModes::COMPUTE);
            game->SetComputeStorageBufferSize(sizeof(StructStorageBuffer));
            game->SetComputeStorageBufferUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }

        void PostInitialize() override{
            //Initial Host data
            storageBufferObject.M = DIM_M;
            storageBufferObject.N = DIM_N;
            storageBufferObject.K = DIM_K;
            for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
            for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;
            if(bVerbose) game->LogContext("A: ", storageBufferObject.MatA, DIM_M*DIM_K);
            if(bVerbose) game->LogContext("B: ", storageBufferObject.MatB, DIM_K*DIM_N);
            //std::cout<<"Initialized A and B."<<std::endl;

            //Host >> Device
            //Notes:
            //- use storage buffer of last frame in flight as compute shader input
            //- there are two storage buffers, binds to #0 and #1
            //- Shader: always use binding#0 as input, binding#1 as output
            //- for flight0, buffer0 is binding#1(output), buffer1 is binding#0(input)
            //- for flight1, buffer0 is binding#0(input),  buffer1 is binding#1(output)
            //- computation starts with flight0, so initialize buffer1 only
            //- the result will be downloaded back to SBO(only MatC has value)
            //game->UploadComputeStorageBuffer(game->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); //upload A/B to buffer0. because buffer0 will be output for frame0, no need to initialize.
            game->UploadComputeStorageBuffer(game->GetCurrentFrame()+1, &storageBufferObject, sizeof(storageBufferObject)); //upload A/B to buffer1: this is the input for the first flight
        }

        void Update() override {
            static int counter = 1;
            if(counter==KernelRunNumber) game->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            game->ComputeDispatch(1, 1, 1);
        }

        void PostUpdate() override {
            game->DeviceWaitIdle();

            float cpu_result0 = 0.0f;
            if(bVerify){
                //std::cout<<"Begin verification..."<<std::endl;
                game->LogContext("Begin verification... ");
                float cpu_result[DIM_M*DIM_N];
                CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObject.MatA, storageBufferObject.MatB, cpu_result, DIM_M*DIM_N);
                //printMatrix(cpu_result, DIM_M, DIM_N, "cpu_C");
                game->LogContext("cpu_C: ", cpu_result, DIM_M*DIM_N);
                cpu_result0 = cpu_result[0];
            }

            if(bVerbose) game->DownloadComputeStorageBuffer(game->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject)); //for the first flight, download result from buffer0 back to SBO. SBO.MatC has result, and SBO.MatA/B will be reset to zero.
            if(bVerbose) game->LogContext("C: ", storageBufferObject.MatC, DIM_M*DIM_N);

            if(bVerify){
                std::cout<<"gpu_result[0] = "<<storageBufferObject.MatC[0]<<std::endl;
                std::cout<<"cpu_result[0] = "<<cpu_result0<<std::endl;
                std::cout<<"Please manually check validation. "<<std::endl; //TODO: automate verification
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
                        game->LogContext("Completed: %f%%", completeRate);
                        std::cout<<"Completed: "<<completeRate<<"%"<<" (sampleNum="<<sampleNum<<", count="<<count<<")"<<std::endl;
                    }
                    
                    if(count >= sampleNum) return;
                }
            }
        }
    };

    EXPORT_FACTORY_FOR(GemmCompute)
}