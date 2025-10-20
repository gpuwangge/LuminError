#pragma once

#include "TypeAppInfo.h"
#include "IApplication.h"
#include "Foundation.h"
#include <iostream>

namespace LEYAML{
    class IYAMLCore {
    public:
        virtual ~IYAMLCore() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        void ReadYAMLFile(const std::string& filename) {
            std::string fullYamlName = YAML_PATH + filename + ".yaml";
            std::cout<<"Loading YAML file: "<<fullYamlName<<std::endl;
            try{
                config = YAML::LoadFile(fullYamlName);
            } catch (...){
                std::cout<<"Error loading yaml file"<<std::endl;
                return;
            }
        }

        virtual void LoadFeatureFromYaml(const YAML::Node& node) = 0;
        virtual void LoadGraphicsFromYaml(const YAML::Node& node) = 0;
        virtual void LoadComputeFromYaml(const YAML::Node& node) = 0;
        virtual void LoadControlUIContainerFromYaml(const YAML::Node& node) = 0;

        AppInfo& GetAppInfo() { return appInfo; }
        YAML::Node& GetConfig() { return config; }

    private:
        AppInfo appInfo;
        YAML::Node config;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
