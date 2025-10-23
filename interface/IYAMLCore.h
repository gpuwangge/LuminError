#pragma once
#include "TypeAppInfo.h"

namespace LEApplication{
    class IApplication;
}

namespace LEYAML{
    class IYAMLCore {
    public:
        virtual ~IYAMLCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        virtual void ReadYAMLFile(const std::string& filename) = 0;

        AppInfo& GetAppInfo() { return appInfo; }
        YAML::Node& GetConfig() { return config; }

        int& GetCustomObjectCount() { return customObjectCount; }
        int& GetCustomTextboxCount() { return customTextboxCount; }
        int& GetCustomLightCount() { return customLightCount; }

    protected:
        AppInfo appInfo;
        YAML::Node config;

        int customObjectCount;
        int customTextboxCount;
        int customLightCount;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
