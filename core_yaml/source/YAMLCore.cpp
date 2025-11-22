#include "YAMLCore.h"
#include "Foundation.h"
#include <iostream>
#include "TypeAppInfo.impl.h"

namespace LEYAML{

void YAMLCore::ReadExampleYAMLFile(const std::string& examplename) {
    std::string fullYamlName = YAML_PATH + examplename + "/" + examplename + ".yaml";
    std::cout<<"Loading Example YAML file: "<<fullYamlName<<std::endl;
    try{
        yamlNode = YAML::LoadFile(fullYamlName);
    } catch (...){
        std::cout<<"Error loading yaml file"<<std::endl;
        return;
    }


    for(const auto& control : (yamlNode)["Controls"])
        if (control["UIContainer"]) appInfo.ControlUIContainer.loadFromYaml(control["UIContainer"]);

    if(yamlNode["Features"]) appInfo.Feature.loadFromYaml(yamlNode["Features"]);

    auto uniformsNode = yamlNode["Uniforms"];
    if(uniformsNode["Graphics"]) appInfo.Uniform.loadGraphicsFromYaml(uniformsNode["Graphics"]);
    if(uniformsNode["Compute"]) appInfo.Uniform.loadComputeFromYaml(uniformsNode["Compute"]);

    if (uniformsNode["GraphicsTextureImageSamplers"]) {
        int samplerCount = 0;
        for(const auto& samplerNode : uniformsNode["GraphicsTextureImageSamplers"]) samplerCount++;
        appInfo.Samplers.resize(samplerCount);
        samplerCount = 0;
        for (const auto& samplerNode : uniformsNode["GraphicsTextureImageSamplers"]) appInfo.Samplers[samplerCount++].loadFromYaml(samplerNode);
    }

    for (const auto& resource : yamlNode["Resources"]) {
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

    if(yamlNode["Attachments"]) appInfo.Attachment.loadFromYaml(yamlNode["Attachments"]);
    if(yamlNode["Subpasses"]) appInfo.Subpass.loadFromYaml(yamlNode["Subpasses"]);


    int max_object_id = -1;
    if (yamlNode["Objects"]) {
        for (const auto& obj : yamlNode["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            max_object_id = (object_id > max_object_id) ? object_id : max_object_id;
        }
    }
    int customObjectCount = ((max_object_id+1) < yamlNode["Objects"].size()) ? (max_object_id+1) : yamlNode["Objects"].size();
    appInfo.Objects.resize(customObjectCount);
    if (yamlNode["Objects"]) {
        //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
        for (const auto& obj : yamlNode["Objects"]) {
            int object_id = obj["object_id"] ? obj["object_id"].as<int>() : 0;
            appInfo.Objects[object_id].loadFromYaml(obj);
        }
    }

    int max_textbox_id = -1;
    if (yamlNode["Textboxes"]) {
        for (const auto& tb : yamlNode["Textboxes"]) {
            int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
            max_textbox_id = (textbox_id > max_textbox_id) ? textbox_id : max_textbox_id;
        }
    }
    int customTextboxCount = ((max_textbox_id+1) < yamlNode["Textboxes"].size()) ? (max_textbox_id+1) : yamlNode["Textboxes"].size();
    appInfo.Textboxes.resize(customTextboxCount);
    if (yamlNode["Textboxes"]) {
        for (const auto& tb : yamlNode["Textboxes"]) {
            int textbox_id = tb["textbox_id"] ? tb["textbox_id"].as<int>() : 0;
            appInfo.Textboxes[textbox_id].loadFromYaml(tb);
        }
    }

    int max_light_id = -1;
    if (yamlNode["Lights"]) {
        for (const auto& light : yamlNode["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            max_light_id = (light_id > max_light_id) ? light_id : max_light_id;
        }
    }
    int customLightCount = ((max_light_id+1) < yamlNode["Lights"].size())?(max_light_id+1):yamlNode["Lights"].size();
    appInfo.Lights.resize(customLightCount);
    if (yamlNode["Lights"]) {
        for (const auto& light : yamlNode["Lights"]) {
            int light_id = light["light_id"] ? light["light_id"].as<int>() : 0;
            appInfo.Lights[light_id].loadFromYaml(light);
        }
    }

    if (yamlNode["MainCamera"]) appInfo.MainCamera.loadFromYaml(yamlNode["MainCamera"]);
    if (yamlNode["LightCamera"]) appInfo.LightCamera.loadFromYaml(yamlNode["LightCamera"]);

}

void YAMLCore::ReadMaterialYAMLFile(const std::string& filename){
    std::string fullYamlName = MATERIAL_YAML_PATH + filename + ".yaml";
    std::cout<<"Loading Material YAML file: "<<fullYamlName<<std::endl;

    try{
        yamlNode = YAML::LoadFile(fullYamlName);
    } catch (...){
        std::cout<<"Error loading yaml file"<<std::endl;
        return;
    }

    int max_material_id = -1;
    if (yamlNode["Materials"]) {
        for (const auto& mat : yamlNode["Materials"]) {
            int material_id = mat["material_id"] ? mat["material_id"].as<int>() : 0;
            max_material_id = (material_id > max_material_id) ? material_id : max_material_id;
        }
    }
    int customMaterialCount = ((max_material_id+1) < yamlNode["Materials"].size()) ? (max_material_id+1) : yamlNode["Materials"].size();
    //std::cout<<"Detected "<<customMaterialCount<<" custom materials in the YAML file."<<std::endl;
    appInfo.Materials.resize(customMaterialCount);
    if (yamlNode["Materials"]) {
        //std::cerr << "No 'Objects' key found in the YAML file!" << std::endl;
        for (const auto& mat : yamlNode["Materials"]) {
            int material_id = mat["material_id"] ? mat["material_id"].as<int>() : 0;
            appInfo.Materials[material_id].loadFromYaml(mat);
        }
    }


    // for(int i = 0; i < appInfo.Materials.size(); i++){
    //     std::cout << "Material ID " << i << ": Name = " << appInfo.Materials[i].material_name << std::endl;
    //     std::cout << "material_id: " << appInfo.Materials[i].material_id << std::endl;
    //     std::cout << "albedo: " << appInfo.Materials[i].albedo[0] << ", " << appInfo.Materials[i].albedo[1] << ", " << appInfo.Materials[i].albedo[2] << std::endl;
    //     std::cout << "emissionColor: " << appInfo.Materials[i].emissionColor[0] << ", " << appInfo.Materials[i].emissionColor[1] << ", " << appInfo.Materials[i].emissionColor[2] << std::endl;
    //     std::cout << "transmissionColor: " << appInfo.Materials[i].transmissionColor[0] << ", " << appInfo.Materials[i].transmissionColor[1] << ", " << appInfo.Materials[i].transmissionColor[2] << std::endl;
    //     std::cout << "metallic: " << appInfo.Materials[i].metallic << std::endl;
    //     std::cout << "roughness: " << appInfo.Materials[i].roughness << std::endl;
    //     std::cout << "alpha: " << appInfo.Materials[i].alpha << std::endl;
    //     std::cout << "emissionStrength: " << appInfo.Materials[i].emissionStrength << std::endl;
    //     std::cout << "reflectance: " << appInfo.Materials[i].reflectance << std::endl;
    //     std::cout << "specular: " << appInfo.Materials[i].specular << std::endl;
    //     std::cout << "ior: " << appInfo.Materials[i].ior << std::endl;
    //     std::cout << "transmission: " << appInfo.Materials[i].transmission << std::endl << std::endl;
    // }


}

}//end of namespace