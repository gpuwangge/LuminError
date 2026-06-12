/* ***********
 * This test draws on the swapchain images, with the use of storage image
 * The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
 * Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
 * Thus no graphics pipeline is needed here
 * When run, a blue rectangle will show on the screen
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"
//#include <iostream>
namespace LuminError{
    class Game : public IGame {
        void RecordRaytracingCommandBuffer() override{
            //Option 2: record command buffer every frame
            //std::cout<<"Game: Start recording raytracing command buffer for RAYTRACING_SWAPCHAIN mode."<<std::endl;
            GameEngine->RayTrace(200, 300, 1);
            //std::cout<<"Game: Finished recording raytracing command buffer for RAYTRACING_SWAPCHAIN mode."<<std::endl;
            
            //Option 3: another test function
            //GameEngine->CreateComputeCommandBuffers_DispatchForSwapchainImage_(200, 300, 1); 
        }

        //void PostInitialize() override{
            //Note: in this test, commandBuffers are recorded once(because they do not change every frame)
            //Option 1: record command buffer once
            //GameEngine->CreateComputeCommandBuffers_DispatchForSwapchainImage(200, 300, 1);
        //}
    };
}
#include "Launcher.hpp"