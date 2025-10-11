#pragma once

#include <string>
#include <vector>
#include <memory>

namespace LEApplication{
    class IApplication;
}
class CInstance;

namespace LESDL{
    struct ISDLCore {
        virtual ~ISDLCore() = default;
        virtual void greet() = 0; 
        virtual void SetApplication(LEApplication::IApplication* pApplication) = 0;
        // virtual void playWith(animal::IPet* pet) = 0;

        // virtual void setPet(animal::IPet *pet) = 0;
        // virtual void playWithPet() = 0;

        virtual bool IsRunning() = 0;
        virtual void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle) = 0;
        virtual void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions) = 0;
        virtual void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface) = 0;
        virtual void eventHandle() = 0;
    };
}
