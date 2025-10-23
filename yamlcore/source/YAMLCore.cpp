#include "../include/YAMLCore.h"
//#include "Foundation.h"
//#include <windows.h>
//#include <iostream>
//#include "Enum.h"

namespace LEYAML{

void YAMLCore::ReadYAMLFile(const std::string& filename) {
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
                appInfo.Font.loadFromYaml(font);
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
            int graphicsPipelineCount = 0;
            for (const auto& pipeline : resource["Pipelines"]) graphicsPipelineCount++;

            //std::cout<<"YAMLCore: graphicsPipelineCount="<<graphicsPipelineCount<<std::endl;
            appInfo.GraphicsPipelines.resize(graphicsPipelineCount);
            graphicsPipelineCount = 0;

            for (const auto& pipeline : resource["Pipelines"]) {
                appInfo.GraphicsPipelines[graphicsPipelineCount].loadFromYaml(pipeline);

                graphicsPipelineCount++;
            }
        }

        
        if (resource["ComputeShaders"]) {
            int computePipelineCount = 0;
            for (const auto& pipeline : resource["ComputeShaders"]) computePipelineCount++;

            //std::cout<<"YAMLCore: computePipelineCount="<<computePipelineCount<<std::endl;
            appInfo.ComputePipelines.resize(computePipelineCount);
            computePipelineCount = 0;

            for (const auto& computeShader : resource["ComputeShaders"]) {
                appInfo.ComputePipelines[computePipelineCount].loadFromYaml(computeShader);

                computePipelineCount++;
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

    int max_light_id = -1;
    if (config["Lights"]) {
        for (const auto& light : config["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            max_light_id = (light_id > max_light_id) ? light_id : max_light_id;
        }
    }
    customLightCount = ((max_light_id+1) < config["Lights"].size())?(max_light_id+1):config["Lights"].size();
    appInfo.Lights.resize(customLightCount);
    if (config["Lights"]) {
        for (const auto& light : config["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            appInfo.Lights[light_id].loadFromYaml(light);
        }
    }

    if (config["MainCamera"]) appInfo.MainCamera.loadFromYaml(config["MainCamera"]);
    if (config["LightCamera"]) appInfo.LightCamera.loadFromYaml(config["LightCamera"]);

}//end of ReadYAMLFile()

}//end of namespace