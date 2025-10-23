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

        virtual void ReadYAMLFile(const std::string& filename) = 0;

        // virtual void LoadFeatureFromYaml(const YAML::Node& node) = 0;
        // virtual void LoadGraphicsFromYaml(const YAML::Node& node) = 0;
        // virtual void LoadComputeFromYaml(const YAML::Node& node) = 0;
        // virtual void LoadControlUIContainerFromYaml(const YAML::Node& node) = 0;

        AppInfo& GetAppInfo() { return appInfo; }
        YAML::Node& GetConfig() { return config; }

        std::vector<int>& GetSamplerMipLevels() { return sampler_miplevels; }
        std::vector<std::vector<bool>>& GetSamplerUvwRepeats() { return sampler_uvwRepeats; }

        std::vector<std::string>& GetModelNames() { return model_names; }
        std::vector<int>& GetModelIds() { return model_ids; }

        std::vector<std::string>& GetTextureNames() { return texture_names; }
        std::vector<int>& GetTextureMipLevels() { return texture_miplevels; }
        std::vector<bool>& GetTextureEnableCubemaps() { return texture_enableCubemaps; }
        std::vector<int>& GetTextureSamplerIds() { return texture_samplerids; }

        int& GetCustomObjectCount() { return customObjectCount; }
        int& GetCustomTextboxCount() { return customTextboxCount; }
        int& GetCustomLightCount() { return customLightCount; }

    protected:
        AppInfo appInfo;
        YAML::Node config;

        std::vector<int> sampler_miplevels;
        std::vector<std::vector<bool>> sampler_uvwRepeats;

        std::vector<std::string> model_names;
        std::vector<int> model_ids;

        std::vector<std::string> texture_names;
        std::vector<int> texture_miplevels;
        std::vector<bool> texture_enableCubemaps;
        std::vector<int> texture_samplerids;

        int customObjectCount;
        int customTextboxCount;
        int customLightCount;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
