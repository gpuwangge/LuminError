/* ***********
 * This test draws on the swapchain images, with the use of storage image
 * The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
 * Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
 * Thus no graphics pipeline is needed here
 * When run, a purple rectangle will show on the screen
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"
namespace LuminError{
    class Game : public IGame {
        void PreInitialize() override {
            GameEngine->SetSwapchainImageSize(MAX_FRAMES_IN_FLIGHT);
            GameEngine->EnableComputeSwapChainImage(true);
	    }

        void Initialize() override {
            GameEngine->SetRenderMode(RenderModes::COMPUTE_SWAPCHAIN);
        }

        void RecordComputeCommandBuffer() override{
            //Option 2: record command buffer every frame
            GameEngine->ComputeDispatch(200, 300, 1);
            
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