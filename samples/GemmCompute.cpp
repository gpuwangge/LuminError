#include "..\\vulkanFramework\\include\\application.h"
#define TEST_CLASS_NAME CGemmCompute
class TEST_CLASS_NAME: public CApplication{
public:
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
	// no need create a separate output
	// struct StructStorageBufferOutput {
	// 	float MatC[DIM_M * DIM_N];
	// };
	// StructStorageBufferOutput storageBufferObjectOutput;	

	bool bVerbose = true;
	bool bVerify = true;

	void initialize(){
		appInfo.RenderMode = renderer.COMPUTE;

		appInfo.Uniform.ComputeStorageBuffer.Size = sizeof(StructStorageBuffer);
		appInfo.Uniform.ComputeStorageBuffer.Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		CApplication::initialize();

		//Initial Host data
		storageBufferObject.M = DIM_M;
		storageBufferObject.N = DIM_N;
		storageBufferObject.K = DIM_K;
		for(int i = 0; i < DIM_M*DIM_K; i++) storageBufferObject.MatA[i] = (float)rand() / (float)RAND_MAX;
		for(int i = 0; i < DIM_K*DIM_N; i++) storageBufferObject.MatB[i] = (float)rand() / (float)RAND_MAX;
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatA, DIM_M, DIM_K, "A");
		//if(bVerbose) printMatrix(storageBufferObjectInput.MatB, DIM_K, DIM_N, "B");
		if(bVerbose) PRINT("A: ", storageBufferObject.MatA, DIM_M*DIM_K);
		if(bVerbose) PRINT("B: ", storageBufferObject.MatB, DIM_K*DIM_N);
		if(bVerbose) PRINT("");
		std::cout<<"Initialized A and B."<<std::endl;

		//Host >> Device
		computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, storageBufferObject); //1 TODO: fill all inflight, update input only
		computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame+1, storageBufferObject); //1
	}

	void update(){
		std::cout<<"update."<<std::endl;
		static int counter = 1;
		if(counter==KernelRunNumber) NeedToPause = true;
		counter++;
		
		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);
	}

	void recordComputeCommandBuffer(){
		Dispatch(1,1,1);
	}

	void postUpdate(){
		std::cout<<"postUpdate."<<std::endl;
		vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());

		CApplication::update(); //update deltaTime and durationTime (and mainCamera and MVP, VP)
		//std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;
		//PRINT("update(): Delta Time: %f, Duration Time: %f", deltaTime, durationTime);

		//Device >> Host
		//if(bVerbose) memcpy(storageBufferObject.MatC, descriptors[0].storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject.MatC));//2
		if(bVerbose) memcpy(&storageBufferObject, computeDescriptorManager.storageBuffersMapped[renderer.currentFrame], sizeof(storageBufferObject));//2 TODO: how to copy only MatC
		//if(bVerbose) printMatrix(storageBufferObjectOutput.MatC, DIM_M, DIM_N, "C");
		if(bVerbose) PRINT("C: ", storageBufferObject.MatC, DIM_M*DIM_N);

		if(bVerify){
			//std::cout<<"Begin verification..."<<std::endl;
			PRINT("Begin verification... ");
			float cpu_result[DIM_M*DIM_N];
			CPUSingleThreadMatMul(DIM_M, DIM_N, DIM_K, storageBufferObject.MatA, storageBufferObject.MatB, cpu_result, DIM_M*DIM_N);
			//printMatrix(cpu_result, DIM_M, DIM_N, "cpu_C");
			PRINT("cpu_C: ", cpu_result, DIM_M*DIM_N);
			std::cout<<"gpu_result[0] = "<<storageBufferObject.MatC[0]<<std::endl;
			std::cout<<"cpu_result[0] = "<<cpu_result[0]<<std::endl;
			std::cout<<"Please manually check validation. "<<std::endl; //TODO: automate verification
		}

		if(bVerbose) PRINT("");
	}

/*
	void printMatrix(float *matA, int DIM_M, int DIM_N, std::string matName){
#ifndef ANDROID		
		std::cout<<"Print "<<matName<<std::endl;
		for(int i=0; i < DIM_M; i++){
			for(int j=0; j < DIM_N; j++){
				std::cout<<matA[i*DIM_N + j]<<", ";
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
#else
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Print %s\n", matName.c_str());
		for(int i=0; i < DIM_M; i++){
			for(int j=0; j < DIM_N; j++){
				__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "%f, ", matA[i*DIM_N + j]);
			}
			//__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "\n");
		}
		__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "\n");
#endif
	}
*/
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
					PRINT("Completed: %f%%", completeRate);
					std::cout<<"Completed: "<<completeRate<<"%"<<" (sampleNum="<<sampleNum<<", count="<<count<<")"<<std::endl;
				}
				
				if(count >= sampleNum) return;
			}
		}
	}
};

#ifndef ANDROID
#include "..\\vulkanFramework\\include\\main.hpp"
#endif

