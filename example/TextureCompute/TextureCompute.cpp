/* ***********
 * This test is similar to simpleComputeStorageImage, but instead use 2 texture image, one as input, the other as output
 * It also render the result to swapchain
 * simpleComputeStorageImage: write result to texture image and show to swapchain
 * textureCompute: read texture image, make changes(use compute shader to do blur effect), and write to texture image, and show to swapchain
 * This test use graphics pipeline but does not draw anything with it, because result is drawn directly to swap chain
 * The reason for graphics pipeline is to load texture to texture image
 * no need bind graphics pipeline?
 * TODO: improve this sample, there is still something unknown for this test; something can be simplified
 * A texture image is presented on the screen. Image is blured. char is blue/white. background is yellow.
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

        void PostInitialize() override{
            //Note: in this test, commandBuffers are recorded once(because they do not change every frame)
            GameEngine->CreateComputeCommandBuffers_DispatchForSwapchainImage(300, 600, 1);
        }

        void PostUpdate() override {
            GameEngine->DeviceWaitIdle();
        }
    };
}
#include "Launcher.hpp"