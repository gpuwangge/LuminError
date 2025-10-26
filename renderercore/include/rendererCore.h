#pragma once
#include "IRendererCore.h"

namespace LERenderer{
    class RendererCore final : public IRendererCore{
    public:
        RendererCore(){}
        ~RendererCore(){}
        //void ReadYAMLFile() override;
    private:
        //YAML::Node yamlNode;
    };
    EXPORT_FACTORY_FOR(RendererCore);
}

