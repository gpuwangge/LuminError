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


            //Must initialize smart pointers, otherwise will crash
            appInfo.VertexShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            appInfo.FragmentShader =  std::make_unique<std::vector<std::string>>(std::vector<std::string>());
            appInfo.RenderPassShadowmap = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.Subpass =  std::make_unique<std::vector<int>>(std::vector<int>());
            appInfo.VertexDatatype = std::make_unique<std::vector<int>>(std::vector<int>());
            appInfo.BlendEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.DepthTestEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.DepthWriteEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.SkyboxEnable = std::make_unique<std::vector<bool>>(std::vector<bool>());
            appInfo.ComputeShader = std::make_unique<std::vector<std::string>>(std::vector<std::string>());


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
                        //pipeline_names.push_back(pipeline["resource_graphics_pipeline_name"] ? pipeline["resource_graphics_pipeline_name"].as<std::string>() : "DefaultPipeline");

                        std::string pipelineName = pipeline["resource_graphics_pipeline_name"] ? pipeline["resource_graphics_pipeline_name"].as<std::string>() : "DefaultPipeline"; //not used

                        appInfo.VertexShader->push_back(pipeline["resource_graphics_pipeline_vertexshader_name"] ? pipeline["resource_graphics_pipeline_vertexshader_name"].as<std::string>() : "default.vert");
                        appInfo.FragmentShader->push_back(pipeline["resource_graphics_pipeline_fragmentshader_name"] ? pipeline["resource_graphics_pipeline_fragmentshader_name"].as<std::string>() : "default.frag");
                        appInfo.RenderPassShadowmap->push_back(pipeline["renderpasses_shadowmap"] ? pipeline["renderpasses_shadowmap"].as<bool>() : false);
                        appInfo.Subpass->push_back(pipeline["subpasses_subpass_id"] ? pipeline["subpasses_subpass_id"].as<int>() : 0);
                        appInfo.VertexDatatype->push_back(pipeline["resource_graphics_pipeline_vertexdatatype"] ? pipeline["resource_graphics_pipeline_vertexdatatype"].as<int>() : 2);
                        appInfo.BlendEnable->push_back(pipeline["resource_graphics_pipeline_blend_enable"] ? pipeline["resource_graphics_pipeline_blend_enable"].as<bool>() : false);
                        appInfo.DepthTestEnable->push_back(pipeline["resource_graphics_pipeline_depth_test_enable"] ? pipeline["resource_graphics_pipeline_depth_test_enable"].as<bool>() : true);
                        appInfo.DepthWriteEnable->push_back(pipeline["resource_graphics_pipeline_depth_write_enable"] ? pipeline["resource_graphics_pipeline_depth_write_enable"].as<bool>() : true);
                        appInfo.SkyboxEnable->push_back(pipeline["resource_graphics_pipeline_skybox"] ? pipeline["resource_graphics_pipeline_skybox"].as<bool>() : false);
    
                    }
                }

                if (resource["ComputeShaders"]) {
                    for (const auto& computeShader : resource["ComputeShaders"]) {
                        appInfo.ComputeShader->push_back(computeShader["resource_computeshader_name"].as<std::string>());
                    }
                }

            }


            bShadowmapAttachmentDepthLight = config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"] ? config["ShadowmapRenderpassAttachments"]["ShadowmapRenderpass_attachment_depth_light"].as<bool>() : false;
            bMainSceneAttachmentDepthLight = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_light"].as<bool>() : false;
            bMainSceneAttachmentDepthCamera = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_depth_camera"].as<bool>()  : false;
            bMainSceneAttachmentColorResovle = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_resovle"].as<bool>()  : false;
            bMainSceneAttachmentColorPresent = config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"] ? config["MainSceneRenderpassAttachments"]["mainsceneRenderpass_attachment_color_present"].as<bool>()  : true; //need al least one subpass with at least one color attachment


            bEnableShadowmapRenderpassSubpassShadowmap = config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"] ? config["ShadowmapRenderpassSubpasses"]["shadowmapRenderpass_subpasses_shadowmap"].as<bool>() : false;
            bEnableMainSceneRenderpassSubpassShadowmap = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_shadowmap"].as<bool>() : false;
            bEnableMainSceneRenderpassSubpassDraw = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_draw"].as<bool>() : true; //need at least one subpass, even for compute sample
            bEnableMainSceneRenderpassSubpassObserve = config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"] ? config["MainSceneRenderpassSubpasses"]["mainsceneRenderpass_subpasses_observe"].as<bool>() : false;

            int max_object_id = -1;
            if (config["Objects"]) {
                for (const auto& obj : config["Objects"]) {
                    int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
                    max_object_id = (object_id > max_object_id) ? object_id : max_object_id;
                }
            }
            customObjectCount = ((max_object_id+1) < config["Objects"].size()) ? (max_object_id+1) : config["Objects"].size();
            appInfo.Objects.resize(customObjectCount);
            if (config["Objects"]) {
                //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
                for (const auto& obj : config["Objects"]) {
                    int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
                    appInfo.Objects[object_id].loadFromYaml(obj);
                }
            }

            int max_textbox_id = -1;
            if (config["Textboxes"]) {
                for (const auto& tb : config["Textboxes"]) {
                    int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
                    max_textbox_id = (textbox_id > max_textbox_id) ? textbox_id : max_textbox_id;
                }
            }
            customTextboxCount = ((max_textbox_id+1) < config["Textboxes"].size()) ? (max_textbox_id+1) : config["Textboxes"].size();
            appInfo.Textboxes.resize(customTextboxCount);
            if (config["Textboxes"]) {
                for (const auto& tb : config["Textboxes"]) {
                    int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
                    appInfo.Textboxes[textbox_id].loadFromYaml(tb);
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

        bool GetShadowmapAttachmentDepthLight() { return bShadowmapAttachmentDepthLight; }
        bool GetMainSceneAttachmentDepthLight() { return bMainSceneAttachmentDepthLight; }
        bool GetMainSceneAttachmentDepthCamera() { return bMainSceneAttachmentDepthCamera; }
        bool GetMainSceneAttachmentColorResovle() { return bMainSceneAttachmentColorResovle; }
        bool GetMainSceneAttachmentColorPresent() { return bMainSceneAttachmentColorPresent; }

        bool GetEnableShadowmapRenderpassSubpassShadowmap() { return bEnableShadowmapRenderpassSubpassShadowmap; }
        bool GetEnableMainSceneRenderpassSubpassShadowmap() { return bEnableMainSceneRenderpassSubpassShadowmap; }
        bool GetEnableMainSceneRenderpassSubpassDraw() { return bEnableMainSceneRenderpassSubpassDraw; }
        bool GetEnableMainSceneRenderpassSubpassObserve() { return bEnableMainSceneRenderpassSubpassObserve; }

        int& GetCustomObjectCount() { return customObjectCount; }
        int& GetCustomTextboxCount() { return customTextboxCount; }
        int& GetCustomLightCount() { return customLightCount; }

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

        bool bShadowmapAttachmentDepthLight;
        bool bMainSceneAttachmentDepthLight;
        bool bMainSceneAttachmentDepthCamera;
        bool bMainSceneAttachmentColorResovle;
        bool bMainSceneAttachmentColorPresent;

        bool bEnableShadowmapRenderpassSubpassShadowmap;
        bool bEnableMainSceneRenderpassSubpassShadowmap;
        bool bEnableMainSceneRenderpassSubpassDraw;
        bool bEnableMainSceneRenderpassSubpassObserve;

        int customObjectCount;
        int customTextboxCount;
        int customLightCount;
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}
