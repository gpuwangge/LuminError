#pragma once

#include <string>
#include <vector>
#include <memory>
#include "IApplication.h"

class CInstance;

namespace LESDL{
    struct ISDLCore {
        virtual ~ISDLCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        virtual bool IsRunning() = 0;
        virtual void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle) = 0;
        virtual void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions) = 0;
        virtual void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface) = 0;
        virtual void eventHandle() = 0;
    };
}
