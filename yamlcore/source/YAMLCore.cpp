#include "YAMLCore.h"
#include "Foundation.h"
#include <iostream>

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


    for(const auto& control : (config)["Controls"])
        if (control["UIContainer"]) appInfo.ControlUIContainer.loadFromYaml(control["UIContainer"]);

    if(config["Features"]) appInfo.Feature.loadFromYaml(config["Features"]);

    auto uniformsNode = config["Uniforms"];
    if(uniformsNode["Graphics"]) appInfo.Uniform.loadGraphicsFromYaml(uniformsNode["Graphics"]);
    if(uniformsNode["Compute"]) appInfo.Uniform.loadComputeFromYaml(uniformsNode["Compute"]);

    if (uniformsNode["GraphicsTextureImageSamplers"]) {
        int samplerCount = 0;
        for(const auto& samplerNode : uniformsNode["GraphicsTextureImageSamplers"]) samplerCount++;
        appInfo.Samplers.resize(samplerCount);
        samplerCount = 0;
        for (const auto& samplerNode : uniformsNode["GraphicsTextureImageSamplers"]) appInfo.Samplers[samplerCount++].loadFromYaml(samplerNode);
    }

    for (const auto& resource : config["Resources"]) {
        if (resource["Fonts"]) {
            for (const auto& font : resource["Fonts"]) {
                appInfo.Font.loadFromYaml(font);
            }
        }

        if (resource["Models"]) {
            int modelCount = 0;
            for (const auto& model : resource["Models"]) modelCount++;
            appInfo.Models.resize(modelCount);
            modelCount=0;
            for (const auto& model : resource["Models"]) appInfo.Models[modelCount++].loadFromYaml(model);
        }

        if (resource["Textures"]) {
            int textureCount = 0;
            for (const auto& texture : resource["Textures"]) textureCount++;
            appInfo.Textures.resize(textureCount);
            textureCount = 0;
            for (const auto& texture : resource["Textures"]) appInfo.Textures[textureCount++].loadFromYaml(texture);
        }

        if (resource["Pipelines"]) {
            int graphicsPipelineCount = 0;
            for (const auto& pipeline : resource["Pipelines"]) graphicsPipelineCount++;
            appInfo.GraphicsPipelines.resize(graphicsPipelineCount);
            graphicsPipelineCount = 0;
            for (const auto& pipeline : resource["Pipelines"]) appInfo.GraphicsPipelines[graphicsPipelineCount++].loadFromYaml(pipeline);
        }
        
        if (resource["ComputeShaders"]) {
            int computePipelineCount = 0;
            for (const auto& pipeline : resource["ComputeShaders"]) computePipelineCount++;
            appInfo.ComputePipelines.resize(computePipelineCount);
            computePipelineCount = 0;
            for (const auto& computeShader : resource["ComputeShaders"]) appInfo.ComputePipelines[computePipelineCount++].loadFromYaml(computeShader);
        }
    }

    if(config["Attachments"]) appInfo.Attachment.loadFromYaml(config["Attachments"]);
    if(config["Subpasses"]) appInfo.Subpass.loadFromYaml(config["Subpasses"]);


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