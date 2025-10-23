#pragma once
#include "IYAMLCore.h"

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

