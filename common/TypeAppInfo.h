#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <array>

namespace YAML{
    class Node;
}

 /******************
* Utility Functions
******************/
template <typename T>
T getOrDefault(const YAML::Node& node, const std::string& key, const T& defaultValue);

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
    bool b_uniform_graphics_global = false;
    bool b_uniform_graphics_object_dynamic = false;
    bool b_uniform_graphics_text_dynamic = false;
    bool b_uniform_graphics_lighting = false;
    bool b_uniform_graphics_custom = false;
    bool b_uniform_graphics_depth_image_sampler = false;
    bool b_uniform_graphics_lightdepth_image_sampler = false;
    bool b_uniform_graphics_lightdepth_image_sampler_hardware = false;
    struct GraphicsCustomInfo {
        VkDeviceSize Size = 0;
        VkDescriptorSetLayoutBinding Binding{};
    } GraphicsCustom;

    bool b_uniform_compute_global = false;
    bool b_storage_compute_windowswap = false;
    bool b_storage_compute_material = false;
    bool b_storage_compute_triangle_vertex = false;
    bool b_storage_compute_triangle_index = false;
    bool b_storage_compute_bvhnode = false;
    bool b_storage_compute_sphere = false;
    bool b_uniform_compute_custom = false;
    bool b_storage_compute_customswap = false;

    bool b_uniform_compute_swapchain_storage = false;
    bool b_uniform_compute_texture_storage = false;
    struct ComputeCustomInfo {
        VkDeviceSize Size = 0;
        VkDescriptorSetLayoutBinding Binding{};
    } ComputeCustom;
    struct StructComputeStorageBufferInfo {
        VkDeviceSize Size = 0;
        VkBufferUsageFlags Usage = 0;
    } ComputeStorageBufferInfo_CustomSwap;

    void loadGraphicsFromYaml(const YAML::Node& node) {
        b_uniform_graphics_global                   = getOrDefault(node, "uniform_graphics_global", false);
        b_uniform_graphics_object_dynamic                        = getOrDefault(node, "uniform_graphics_object_dynamic", false);
        b_uniform_graphics_text_dynamic                   = getOrDefault(node, "uniform_graphics_text_dynamic", false);
        b_uniform_graphics_lighting                   = getOrDefault(node, "uniform_graphics_lighting", false);
        b_uniform_graphics_custom                     = getOrDefault(node, "uniform_graphics_custom", false);
        b_uniform_graphics_depth_image_sampler        = getOrDefault(node, "uniform_graphics_depth_image_sampler", false);
        b_uniform_graphics_lightdepth_image_sampler   = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler", false);
        b_uniform_graphics_lightdepth_image_sampler_hardware = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler_hardware", false);
    }

    void loadComputeFromYaml(const YAML::Node& node) {
        b_uniform_compute_global                      = getOrDefault(node, "uniform_compute_global", false);

        //b_uniform_compute_storage                      = getOrDefault(node, "uniform_compute_storage", false);
        //b_uniform_compute_custom                       = getOrDefault(node, "uniform_compute_custom", false);

        b_storage_compute_windowswap                        = getOrDefault(node, "storage_compute_windowswap", false);
        b_storage_compute_material                          = getOrDefault(node, "storage_compute_material", false);
        b_storage_compute_triangle_vertex                   = getOrDefault(node, "storage_compute_triangle_vertex", false);
        b_storage_compute_triangle_index                    = getOrDefault(node, "storage_compute_triangle_index", false);
        b_storage_compute_bvhnode                           = getOrDefault(node, "storage_compute_bvhnode", false);
        b_storage_compute_sphere                            = getOrDefault(node, "storage_compute_sphere", false);
        b_uniform_compute_custom                            = getOrDefault(node, "uniform_compute_custom", false);
        b_storage_compute_customswap                        = getOrDefault(node, "storage_compute_customswap", false);

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

struct TextConfig {
    int textbox_id = 0;
    std::string textbox_name = "Default";
    std::vector<float> textbox_position = std::vector<float>(3,0);
    std::vector<float> textbox_rotation = std::vector<float>(3,0);
    bool textbox_bSticker = false;
    float textbox_scale = 1.0f;
    std::vector<float> textbox_color = std::vector<float>(4,1.0f);
    int textbox_resource_model_id = 0;
    //std::vector<int> resource_text_id_list;
    std::string textbox_text_content = "";
    std::vector<float> textbox_text_color = std::vector<float>(4,1.0f);
    int textbox_resource_default_graphics_pipeline_id = 0;

    void loadFromYaml(const YAML::Node& node) {
        textbox_id                                     = getOrDefault(node, "textbox_id", 0);
        textbox_name                                   = getOrDefault(node, "textbox_name", std::string{"Default"});
        textbox_position                               = getOrDefault(node, "textbox_position", std::vector<float>(3, 0.0f));
        textbox_rotation                               = getOrDefault(node, "textbox_rotation", std::vector<float>(3, 0.0f));
        textbox_bSticker                               = getOrDefault(node, "textbox_sticker", false);
        textbox_scale                                  = getOrDefault(node, "textbox_scale", 1.0f);
        textbox_color                                  = getOrDefault(node, "textbox_color", std::vector<float>(4, 1.0f));
        textbox_resource_model_id                      = getOrDefault(node, "resource_model_id", 0);
        textbox_text_content                           = getOrDefault(node, "textbox_text_content", std::string{""});
        textbox_text_color                             = getOrDefault(node, "textbox_text_color", std::vector<float>(4, 1.0f));
        textbox_resource_default_graphics_pipeline_id  = getOrDefault(node, "resource_default_graphics_pipeline_id", 0);
    }
};

struct LightConfig{
    int light_id = 0;
    std::string light_name = "Default";
    std::vector<float> light_position = std::vector<float>(3,0);
    std::vector<float> light_intensity = std::vector<float>(4,0);
    std::vector<float> light_color = std::vector<float>(3,1.0f);
    std::vector<float> light_spotAngle = std::vector<float>(2, 180.0f); //the default value is [180,180] degrees which sets the light to point light instead of spot light

    void loadFromYaml(const YAML::Node& node) {
        light_id                                       = getOrDefault(node, "light_id", 0);
        light_name                                     = getOrDefault(node, "light_name", std::string{"Default"});
        light_position                                 = getOrDefault(node, "light_position", std::vector<float>(3, 0.0f));
        light_intensity                                = getOrDefault(node, "light_intensity", std::vector<float>(4, 0.0f));
        light_color                                    = getOrDefault(node, "light_color", std::vector<float>(3, 1.0f));
        light_spotAngle                                = getOrDefault(node, "light_spot", std::vector<float>(2, 180.0f));
    }
};

struct CameraConfig{
    int camera_mode = 0;
    std::array<float,3> camera_position{};
    std::array<float,3> camera_rotation{};
    int object_id_target = 1;
    std::array<float,2> camera_z{};
    bool camera_projection_enable_orthographic = false;
    float camera_projection_perspective_fov = 60;
    float camera_projection_orthographic_width = 8;
    float camera_projection_orthographic_height = 8;
    float camera_keyboard_sensitive = 3;
    float camera_mouse_sensitive = 60;

    void loadFromYaml(const YAML::Node& node) {
        camera_mode                                    = getOrDefault(node, "camera_mode", 0);
        camera_position                                = getOrDefault(node, "camera_position", std::array<float, 3>{0.0f, 0.0f, 0.0f});
        camera_rotation                                = getOrDefault(node, "camera_rotation", std::array<float, 3>{0.0f, 0.0f, 0.0f});
        object_id_target                               = getOrDefault(node, "object_id_target", 1);
        camera_z                                       = getOrDefault(node, "camera_z", std::array<float, 2>{0.0f, 0.0f});
        camera_projection_enable_orthographic          = getOrDefault(node, "camera_projection_enable_orthographic", false);
        camera_projection_perspective_fov              = getOrDefault(node, "camera_projection_perspective_fov", 60.0f);
        camera_projection_orthographic_width           = getOrDefault(node, "camera_projection_orthographic_width", 8.0f);
        camera_projection_orthographic_height          = getOrDefault(node, "camera_projection_orthographic_height", 8.0f);
        camera_keyboard_sensitive                      = getOrDefault(node, "camera_keyboard_sensitive", 3.0f);
        camera_mouse_sensitive                         = getOrDefault(node, "camera_mouse_sensitive", 60.0f);
    }
};

struct GraphicsPipelineConfig{
    std::string graphics_pipeline_name = "Default";
    std::string graphics_pipeline_vertexshader_name = "Default";
    std::string graphics_pipeline_fragmentshader_name = "Default";
    bool graphics_pipeline_renderpasses_shadowmap = 0;
    int graphics_pipeline_subpasses_subpass_id = 0;
    int graphics_pipeline_vertexdatatype = 2;
    bool graphics_pipeline_blend_enable = false;
    bool graphics_pipeline_depth_test_enable = true;
    bool graphics_pipeline_depth_write_enable = true;
    bool graphics_pipeline_skybox = false;

    void loadFromYaml(const YAML::Node& node) {
        graphics_pipeline_name                         = getOrDefault(node, "resource_graphics_pipeline_name", std::string{"Default"});
        graphics_pipeline_vertexshader_name            = getOrDefault(node, "resource_graphics_pipeline_vertexshader_name", std::string{"Default"});
        graphics_pipeline_fragmentshader_name          = getOrDefault(node, "resource_graphics_pipeline_fragmentshader_name", std::string{"Default"});
        graphics_pipeline_renderpasses_shadowmap       = getOrDefault(node, "renderpasses_shadowmap", false);
        graphics_pipeline_subpasses_subpass_id         = getOrDefault(node, "subpasses_subpass_id", 0);
        graphics_pipeline_vertexdatatype               = getOrDefault(node, "resource_graphics_pipeline_vertexdatatype", 2);
        graphics_pipeline_blend_enable                 = getOrDefault(node, "resource_graphics_pipeline_blend_enable", false);
        graphics_pipeline_depth_test_enable            = getOrDefault(node, "resource_graphics_pipeline_depth_test_enable", true);
        graphics_pipeline_depth_write_enable           = getOrDefault(node, "resource_graphics_pipeline_depth_write_enable", true);
        graphics_pipeline_skybox                       = getOrDefault(node, "resource_graphics_pipeline_skybox", false);
    }
};

struct ComputePipelineConfig{
    std::string compute_pipeline_computeshader_name = "Default";

    void loadFromYaml(const YAML::Node& node) {
        compute_pipeline_computeshader_name            = getOrDefault(node, "resource_computeshader_name", std::string{"Default"});
    }
};

struct FontConfig{
    std::string font_name = "Default";
    int font_samplerid = 0;
    std::array<int,4> font_outlineColor{};
    std::array<int,4> font_textColor{};
    int font_size = 0;

    void loadFromYaml(const YAML::Node& node) {
        font_name                                      = getOrDefault(node, "resource_font_name", std::string{"Default"});
        font_samplerid                                 = getOrDefault(node, "uniform_sampler_id", 0);
        font_outlineColor                              = getOrDefault(node, "resource_font_outlinecolor", std::array<int,4>{255, 255, 255, 255});
        font_textColor                                 = getOrDefault(node, "resource_font_textcolor", std::array<int,4>{255, 255, 255, 255});
        font_size                                      = getOrDefault(node, "resource_font_size", 0);
    }
};

struct AttachmentConfig{
    bool bShadowmapAttachmentDepthLight = false;
    bool bMainSceneAttachmentDepthLight = false;
    bool bMainSceneAttachmentDepthCamera = false;
    bool bMainSceneAttachmentColorResovle = false;
    bool bMainSceneAttachmentColorPresent = true;

    void loadFromYaml(const YAML::Node& node) {
        bShadowmapAttachmentDepthLight                 = getOrDefault(node, "ShadowmapRenderpass_attachment_depth_light", false);
        bMainSceneAttachmentDepthLight                 = getOrDefault(node, "mainsceneRenderpass_attachment_depth_light", false);
        bMainSceneAttachmentDepthCamera                = getOrDefault(node, "mainsceneRenderpass_attachment_depth_camera", false);
        bMainSceneAttachmentColorResovle               = getOrDefault(node, "mainsceneRenderpass_attachment_color_resovle", false);
        bMainSceneAttachmentColorPresent               = getOrDefault(node, "mainsceneRenderpass_attachment_color_present", true);
    }
};

struct SubpassConfig{
    bool bEnableShadowmapRenderpassSubpassShadowmap = false;
    bool bEnableMainSceneRenderpassSubpassShadowmap = false;
    bool bEnableMainSceneRenderpassSubpassDraw = true;
    bool bEnableMainSceneRenderpassSubpassObserve = false;

    void loadFromYaml(const YAML::Node& node) {
        bEnableShadowmapRenderpassSubpassShadowmap     = getOrDefault(node, "shadowmapRenderpass_subpasses_shadowmap", false);
        bEnableMainSceneRenderpassSubpassShadowmap     = getOrDefault(node, "mainsceneRenderpass_subpasses_shadowmap", false);
        bEnableMainSceneRenderpassSubpassDraw          = getOrDefault(node, "mainsceneRenderpass_subpasses_draw", true);
        bEnableMainSceneRenderpassSubpassObserve       = getOrDefault(node, "mainsceneRenderpass_subpasses_observe", false);
    }
};

struct TextureConfig{
    std::string texture_name;
    int texture_miplevel = 1;
    bool texture_enableCubemap = false;
    int texture_samplerid = 0;

    void loadFromYaml(const YAML::Node& node) {
        texture_name                                   = getOrDefault(node, "resource_texture_name", std::string{"Default"});
        texture_miplevel                               = getOrDefault(node, "resource_texture_miplevels", 1);
        texture_enableCubemap                          = getOrDefault(node, "resource_texture_cubemap", false);
        texture_samplerid                              = getOrDefault(node, "uniform_sampler_id", 0);
    }
};

struct ModelConfig{
    std::string model_names;
    //int model_ids;

    void loadFromYaml(const YAML::Node& node) {
        model_names                                    = getOrDefault(node, "resource_model_name", std::string{""});
        //model_ids                                      = getOrDefault(node, "model_ids", 0);
    }
};

struct SamplerConfig{
    std::string sampler_name;
    int sampler_miplevels;
    std::array<bool,3> sampler_uvwRepeats;

    void loadFromYaml(const YAML::Node& node) {
        sampler_name                                   = getOrDefault(node, "uniform_graphics_texture_image_sampler_name", std::string{"Default"});
        sampler_miplevels                              = getOrDefault(node, "uniform_graphics_texture_image_sampler_miplevel", 0);
        sampler_uvwRepeats                             = getOrDefault(node, "uniform_graphics_texture_image_sampler_uvwrepeat", std::array<bool,3>{true, true, true});
    }
};

struct MaterialConfig {
    std::string material_name;
    int material_id = -1;
    std::array<float, 3> albedo{1.0f, 1.0f, 1.0f};
    std::array<float, 3> emissionColor{0.0f, 0.0f, 0.0f};
    std::array<float, 3> transmissionColor{1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 1.0f;
    float alpha = 1.0f;
    float emissionStrength = 0.0f;
    float reflectance = 0.04f;
    float specular = 0.5f;
    float ior = 1.0f;
    float transmission = 0.0f;

    void loadFromYaml(const YAML::Node& node) {
        material_name = getOrDefault(node, "material_name", std::string{"Default"});
        material_id = getOrDefault(node, "material_id", -1);
        albedo = getOrDefault(node, "albedo", std::array<float, 3>{1.0f, 1.0f, 1.0f});
        emissionColor = getOrDefault(node, "emissionColor", std::array<float, 3>{0.0f, 0.0f, 0.0f});
        transmissionColor = getOrDefault(node, "transmissionColor", std::array<float, 3>{1.0f, 1.0f, 1.0f});
        metallic = getOrDefault(node, "metallic", 0.0f);
        roughness = getOrDefault(node, "roughness", 1.0f);
        alpha = getOrDefault(node, "alpha", 1.0f);
        emissionStrength = getOrDefault(node, "emissionStrength", 0.0f);
        reflectance = getOrDefault(node, "reflectance", 0.04f);
        specular = getOrDefault(node, "specular", 0.5f);
        ior = getOrDefault(node, "ior", 1.0f);
        transmission = getOrDefault(node, "transmission", 0.0f);
    }
};

struct AppInfo{
    std::vector<ObjectConfig> Objects;
    std::vector<TextConfig> Textboxes;
    std::vector<LightConfig> Lights;
    std::vector<MaterialConfig> Materials;

    FontConfig Font;
    std::vector<ModelConfig> Models;
    std::vector<TextureConfig> Textures;
    std::vector<GraphicsPipelineConfig> GraphicsPipelines;
    std::vector<ComputePipelineConfig> ComputePipelines;

    FeatureConfig Feature;
    ControlUIContainerConfig ControlUIContainer;
    UniformConfig Uniform;
    std::vector<SamplerConfig> Samplers;
    
    CameraConfig MainCamera;
    CameraConfig LightCamera;

    AttachmentConfig Attachment;
    SubpassConfig Subpass;

    //RenderModes RenderMode = RenderModes::GRAPHICS;
    int RenderMode = 0;
};
