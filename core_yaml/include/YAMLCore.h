#pragma once
#include "IYAMLCore.h"

#ifdef WIN32
    #define YAML_CPP_STATIC_DEFINE //to disable lots of yaml warnings
#endif
#include "../../external/yaml-cpp/yaml.h"

namespace LEYAML{
    class YAMLCore final : public IYAMLCore{
    public:
        YAMLCore(){}
        ~YAMLCore(){}
        void ReadYAMLFile(const std::string& filename) override;
    private:
        YAML::Node yamlNode;
    };
    EXPORT_FACTORY_FOR(YAMLCore);
}

