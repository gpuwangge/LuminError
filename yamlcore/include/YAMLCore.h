#pragma once

#include "IYAMLCore.h"
//#include <memory>
//#include <vector>
#include <iostream>
//#include <vulkan/vulkan.h>

//class CApplication;

namespace LEYAML{
    class YAMLCore final : public IYAMLCore{
    public:
        YAMLCore(){}
        ~YAMLCore(){}

        void LoadFeatureFromYaml(const YAML::Node& node) override {
        }

        void LoadGraphicsFromYaml(const YAML::Node& node) override {
        }

        void LoadComputeFromYaml(const YAML::Node& node) override {
        }

        void LoadControlUIContainerFromYaml(const YAML::Node& node) override {
        }

    };
    EXPORT_FACTORY_FOR(YAMLCore);
}

