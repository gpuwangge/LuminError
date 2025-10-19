#pragma once

#ifdef WIN32
    #define YAML_CPP_STATIC_DEFINE //to disable lots of yaml warnings
#endif
#include "../thirdParty/yaml-cpp/yaml.h"

#include "IApplication.h"

namespace LEYAML{
    class IYAMLCore {
    public:
        virtual ~IYAMLCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}

        virtual void Greet() = 0;
        virtual void LoadFeatureFromYaml(const YAML::Node& node) = 0;
        virtual void LoadGraphicsFromYaml(const YAML::Node& node) = 0;
        virtual void LoadComputeFromYaml(const YAML::Node& node) = 0;
        virtual void LoadControlUIContainerFromYaml(const YAML::Node& node) = 0;


    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
