#pragma once
#include "TypeAppInfo.h"

namespace LEApplication{
    class IApplication;
}

namespace LEYAML{
    class IYAMLCore {
    public:
        virtual ~IYAMLCore() = default;
        virtual void ReadYAMLFile(const std::string& filename) = 0;
        AppInfo& GetAppInfo() { return appInfo; }
    protected:
        AppInfo appInfo;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
