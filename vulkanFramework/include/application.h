#ifndef H_APPLICATION
#define H_APPLICATION

#include "common.h"
#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
#include "dataBuffer.hpp"
#include "context.h"
#include "graphicsDescriptor.h"
#include "computeDescriptor.h"
#include "renderProcess.h"
#include "shaderManager.h"
#include "renderer.h"
#include "texture.h" //this includes imageManager.h
#include "modelManager.h"
#include "object.h"
#include "textManager.h"
#include "light.h"
#include "timer.h"
#include "controlNode.h"

//Macro to convert the macro value to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef WIN32
    #define YAML_CPP_STATIC_DEFINE //to disable lots of yaml warnings
#endif
#include "../../thirdParty/yaml-cpp/yaml.h"

#ifndef ANDROID
    #ifndef SDL
        #include "..\\..\\glfwFramework\\include\\glfwManager.h"
    #else
        #include "..\\..\\sdlFramework\\include\\sdlManager.h"
    #endif
#endif

//Decide not to use macro
// #define START_GRAPHICS_RECORD(descriptorIdx) { \
//     renderer.StartRecordGraphicsCommandBuffer(renderProcess.graphicsPipeline, renderProcess.graphicsPipelineLayout, \
//     renderProcess.renderPass,   \
//     swapchain.swapChainFramebuffers,swapchain.swapChainExtent,   \
//     descriptors[descriptorIdx].descriptorSets,  \
//     clearValues);}
// #define END_GRAPHICS_RECORD renderer.EndRecordGraphicsCommandBuffer();

//Decide not to use macro
//#define START_COMPUTE_RECORD(descriptorIdx) renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout, descriptors[descriptorIdx].descriptorSets);
//#define END_COMPUTE_RECORD renderer.EndRecordComputeCommandBuffer();

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

inline std::string to_string_prec(double value, int prec = 1) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(prec) << value;
    return oss.str();
}

class CApplication{
public:
    CApplication();
    ~CApplication();

    CLogManager logManager;

    //VkInstance instance;//01
    std::unique_ptr<CInstance> instance{nullptr};

    int windowWidth, windowHeight;
    VkSurfaceKHR surface;//03
    
    //bool framebufferResized = false;
    //bool needWindow = false;

	CSwapchain swapchain;
    CRenderProcess renderProcess;
    CShaderManager shaderManager;
    CGraphicsDescriptorManager graphicsDescriptorManager;
    CComputeDescriptorManager computeDescriptorManager;
    CRenderer renderer;
    CModelManager modelManager;
    CTextureManager textureManager;
    CTextImageManager textImageManager;
    CTextManager textManager;

    int objectCountControl = 0;
    int textboxCountControl = 0;
    int lightCountControl = 0;
    //static int focusObjectId;
    static std::vector<CObject> objects;
    //static std::vector<CTextBox> textBoxes;
    static std::vector<CLight> lights;
    int customObjectSize = 0;
    int customLightsSize = 0;
    int customTextboxSize = 0;

    void CleanUp();

    void UpdateRecordRender();
    
#ifndef ANDROID
    #ifndef SDL
        CGLFWManager glfwManager;
    #else
        CSDLManager sdlManager;
    #endif
    void run();
#endif

    //for static class member. But can not define and init them in the header file!
    static Camera mainCamera; 

    //static Camera lightCameras[2];
    //static std::vector<Camera> lightCameras;

    //Camera lightCameras[2]; //works
    std::vector<Camera> lightCameras; 

    static bool NeedToExit;
    static bool NeedToPause;

    //static bool PrintFPS;

    /*Clean up Functions*/
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    //Make elapseTime and deltaTime public so that user(sample) can access them
    double elapseTime = 0;
    double deltaTime = 0;

    int frameCount = 0;
    double totalInitTime = 0;

    std::string m_sampleName;
    YAML::Node config;

    std::vector<std::unique_ptr<CControlNode>> controlNodes;  

    /******************
    * Core Functions
    ******************/
    /*Virtual function: base and derived class will implement. If drived class not implement, call base's verson*/
    virtual void initialize(); //use this to call sample initialization
    //void initialize(); //base: create sync object, destroy shader resource
    virtual void update(); //base: update time, frame id, camera and ubo
    virtual void recordGraphicsCommandBuffer_renderpassMainscene();
    virtual void recordGraphicsCommandBuffer_renderpassShadowmap(int renderpassIndex);
    virtual void recordComputeCommandBuffer();
    virtual void postUpdate();

    /*Pure virtual function(=0): base class not implment, derived class must implement*/
    //NA

    /******************
    * Helper Functions
    ******************/
    void ReadControls();
    void ReadFeatures();
    void ReadUniforms();
    void ReadAttachments();
    void ReadSubpasses();
    void ReadResources();
    void CreateUniformDescriptors(bool b_uniform_graphics, bool b_uniform_compute);
    void CreatePipelines();
    void ReadRegisterObjects();
    void ReadRegisterTextboxes();
    void ReadLightings();
    void ReadCameras();
    void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);  
        
    /*************
     * APP INFO
     *******/
    struct FeatureConfig {
        bool b_feature_graphics_48pbt = false;
        bool b_feature_graphics_push_constant = false;
        bool b_feature_graphics_blend = false;
        bool b_feature_graphics_rainbow_mipmap = false;
        int feature_graphics_pipeline_skybox_id = -1;
        int feature_graphics_observe_attachment_id = -1;
        bool feature_graphics_show_performance_control = true;
        bool feature_graphics_show_all_metric_controls = true;

        bool feature_graphics_enable_controls = false;

        void loadFromYaml(const YAML::Node& node) {
            b_feature_graphics_48pbt                    = getOrDefault(node, "feature_graphics_48pbt", false);
            b_feature_graphics_push_constant            = getOrDefault(node, "feature_graphics_push_constant", false);
            b_feature_graphics_blend                    = getOrDefault(node, "feature_graphics_blend", false);
            b_feature_graphics_rainbow_mipmap           = getOrDefault(node, "feature_graphics_rainbow_mipmap", false);
            feature_graphics_pipeline_skybox_id         = getOrDefault(node, "feature_graphics_pipeline_skybox_id", -1);
            feature_graphics_observe_attachment_id      = getOrDefault(node, "feature_graphics_observe_attachment_id", -1);
            feature_graphics_show_performance_control   = getOrDefault(node, "feature_graphics_show_performance_control", false);
            feature_graphics_show_all_metric_controls   = getOrDefault(node, "feature_graphics_show_all_metric_controls", false);
            
            feature_graphics_enable_controls = feature_graphics_show_performance_control || feature_graphics_show_all_metric_controls;
        }
    };

    // struct AttachmentInfo{
    //     bool bAttachmentDepthLight;
    //     bool bAttachmentDepthCamera;
    //     bool bAttachmentColorResovle;
    //     bool bAttachmentColorPresent;
    // };
    // struct UniformInfo{
    //     bool b_uniform_graphics_custom = false;
    //     bool b_uniform_graphics_mvp = false;
    //     bool b_uniform_graphics_text_mvp = false;
    //     bool b_uniform_graphics_vp = false;
    //     bool b_uniform_graphics_lighting = false;
    //     bool b_uniform_graphics_depth_image_sampler = false;
    //     bool b_uniform_graphics_lightdepth_image_sampler = false;
    //     bool b_uniform_graphics_lightdepth_image_sampler_hardware = false;
    //     bool b_uniform_compute_custom = false;
    //     bool b_uniform_compute_storage = false;
    //     bool b_uniform_compute_swapchain_storage = false;
    //     bool b_uniform_compute_texture_storage = false;
    //     struct GraphicsCustomInfo{
    //         VkDeviceSize Size = 0;
    //         VkDescriptorSetLayoutBinding Binding;
    //     }GraphicsCustom;
    //     struct ComputeCustomInfo{
    //         VkDeviceSize Size = 0;
    //         VkDescriptorSetLayoutBinding Binding;
    //     }ComputeCustom;
    //     struct ComputeStorageBufferInfo{
    //         VkDeviceSize Size = 0;
    //         VkBufferUsageFlags Usage;
    //     }ComputeStorageBuffer;
    // };

    struct UniformConfig {
        bool b_uniform_graphics_custom = false;
        bool b_uniform_graphics_mvp = false;
        bool b_uniform_graphics_text_mvp = false;
        bool b_uniform_graphics_vp = false;
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
            b_uniform_graphics_mvp                        = getOrDefault(node, "uniform_graphics_mvp", false);
            b_uniform_graphics_text_mvp                   = getOrDefault(node, "uniform_graphics_text_mvp", false);
            b_uniform_graphics_vp                         = getOrDefault(node, "uniform_graphics_vp", false);
            b_uniform_graphics_lighting                   = getOrDefault(node, "uniform_graphics_lighting", false);
            b_uniform_graphics_depth_image_sampler        = getOrDefault(node, "uniform_graphics_depth_image_sampler", false);
            b_uniform_graphics_lightdepth_image_sampler   = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler", false);
            b_uniform_graphics_lightdepth_image_sampler_hardware = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler_hardware", false);

            // std::cout<<"b_uniform_graphics_custom "<<b_uniform_graphics_custom<<std::endl;
            // std::cout<<"b_uniform_graphics_mvp "<<b_uniform_graphics_mvp<<std::endl;
            // std::cout<<"b_uniform_graphics_text_mvp "<<b_uniform_graphics_text_mvp<<std::endl;
            // std::cout<<"b_uniform_graphics_vp "<<b_uniform_graphics_vp<<std::endl;
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
        std::unique_ptr<std::vector<std::string>> ComputeShader;
        CRenderer::RenderModes RenderMode = CRenderer::GRAPHICS;
    }appInfo;
};



#endif
