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


            for (const auto& control : (config)["Controls"])
                if (control["UIContainer"]) appInfo.ControlUIContainer.loadFromYaml(control["UIContainer"]);

            if (config["Features"]) appInfo.Feature.loadFromYaml(config["Features"]);

            auto uniformsNode = config["Uniforms"];
            if(uniformsNode["Graphics"]) appInfo.Uniform.loadGraphicsFromYaml(uniformsNode["Graphics"]);
            if(uniformsNode["Compute"]) appInfo.Uniform.loadComputeFromYaml(uniformsNode["Compute"]);

            if (uniformsNode["GraphicsTextureImageSamplers"]) {
                auto samplersNode = uniformsNode["GraphicsTextureImageSamplers"];
                //std::vector<int> miplevels;
                //std::vector<std::vector<bool>> uvwRepeats;

                for (const auto& samplerNode : samplersNode) {
                    std::string name = getOrDefault<std::string>(samplerNode, "uniform_graphics_texture_image_sampler_name", ""); //not used
                    int miplevel = getOrDefault<int>(samplerNode, "uniform_graphics_texture_image_sampler_miplevel", 0);
                    std::vector<bool> uvwRepeat = getOrDefault<std::vector<bool>>(samplerNode, "uniform_graphics_texture_image_sampler_uvwrepeat", {true,true,true});

                    sampler_miplevels.push_back(miplevel);
                    sampler_uvwRepeats.push_back(uvwRepeat);
                }
            }



            for (const auto& resource : config["Resources"]) {
                if (resource["Fonts"]) {
                    for (const auto& font : resource["Fonts"]) {
                        font_name = font["resource_font_name"].as<std::string>();
                        font_samplerid = font["uniform_sampler_id"].as<int>();
                        font_outlineColor = font["resource_font_outlinecolor"] ? font["resource_font_outlinecolor"].as<std::vector<int>>() : std::vector<int>{255, 255, 255, 255};
                        font_textColor = font["resource_font_textcolor"] ? font["resource_font_textcolor"].as<std::vector<int>>() : std::vector<int>{0, 0, 0, 255};
                        font_size = font["resource_font_size"] ? font["resource_font_size"].as<int>() : 20;
                    }
                }

                if (resource["Models"]) {
                    for (const auto& model : resource["Models"]) {
                        model_names.push_back(model["resource_model_name"] ? model["resource_model_name"].as<std::string>() : "Default");
                        model_ids.push_back(model["resource_model_id"] ? model["resource_model_id"].as<int>() : 0);
                    }
                }

                if (resource["Textures"]) {
                    for (const auto& texture : resource["Textures"]) {
                        texture_names.push_back(texture["resource_texture_name"].as<std::string>());
                        texture_miplevels.push_back(texture["resource_texture_miplevels"].as<int>());
                        texture_enableCubemaps.push_back(texture["resource_texture_cubmap"].as<bool>());
                        texture_samplerids.push_back(texture["uniform_sampler_id"].as<int>());
                    }
                }

                if (resource["Pipelines"]) {
                    for (const auto& pipeline : resource["Pipelines"]) {
                        pipeline_names.push_back(pipeline["resource_graphics_pipeline_name"] ? pipeline["resource_graphics_pipeline_name"].as<std::string>() : "DefaultPipeline");
                        pipeline_vertexShaderNames.push_back(pipeline["resource_graphics_pipeline_vertexshader_name"] ? pipeline["resource_graphics_pipeline_vertexshader_name"].as<std::string>() : "default.vert");
                        pipeline_fragmentShaderNames.push_back(pipeline["resource_graphics_pipeline_fragmentshader_name"] ? pipeline["resource_graphics_pipeline_fragmentshader_name"].as<std::string>() : "default.frag");
                        pipeline_bRenderPassShadowmaps.push_back(pipeline["renderpasses_shadowmap"] ? pipeline["renderpasses_shadowmap"].as<bool>() : false);
                        pipeline_subpassIds.push_back(pipeline["subpasses_subpass_id"] ? pipeline["subpasses_subpass_id"].as<int>() : 0);
                        pipeline_vertexDatatypes.push_back(pipeline["resource_graphics_pipeline_vertexdatatype"] ? pipeline["resource_graphics_pipeline_vertexdatatype"].as<int>() : 2);
                        pipeline_blendEnables.push_back(pipeline["resource_graphics_pipeline_blend_enable"] ? pipeline["resource_graphics_pipeline_blend_enable"].as<bool>() : false);
                        pipeline_depthTestEnables.push_back(pipeline["resource_graphics_pipeline_depth_test_enable"] ? pipeline["resource_graphics_pipeline_depth_test_enable"].as<bool>() : true);
                        pipeline_depthWriteEnables.push_back(pipeline["resource_graphics_pipeline_depth_write_enable"] ? pipeline["resource_graphics_pipeline_depth_write_enable"].as<bool>() : true);
                        pipeline_skyboxEnables.push_back(pipeline["resource_graphics_pipeline_skybox"] ? pipeline["resource_graphics_pipeline_skybox"].as<bool>() : false);
                    }
                }
            }


        }

        virtual void LoadFeatureFromYaml(const YAML::Node& node) = 0;
        virtual void LoadGraphicsFromYaml(const YAML::Node& node) = 0;
        virtual void LoadComputeFromYaml(const YAML::Node& node) = 0;
        virtual void LoadControlUIContainerFromYaml(const YAML::Node& node) = 0;

        AppInfo& GetAppInfo() { return appInfo; }
        YAML::Node& GetConfig() { return config; }

        std::vector<int>& GetSamplerMipLevels() { return sampler_miplevels; }
        std::vector<std::vector<bool>>& GetSamplerUvwRepeats() { return sampler_uvwRepeats; }

        std::string& GetFontName() { return font_name; }
        int& GetFontSamplerId() { return font_samplerid; }
        std::vector<int>& GetOutlineColor() { return font_outlineColor; }
        std::vector<int>& GetTextColor() { return font_textColor; }
        int& GetFontSize() { return font_size; }

        std::vector<std::string>& GetModelNames() { return model_names; }
        std::vector<int>& GetModelIds() { return model_ids; }

        std::vector<std::string>& GetTextureNames() { return texture_names; }
        std::vector<int>& GetTextureMipLevels() { return texture_miplevels; }
        std::vector<bool>& GetTextureEnableCubemaps() { return texture_enableCubemaps; }
        std::vector<int>& GetTextureSamplerIds() { return texture_samplerids; }

        std::vector<std::string>& GetPipelineNames() { return pipeline_names; }
        std::vector<std::string>& GetPipelineVertexShaderNames() { return pipeline_vertexShaderNames; }
        std::vector<std::string>& GetPipelineFragmentShaderNames() { return pipeline_fragmentShaderNames; }
        std::vector<bool>& GetPipelineRenderPassShadowmaps() { return pipeline_bRenderPassShadowmaps; }
        std::vector<int>& GetPipelineSubpassIds() { return pipeline_subpassIds; }
        std::vector<int>& GetPipelineVertexDatatypes() { return pipeline_vertexDatatypes; }
        std::vector<bool>& GetPipelineBlendEnables() { return pipeline_blendEnables; }
        std::vector<bool>& GetPipelineDepthTestEnables() { return pipeline_depthTestEnables; }
        std::vector<bool>& GetPipelineDepthWriteEnables() { return pipeline_depthWriteEnables; }
        std::vector<bool>& GetPipelineSkyboxEnables() { return pipeline_skyboxEnables; }

    private:
        AppInfo appInfo;
        YAML::Node config;

        std::vector<int> sampler_miplevels;
        std::vector<std::vector<bool>> sampler_uvwRepeats;

        std::string font_name;
        int font_samplerid;
        std::vector<int> font_outlineColor;
        std::vector<int> font_textColor;
        int font_size;

        std::vector<std::string> model_names;
        std::vector<int> model_ids;

        std::vector<std::string> texture_names;
        std::vector<int> texture_miplevels;
        std::vector<bool> texture_enableCubemaps;
        std::vector<int> texture_samplerids;

        std::vector<std::string> pipeline_names;
        std::vector<std::string> pipeline_vertexShaderNames;
        std::vector<std::string> pipeline_fragmentShaderNames;
        std::vector<bool> pipeline_bRenderPassShadowmaps;
        std::vector<int> pipeline_subpassIds;
        std::vector<int> pipeline_vertexDatatypes;
        std::vector<bool> pipeline_blendEnables;
        std::vector<bool> pipeline_depthTestEnables;
        std::vector<bool> pipeline_depthWriteEnables;
        std::vector<bool> pipeline_skyboxEnables;

    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
