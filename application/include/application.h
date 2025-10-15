#ifndef H_APPLICATION
#define H_APPLICATION

#include "camera.hpp"
#include "instance.h"
#include "swapchain.h"
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

#include <windows.h>
#include "IApplication.h"
#include "IGame.h"
#include "Utility.h"
#include <vector>
#include <iomanip>
#include <iostream>
#include "Enum.h"

//Macro to convert the macro value to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef WIN32
    #define YAML_CPP_STATIC_DEFINE //to disable lots of yaml warnings
#endif
#include "../../thirdParty/yaml-cpp/yaml.h"

#include "ISDLCore.h"


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

namespace LEApplication{
    class Application : public LEApplication::IApplication{
    public:
        Application();
        ~Application();

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

        void Greet() override {std::cout<<"test greet"<<std::endl;}

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

        std::string m_sampleName = "CSimpleTriangle";
        YAML::Node config;

        std::vector<std::unique_ptr<CControlNode>> controlNodes;  

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
                b_uniform_graphics_object_mvp                        = getOrDefault(node, "uniform_graphics_object_mvp", false);
                b_uniform_graphics_text_mvp                   = getOrDefault(node, "uniform_graphics_text_mvp", false);
                b_uniform_graphics_vp                         = getOrDefault(node, "uniform_graphics_vp", false);
                b_uniform_graphics_lighting                   = getOrDefault(node, "uniform_graphics_lighting", false);
                b_uniform_graphics_depth_image_sampler        = getOrDefault(node, "uniform_graphics_depth_image_sampler", false);
                b_uniform_graphics_lightdepth_image_sampler   = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler", false);
                b_uniform_graphics_lightdepth_image_sampler_hardware = getOrDefault(node, "uniform_graphics_lightdepth_image_sampler_hardware", false);

                // std::cout<<"b_uniform_graphics_custom "<<b_uniform_graphics_custom<<std::endl;
                // std::cout<<"b_uniform_graphics_object_mvp "<<b_uniform_graphics_object_mvp<<std::endl;
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
            std::unique_ptr<std::vector<bool>> BlendEnable;
            std::unique_ptr<std::vector<bool>> DepthTestEnable;
            std::unique_ptr<std::vector<bool>> DepthWriteEnable;
            std::unique_ptr<std::vector<bool>> SkyboxEnable;
            std::unique_ptr<std::vector<std::string>> ComputeShader;
            RenderModes RenderMode = RenderModes::GRAPHICS;
        }appInfo;




        //Functions to call example functions
        /******************
        * Core Functions
        ******************/
        void Run(std::string exampleName) override;
        void UpdateRecordRender();


        void Initialize(); //use this to call sample initialization
        void Update(); //base: update time, frame id, camera and ubo
        //void PostUpdate();
        void RecordGraphicsCommandBuffer_RenderpassMainscene();
        void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex);
        void RecordComputeCommandBuffer();
        
        

        //Module Related
        HMODULE handle_module_sdlcore;
        LESDL::ISDLCore *instance_sdlcore = NULL;
        HMODULE handle_module_game;
        LuminError::IGame *instance_game = NULL;
        void LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName);
        void DestroyInstance(HMODULE handle, void* instance);

        //Expose functions for SDL Core to use
        bool Get_feature_graphics_enable_controls() override {return appInfo.Feature.feature_graphics_enable_controls;}
        bool Get_feature_graphics_show_all_metric_controls() override {return appInfo.Feature.feature_graphics_show_all_metric_controls;}
        bool Get_feature_graphics_show_performance_control() override {return appInfo.Feature.feature_graphics_show_performance_control;}
        void Set_feature_graphics_enable_controls(bool value) override {appInfo.Feature.feature_graphics_enable_controls = value;}
        void Set_feature_graphics_show_all_metric_controls(bool value) override {appInfo.Feature.feature_graphics_show_all_metric_controls = value;}
        void Set_feature_graphics_show_performance_control(bool value) override {appInfo.Feature.feature_graphics_show_performance_control = value;}
        int GetControlNodeSize() override { return controlNodes.size();}
        void SetControlNodeVisible(int nodeId, bool value) override { controlNodes[nodeId]->bVisible = value;}
        void* GetInstanceHandle() override {return instance->getHandle();}
        void SetMainCameraVelocityX(float value) override { mainCamera.Velocity.x = value; }
        void SetMainCameraVelocityY(float value) override { mainCamera.Velocity.y = value; }
        void SetMainCameraVelocityZ(float value) override { mainCamera.Velocity.z = value; }
        void SetMainCameraAngularVelocityX(float value) override { mainCamera.AngularVelocity.x = value; }
        void SetMainCameraAngularVelocityY(float value) override { mainCamera.AngularVelocity.y = value; }
        void SetMainCameraAngularVelocityZ(float value) override { mainCamera.AngularVelocity.z = value; }
        void SetMainCameraType(int type) override { mainCamera.cameraType = (CameraType)type; }
        int GetMainCameraType() override { return mainCamera.cameraType; }
        void SetMainCameraFocusObjectId(int objectId) override { mainCamera.focusObjectId = objectId; }
        int GetMainCameraFocusObjectId() override { return mainCamera.focusObjectId; }
        void MoveMainCameraLeft(float distance, float speed) override { mainCamera.MoveLeft(distance, speed); }
        void MoveMainCameraRight(float distance, float speed) override { mainCamera.MoveRight(distance, speed); }
        void MoveMainCameraForward(float distance, float speed) override { mainCamera.MoveForward(distance, speed); }
        void MoveMainCameraBackward(float distance, float speed) override { mainCamera.MoveBackward(distance, speed); }

        //Expose functions for Example(SimpleTriangle) to use
        int GetObjectSize() override { return objects.size(); }
        void DrawObject(int objectId) override { objects[objectId].Draw(); }
        void DrawTexts() override { textManager.Draw(); }
        void DrawObjects() override { for(int i = 0; i < objects.size(); i++) objects[i].Draw(); }
        
        //Expose functions for Example(BasicTriangles) to use
        int GetCurrentFrame() override { return renderer.currentFrame;}
        double GetElapseTime() override { return elapseTime;}
        void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage) override {
            modelManager.CreateCustomModel3D(vertices3D, indices3D, isTextboxImage);
        }
        void SetGraphicsCustomSize(int size) override { appInfo.Uniform.GraphicsCustom.Size = size; }
        void SetGraphicsCustomBinding(void* binding) override {
            VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
            if (bindingPtr) appInfo.Uniform.GraphicsCustom.Binding = *bindingPtr;
        }
        void UploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) override {
            //graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, customUBO);
            graphicsDescriptorManager.uploadCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
        }
        void SetObjectVelocity(int objectId, float vx, float vy, float vz) override {objects[objectId].SetVelocity(vx, vy, vz);}
        void SetObjectVelocity(int objectId, glm::vec3 v) override {objects[objectId].SetVelocity(v);}

        //Expose functions for Example(Furmark) to use
        int GetWindowWidth() override { return windowWidth; }
        int GetWindowHeight() override { return windowHeight; }

        //Expose functions for Example(GemmCompute) to use
        void SetRenderMode(int mode) override { appInfo.RenderMode = (RenderModes)mode; }
        void SetComputeStorageBufferSize(int size) override { appInfo.Uniform.ComputeStorageBuffer.Size = size; }
        void SetComputeStorageBufferUsage(int usage) override {appInfo.Uniform.ComputeStorageBuffer.Usage = usage; }
        void UploadComputeStorageBuffer(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override {
            computeDescriptorManager.uploadStorageBuffer(currentFrame, storageBufferObject, dataSize);
        }
        void ComputeDispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override {
            Dispatch(numWorkGroupsX,numWorkGroupsY,numWorkGroupsZ);
        }
        void DeviceWaitIdle() override { vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice()); }
        void DownloadComputeStorageBuffer(uint32_t currentFrame, void* storageBufferObject, int dataSize) override {
            //std::cout<<"RestoreComputeStorageBuffer: dataSize = "<<dataSize<<", currentFrame = "<<currentFrame<<std::endl;
            //memcpy(storageBufferObject, computeDescriptorManager.storageBuffersMapped[currentFrame], dataSize);
            computeDescriptorManager.downloadStorageBuffer(currentFrame, storageBufferObject, dataSize);
        }
        void SetPause(bool value) override { NeedToPause = value; }
        void LogContext(std::string s, float *n, int size) override { CContext::GetHandle().logManager.print(s, n, size);}
        void LogContext(std::string s) override { CContext::GetHandle().logManager.print(s); }
        void LogContext(std::string s, float n) override {CContext::GetHandle().logManager.print(s, n);}
        void LogContext(std::string s, int n1, int n2) override {CContext::GetHandle().logManager.print(s, n1, n2);}

        //Expose functions for Example(MultiCubes) to use
        void SetObjectAngularVelocity(int objectId, float vx, float vy, float vz) override {objects[objectId].SetAngularVelocity(vx, vy, vz); }
 
        //Expose functions for Example(MultiPhongLightings) to use
        int GetLightSize() override { return lights.size(); }
        glm::vec3 GetLightPosition(int lightId) override { return lights[lightId].GetLightPosition(); }
        void SetLightPosition(int lightId, float px, float py, float pz) override { lights[lightId].SetLightPosition(glm::vec3(px, py, pz)); }
        void SetLightPosition(int lightId, glm::vec3 p) override { lights[lightId].SetLightPosition(p); }
        void SetObjectPosition(int objectId, float px, float py, float pz) override { objects[objectId].SetPosition(px, py, pz); }
        void SetObjectPosition(int objectId, glm::vec3 p) override { objects[objectId].SetPosition(p); }
 
        //Expose functions for Example(MultiPhongShadows) to use
        void SetLightCameraPosition(int lightCameraId, glm::vec3 p) override { lightCameras[lightCameraId].SetPosition(p); }
        void DrawObject(int objectId, int pipelineId) override { objects[objectId].Draw(pipelineId); }
        void PushConstantToCommand(void* pcData, int pipelineId) override {
            renderer.PushConstantToCommand(pcData, renderProcess.graphicsPipelineLayouts[pipelineId], shaderManager.pushConstantRange);
        }
        void CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override {
            vkCmdSetDepthBias(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
        }

        //Expose functions for Example(SimpleComputeStorageImage) to use
        void SetSwapchainImageSize(int size) override { swapchain.swapchainImageSize = size; }
        void EnableComputeSwapChainImage(bool enable) override { swapchain.bComputeSwapChainImage = enable; }
        void CreateComputeCommandBuffers() override {
            std::vector<VkCommandBuffer> &commandBuffers = renderer.commandBuffers[renderer.computeCmdId];
            std::vector<VkImage> &swapChainImages = swapchain.swapchain_images;

            for (size_t i = 0; i < commandBuffers.size(); i++) {
                renderer.currentFrame = i;
                //std::cout<<"commandbuffer i: "<<i<<std::endl;
                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                //if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                //    throw std::runtime_error("failed to begin recording command buffer!");
                //}
                renderer.StartRecordComputeCommandBuffer(renderProcess.computePipeline, renderProcess.computePipelineLayout);


                recordImageBarrier(commandBuffers[i], swapChainImages[i],
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                    VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_SHADER_WRITE_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                
                
                Dispatch(200,300,1);
            
                recordImageBarrier(commandBuffers[i], swapChainImages[i],
                    VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

                //if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                //    throw std::runtime_error("failed to record command buffer!");
                //}
                renderer.EndRecordComputeCommandBuffer();
            }
            renderer.currentFrame = 0;


        } //TODO: optimize this
        //void RecordImageBarrier() override {} //TODO: optimize this
        void recordImageBarrier(VkCommandBuffer buffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
            VkAccessFlags scrAccess, VkAccessFlags dstAccess, VkPipelineStageFlags srcBind, VkPipelineStageFlags dstBind) {
            VkImageMemoryBarrier barrier{};
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcAccessMask = scrAccess;
            barrier.dstAccessMask = dstAccess;
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            VkImageSubresourceRange sub{};
            sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            sub.baseArrayLayer = 0;
            sub.baseMipLevel = 0;
            sub.layerCount = VK_REMAINING_MIP_LEVELS;
            sub.levelCount = VK_REMAINING_MIP_LEVELS;
            barrier.subresourceRange = sub;

            vkCmdPipelineBarrier(buffer, srcBind, dstBind,
                0, 0, nullptr, 0, nullptr, 1, &barrier);
        }

        //Expose functions for Example(SimpleDepthImage) to use
        void SetObjectScaleRectangleXY(int objectId, float x0, float y0, float x1, float y1) override { objects[objectId].SetScaleRectangleXY(x0, y0, x1, y1); }
        void CmdNextSubpass() override { vkCmdNextSubpass(renderer.commandBuffers[renderer.graphicsCmdId][renderer.currentFrame], VK_SUBPASS_CONTENTS_INLINE); }
        int GetCustomObjectSize() override { return customObjectSize; }

        //Expose functions for Example(SimpleEnvironmentmap) to use
        glm::vec3 GetObjectPosition(int objectId) override { return objects[objectId].Position; }
        glm::vec3 GetMainCameraPosition() override { return mainCamera.Position; }

        //Expose functions for Example(SimpleParticles) to use
        double GetDeltaTime() override { return deltaTime; }
        void DrawParticlesFromStorageBuffer(int objectId, uint32_t particleCount) override {
            //objects[objectId].Draw(*static_cast<std::vector<CWxjBuffer>*>(buffer), graphicsPipelineId, particleCount);
            objects[objectId].Draw(computeDescriptorManager.storageBuffers, -1, particleCount);
        }
        void SetComputeCustomSize(int size) override { appInfo.Uniform.ComputeCustom.Size = size; }
        void SetComputeCustomBinding(void* binding) override {
            VkDescriptorSetLayoutBinding* bindingPtr = static_cast<VkDescriptorSetLayoutBinding*>(binding);
            if (bindingPtr) appInfo.Uniform.ComputeCustom.Binding = *bindingPtr;
        }
        void UploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) override {
            //graphicsDescriptorManager.updateCustomUniformBuffer<CustomUniformBufferObject>(renderer.currentFrame, customUBO);
            computeDescriptorManager.uploadCustomUniformBuffer(currentFrame, customUniformBufferObject, dataSize);
        }
    };


    extern "C" void* CreateInstance();
    extern "C" void DestroyInstance(void *p);
}



#endif
