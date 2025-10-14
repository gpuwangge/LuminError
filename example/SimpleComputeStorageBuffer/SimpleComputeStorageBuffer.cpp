/************
 * A sample to test storage buffer (for compute)	
 * This sample write 1, 2, 3, 4, 5 to storage buffer and read back
 * storage buffer can be used for gemm
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include <vulkan/vulkan.h>
#include <iostream>

namespace LuminError{
    struct SimpleComputeStorageBuffer : public IGame {
        const int KernelRunNumber = 5;
        struct StructStorageBuffer {
            glm::vec4 data;
        };
        StructStorageBuffer storageBufferObject;

        void Initialize() override{
            game->SetRenderMode(RenderModes::COMPUTE);
            game->SetComputeStorageBufferSize(1); //this can be any non-zero number, in order to enable storage buffer
            game->SetComputeStorageBufferUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }

        void Update() override {
            static int counter = 1;
            //Initial Host data
            game->LogContext("update(): write counter = %d to the device, frame = %d", counter, game->GetCurrentFrame());
            storageBufferObject.data = {counter+0.0f, counter+0.1f, counter+0.2f, counter+0.3f};

            //Host >> Device
            //computeDescriptorManager.updateStorageBuffer<StructStorageBuffer>(renderer.currentFrame, storageBufferObject);
            game->UploadComputeStorageBuffer(game->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject));
            //std::cout<<"update(): Delta Time: "<<deltaTime<<", Duration Time: "<<durationTime<<std::endl;

            if(counter==KernelRunNumber) game->SetPause(true);
            counter++;
        }

        void RecordComputeCommandBuffer() override{
            //In this test, there is no need to dispatch(and no need to bind descriptor).
		    //The storage buffer will work by itself.
        }

        void PostUpdate() override {
            game->DeviceWaitIdle();

            //Device >> Host
            float data[4] = {0};
            //std::cout<<"compute(): Current Frame = "<<renderer.currentFrame<<": "<<std::endl;
            //memcpy(data, computeDescriptorManager.storageBuffersMapped[renderer.currentFrame], sizeof(data));
            ///game->DownloadComputeStorageBuffer(game->GetCurrentFrame(), &storageBufferObject, sizeof(storageBufferObject));
            game->DownloadComputeStorageBuffer(game->GetCurrentFrame(), &data, sizeof(data));
            game->LogContext("compute() read data: ", data, 4);
            std::cout<<"compute() read data: "<<data[0]<<", "<<data[1]<<", "<<data[2]<<", "<<data[3]<<", "<<std::endl;

        }

    };

    EXPORT_FACTORY_FOR(SimpleComputeStorageBuffer)
}