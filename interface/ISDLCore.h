#pragma once
#include <string>
#include <vector>
#include <memory>

namespace LEApplication{
    class IApplication;
}

class CInstance;

namespace LESDL{
    class ISDLCore {
    public:
        virtual ~ISDLCore() = default;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        virtual bool IsRunning() = 0;
        virtual void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle) = 0;
        virtual void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions) = 0;
        virtual void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface) = 0;
        virtual void eventHandle() = 0;

        virtual void SetMouseSensibility(float value) = 0;
        virtual void SetKeyboardSensibility(float value) = 0;
    protected:
        LEApplication::IApplication* game;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
