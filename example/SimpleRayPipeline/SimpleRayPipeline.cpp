/* ***********
 * This test draws on the swapchain images, with the use of storage image(Bind0)
 * The size of swapchain must be equal to MAX_FRAMES_IN_FLIGHT
 * Because number of descriptor is MAX_FRAMES_IN_FLIGHT, each swapchain imageview must attach to a descriptor
 * Thus no graphics pipeline is needed here
 * Use ray tracing, as is TLAS(bind1)
 * *********** */
#include "IGame.h"
#include "Enum.h"
#include "Config.h"

namespace LuminError{
    class Game : public IGame {
        void Update() override {
            GameEngine->PrintFPS(1.0f);
        }

        void RecordRaytracingCommandBuffer() override{
            GameEngine->RayTrace(800, 800, 1);
        }
    };
}
#include "Launcher.hpp"