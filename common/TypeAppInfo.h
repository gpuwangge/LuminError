#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#ifdef WIN32
    #define YAML_CPP_STATIC_DEFINE //to disable lots of yaml warnings
#endif
#include "../thirdParty/yaml-cpp/yaml.h"

 /******************
* Utility Functions
******************/
template <typename T>
T getOrDefault(const YAML::Node& node, const std::string& key, const T& defaultValue) {
    if (node[key]) {
        return node[key].as<T>();
    }
    return defaultValue;
}

/*************
 * APP INFO
 *******/
struct FeatureConfig {
    bool b_feature_graphics_48pbt = false;
    bool b_feature_graphics_push_constant = false;
    bool b_feature_graphics_global_blend = false;
    bool b_feature_graphics_rainbow_mipmap = false;
    //int feature_graphics_pipeline_skybox_id = -1;
    int feature_graphics_observe_attachment_id = -1;
    bool feature_graphics_show_performance_control = true;
    bool feature_graphics_show_all_metric_controls = true;

    bool feature_graphics_enable_controls = false; //this is not read from yaml

    void loadFromYaml(const YAML::Node& node) {
        b_feature_graphics_48pbt                    = getOrDefault(node, "feature_graphics_48pbt", false);
        b_feature_graphics_push_constant            = getOrDefault(node, "feature_graphics_push_constant", false);
        b_feature_graphics_global_blend                    = getOrDefault(node, "feature_graphics_global_blend", false);
        b_feature_graphics_rainbow_mipmap           = getOrDefault(node, "feature_graphics_rainbow_mipmap", false);
        //feature_graphics_pipeline_skybox_id         = getOrDefault(node, "feature_graphics_pipeline_skybox_id", -1);
        feature_graphics_observe_attachment_id      = getOrDefault(node, "feature_graphics_observe_attachment_id", -1);
        feature_graphics_show_performance_control   = getOrDefault(node, "feature_graphics_show_performance_control", false);
        feature_graphics_show_all_metric_controls   = getOrDefault(node, "feature_graphics_show_all_metric_controls", false);
        
        feature_graphics_enable_controls = feature_graphics_show_performance_control || feature_graphics_show_all_metric_controls;
    }
};

struct UniformConfig {
    bool b_uniform_graphics_custom = false;
    bool b_uniform_graphics_object_mvp = false;
    bool b_uniform_graphics_text_mvp = false;
    bool b_uniform_graphics_object_vp = false;
    bool b_uniform_graphics_lighting = false;
    bool b_uniform_graphics_depth_image_sampler = false;
    bool b_uniform_graphics_lightdepth_image_sampler = false;
    bool b_uniform_graphics_lightdepth_image_sampler_hardware = false;
    struct GraphicsCustomInfo {
        VkDeviceSize Size = 0;
        VkDescriptorSetLayoutBinding Binding{};
    } GraphicsCustom;

    bool b_uniform_compute_custom = false;
    bool b_uniform_compute_storage = false;
    bool b_uniform_compute_swapchain_storage = false;
    bool b_uniform_compute_texture_storage = false;
    struct ComputeCustomInfo {
        VkDeviceSize Size = 0;
        VkDescriptorSetLayoutBinding Binding{};
    } ComputeCustom;
    struct ComputeStorageBufferInfo {
        VkDeviceSize Size = 0;
        VkBufferUsageFlags Usage = 0;
    } ComputeStorageBuffer;

    void loadGraphicsFromYaml(const YAML::Node& node) {
        b_uniform_graphics_custom                     = getOrDefault(node, "uniform_graphics_custom", false);
        b_uniform_graphics_object_mvp                        = getOrDefault(node, "uniform_graphics_object_mvp", false);
        b_uniform_graphics_text_mvp                   = getOrDefault(node, "uniform_graphics_text_mvp", false);
        b_uniform_graphics_object_vp                         = getOrDefault(node, "uniform_graphics_object_vp", false);
        b_uniform_graphics_lighting                   = getOrDefault(node, "uniform_graphics_lighting", false);
        b_uniform_graphics_depth_image_sampler        = getOrDefault(node, "uniform_graphics_depth_image_sampler", false);
        b_uniform_graphics_lightdepth_image_sampler   = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler", false);
        b_uniform_graphics_lightdepth_image_sampler_hardware = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler_hardware", false);

        // std::cout<<"b_uniform_graphics_custom "<<b_uniform_graphics_custom<<std::endl;
        // std::cout<<"b_uniform_graphics_object_mvp "<<b_uniform_graphics_object_mvp<<std::endl;
        // std::cout<<"b_uniform_graphics_text_mvp "<<b_uniform_graphics_text_mvp<<std::endl;
        // std::cout<<"b_uniform_graphics_object_vp "<<b_uniform_graphics_object_vp<<std::endl;
        // std::cout<<"b_uniform_graphics_lighting "<<b_uniform_graphics_lighting<<std::endl;
        // std::cout<<"b_uniform_graphics_depth_image_sampler "<<b_uniform_graphics_depth_image_sampler<<std::endl;
        // std::cout<<"b_uniform_graphics_lightdepth_image_sampler "<<b_uniform_graphics_lightdepth_image_sampler<<std::endl;
        // std::cout<<"b_uniform_graphics_lightdepth_image_sampler_hardware "<<b_uniform_graphics_lightdepth_image_sampler_hardware<<std::endl;

    }

    void loadComputeFromYaml(const YAML::Node& node) {
        b_uniform_compute_custom                       = getOrDefault(node, "uniform_compute_custom", false);
        b_uniform_compute_storage                      = getOrDefault(node, "uniform_compute_storage", false);
        b_uniform_compute_swapchain_storage           = getOrDefault(node, "uniform_compute_swapchain_storage", false);
        b_uniform_compute_texture_storage             = getOrDefault(node, "uniform_compute_texture_storage", false);
    }
};

struct ControlUIContainerConfig {
    std::vector<int> resource_texture_id_list_box;
    int resource_model_id_box = 0;
    int resource_default_graphics_pipeline_id_box = 0;
    int resource_model_id_text = 0;
    int resource_default_graphics_pipeline_id_text = 0;

    void loadFromYaml(const YAML::Node& node) {
        resource_texture_id_list_box            = getOrDefault(node, "resource_texture_id_list_box", std::vector<int>{0});
        resource_model_id_box                   = getOrDefault(node, "resource_model_id_box", 0);
        resource_default_graphics_pipeline_id_box= getOrDefault(node, "resource_default_graphics_pipeline_id_box", 0);
        resource_model_id_text                  = getOrDefault(node, "resource_model_id_text", 0);
        resource_default_graphics_pipeline_id_text= getOrDefault(node, "resource_default_graphics_pipeline_id_text", 0);
    }
};

struct ObjectConfig {
    std::string object_name = "Default";
    int object_id = 0;
    int object_resource_model_id = 0;
    std::vector<int> object_resource_texture_id_list = std::vector<int>(1, 0);
    int object_resource_default_graphics_pipeline_id = 0;
    bool object_bSticker = false;
    float object_scale = 1.0f;
    std::vector<float> object_scale_3 = std::vector<float>(3, 1);
    std::vector<float> object_position = std::vector<float>(3, 0);
    std::vector<float> object_rotation = std::vector<float>(3, 0);
    std::vector<float> object_velocity = std::vector<float>(3, 0);
    std::vector<float> object_angular_velocity = std::vector<float>(3, 0);

    void loadFromYaml(const YAML::Node& node) {
        object_name                                     = getOrDefault(node, "object_name", std::string{"Default"});
        object_id                                       = getOrDefault(node, "object_id", 0);
        object_resource_model_id                        = getOrDefault(node, "resource_model_id", 0);
        object_resource_texture_id_list                 = getOrDefault(node, "resource_texture_id_list", std::vector<int>{0});
        object_resource_default_graphics_pipeline_id    = getOrDefault(node, "resource_default_graphics_pipeline_id", 0);
        object_bSticker                                 = getOrDefault(node, "object_sticker", false);
        object_scale                                    = getOrDefault(node, "object_scale", 1.0f);
        object_scale_3                                  = getOrDefault(node, "object_scale_3", std::vector<float>(3, 1.0f));
        object_position                                 = getOrDefault(node, "object_position", std::vector<float>(3, 0.0f));
        object_rotation                                 = getOrDefault(node, "object_rotation", std::vector<float>(3, 0.0f));
        object_velocity                                 = getOrDefault(node, "object_velocity", std::vector<float>(3, 0.0f));
        object_angular_velocity                         = getOrDefault(node, "object_angular_velocity", std::vector<float>(3, 0.0f));
    }
};


struct AppInfo{
    FeatureConfig Feature;
    ControlUIContainerConfig ControlUIContainer;
    UniformConfig Uniform;
    std::vector<ObjectConfig> objects;

    std::unique_ptr<std::vector<std::string>> VertexShader;
    std::unique_ptr<std::vector<std::string>> FragmentShader;
    std::unique_ptr<std::vector<bool>> RenderPassShadowmap;
    std::unique_ptr<std::vector<int>> Subpass;
    std::unique_ptr<std::vector<int>> VertexDatatype;
    std::unique_ptr<std::vector<bool>> BlendEnable;
    std::unique_ptr<std::vector<bool>> DepthTestEnable;
    std::unique_ptr<std::vector<bool>> DepthWriteEnable;
    std::unique_ptr<std::vector<bool>> SkyboxEnable;
    std::unique_ptr<std::vector<std::string>> ComputeShader;
    //RenderModes RenderMode = RenderModes::GRAPHICS;
    int RenderMode = 0;
};
