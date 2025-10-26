#pragma once
//#include "TypeAppInfo.h"
#include <iostream>

namespace LEApplication{
    class IApplication;
}

//class AppInfo;

namespace LERenderer{
    class IRendererCore {
    public:
        virtual ~IRendererCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        void greet() {std::cout<<"rendere greet!"<<std::endl;}
        //virtual void ReadYAMLFile(const std::string& filename) = 0;
        //AppInfo& GetAppInfo() { return appInfo; }
    protected:
        //AppInfo appInfo;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
