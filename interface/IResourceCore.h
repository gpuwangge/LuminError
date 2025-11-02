#pragma once
#include <iostream>
#include "ILogCore.h"

namespace LEApplication{
    class IApplication;
}

namespace LEResource{
    class IResourceCore {
    public:
        virtual ~IResourceCore() = default;
        virtual void SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) = 0;
        virtual void SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_) = 0;

    protected:
        LEApplication::IApplication* game;

    }; //class

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
} //namespace
