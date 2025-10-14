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
    struct SimpleComputeStorageImage : public IGame {
        void PreInitialize() override {
            game->SetSwapchainImageSize(MAX_FRAMES_IN_FLIGHT);
            game->EnableComputeSwapChainImage(true);
	    }

        void Initialize() override {
            game->SetRenderMode(RenderModes::COMPUTE_SWAPCHAIN);
        }

        void PostInitialize() override{
            game->CreateComputeCommandBuffers();
        }
    };

    EXPORT_FACTORY_FOR(SimpleComputeStorageImage)
}