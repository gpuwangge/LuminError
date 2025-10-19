#pragma once
#include "IApplication.h"

namespace LEYAML{
    class IYAMLCore {
    public:
        virtual ~IYAMLCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        virtual void Greet() = 0;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
