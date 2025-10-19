#pragma once
#include <vulkan/vulkan.h>

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

    // ---------------------
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

struct AppInfo{
    FeatureConfig Feature;
    ControlUIContainerConfig ControlUIContainer;
    UniformConfig Uniform;

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