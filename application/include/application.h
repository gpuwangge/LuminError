#ifndef H_APPLICATION
#define H_APPLICATION

#include "camera.hpp"
#include "instance.h"
#include "context.h"
#include "texture.h" //this includes imageManager.h
#include "modelManager.h"
#include "object.h"
#include "textManager.h"
#include "light.h"
#include "timer.h"
#include "controlNode.h"
#include "shaderManager.h"

#include "IApplication.h"
#include "IGame.h"
#include "Utility.h"
#include <vector>
#include "Enum.h"

//Macro to convert the macro value to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#include "ISDLCore.h"
#include "IYAMLCore.h"
#include "IRendererCore.h"
#include "ILogCore.h"

//added this to remove windows.h
#ifdef _WIN32
// 前向声明 HMODULE
//typedef void* HMODULE;
// 或者更精确的方式：
struct HINSTANCE__;
typedef HINSTANCE__* HMODULE;
#endif

class CSwapchain;
//class CShaderManager;

namespace LEApplication{
    class Application : public LEApplication::IApplication{
    public:
        Application();
        ~Application();

        CLogManager logManager;

        //VkInstance instance;//01
        std::unique_ptr<CInstance> instance{nullptr};

        int windowWidth = 0;
        int windowHeight = 0;
        VkSurfaceKHR surface;//03

        //bool framebufferResized = false;
        //bool needWindow = false;

        CShaderManager shaderManager;
        CModelManager modelManager;
        CTextureManager textureManager;
        CTextImageManager textImageManager;
        CTextManager textManager;

        int objectCountControl = 0;
        int textboxCountControl = 0;
        int lightCountControl = 0;

        std::vector<CObject> objects;
        std::vector<CLight> lights;

        void CleanUp();

        Camera mainCamera; 
        //Camera lightCameras[2]; //works
        std::vector<Camera> lightCameras; 

        bool NeedToExit;
        bool NeedToPause;

        /*Clean up Functions*/
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        //Make elapseTime and deltaTime public so that user(sample) can access them
        double elapseTime = 0;
        double deltaTime = 0;

        int frameCount = 0;
        double totalInitTime = 0;

        std::string m_sampleName = "CSimpleTriangle";

        std::vector<std::unique_ptr<CControlNode>> controlNodes;  

        /******************
        * Helper Functions
        ******************/
        void Dispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ);
        std::string GetPureName(const std::string& path);

        /******************
        * Core Functions
        ******************/
        void Run(std::string exampleName) override;
        void Record_Present();

        void Initialize(); //use this to call sample initialization
        void Update(); //base: update time, frame id, camera and ubo
        void RecordGraphicsCommandBuffer_RenderpassMainscene(){ gamer->Record(); gamer->RecordGraphicsCommandBuffer_RenderpassMainscene(); }

        //Module Related
        HMODULE handle_module_yamlcore;
        LEYAML::IYAMLCore *yamler = NULL;
        HMODULE handle_module_sdlcore;
        LESDL::ISDLCore *sdler = NULL;
        HMODULE handle_module_game;
        LuminError::IGame *gamer = NULL;
        HMODULE handle_module_renderercore;
        LERenderer::IRendererCore *renderer = NULL;
        HMODULE handle_module_logcore;
        LELog::ILogCore *logger = NULL;
        //std::shared_ptr<LELog::ILogCore> logger;

        void LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName);
        void DestroyInstance(HMODULE handle, void* instance);

        AppInfo* appInfo = nullptr;

        //Expose functions for SDL Core to use
        bool Get_feature_graphics_enable_controls() override;
        bool Get_feature_graphics_show_all_metric_controls() override;
        bool Get_feature_graphics_show_performance_control() override;
        void Set_feature_graphics_enable_controls(bool value) override;
        void Set_feature_graphics_show_all_metric_controls(bool value) override;
        void Set_feature_graphics_show_performance_control(bool value) override;

        int GetControlNodeSize() override;
        void SetControlNodeVisible(int nodeId, bool value) override;
        void* GetInstanceHandle() override;
        
        int GetObjectSize() override;
        int GetCustomObjectSize() override;
        void SetObjectVelocity(int objectId, float vx, float vy, float vz) override;
        void SetObjectVelocity(int objectId, glm::vec3 v) override;
        void SetObjectAngularVelocity(int objectId, float vx, float vy, float vz) override;
        void SetObjectPosition(int objectId, float px, float py, float pz) override;
        void SetObjectPosition(int objectId, glm::vec3 p) override;
        void SetObjectScaleRectangleXY(int objectId, float x0, float y0, float x1, float y1) override;
        glm::vec3 GetObjectPosition(int objectId) override;

        int GetLightSize() override;
        glm::vec3 GetLightPosition(int lightId) override;
        void SetLightPosition(int lightId, float px, float py, float pz) override;
        void SetLightPosition(int lightId, glm::vec3 p) override;

        void CreateCustomModel2D(std::vector<Vertex2D> &vertices2D) override;
        void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage) override;

        void DrawObject(int objectId) override;
        void DrawTexts() override;
        void DrawObjects() override;
        void DrawObjects(int startObjectId, int endObjectId) override;
        void DrawObject(int objectId, int pipelineId) override;
        void DrawObject(int objectId, int pipelineId, int numVertex) override;
        void DrawParticlesFromStorageBuffer(int objectId, uint32_t particleCount) override;

        void ComputeDispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;

        void SetComputeCustomSize(int size) override;
        void SetComputeCustomBinding(void* binding) override;
        void UploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) override;
        void SetComputeStorageBufferSize(int size) override;
        void SetComputeStorageBufferUsage(int usage) override;
        void UploadComputeStorageBuffer(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void DownloadComputeStorageBuffer(uint32_t currentFrame, void* storageBufferObject, int dataSize) override;

        void SetGraphicsCustomSize(int size) override;
        void SetGraphicsCustomBinding(void* binding) override ;
        void UploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) override;

        void SetMainCameraVelocityX(float value) override;
        void SetMainCameraVelocityY(float value) override;
        void SetMainCameraVelocityZ(float value) override;
        void SetMainCameraAngularVelocityX(float value) override;
        void SetMainCameraAngularVelocityY(float value) override;
        void SetMainCameraAngularVelocityZ(float value) override;
        void SetMainCameraType(int type) override;
        int GetMainCameraType() override;
        void SetMainCameraFocusObjectId(int objectId) override;
        int GetMainCameraFocusObjectId() override;
        void MoveMainCameraLeft(float distance, float speed) override;
        void MoveMainCameraRight(float distance, float speed) override;
        void MoveMainCameraForward(float distance, float speed) override;
        void MoveMainCameraBackward(float distance, float speed) override;
        glm::vec3 GetMainCameraPosition() override;
        void SetMainCameraSensitivity(float sensitivity) override;

        void SetLightCameraPosition(int lightCameraId, glm::vec3 p) override;
        void SetLightCameraFocusObjectId(int lightCameraId, int objectId) override;
        int GetLightCameraFocusObjectId(int lightCameraId) override;

        void LogContext(std::string s, float *n, int size) override;
        void LogContext(std::string s) override;
        void LogContext(std::string s, float n) override;
        void LogContext(std::string s, int n1, int n2) override;

        void SetRenderMode(int mode) override;
        void SetPause(bool value) override;
        int GetWindowWidth() override;
        int GetWindowHeight() override;
        int GetCurrentFrame() override;
        double GetElapseTime() override;
        double GetDeltaTime() override;

        void CmdNextSubpass() override;
        void SetSwapchainImageSize(int size) override;
        void EnableComputeSwapChainImage(bool enable) override;
        void DeviceWaitIdle() override;

        void PushConstantToCommand(void* pcData, int pipelineId) override;
        void CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override;

        void CreateComputeCommandBuffers_DispatchForSwapchainImage(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;

        //VkPhysicalDevice GetPhysicalDevice() override { return CContext::GetHandle().physicalDevice->get()->getHandle();}
        QueueFamilyIndices GetQueueFamilyIndices() override { return CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, "Find Queue Families when creating command pool"); }
        VkDevice GetLogicalDevice() override { return CContext::GetHandle().GetLogicalDevice(); }
        VkPhysicalDevice GetPhysicalDevice() override { return CContext::GetHandle().GetPhysicalDevice(); }

        VkQueue GetGraphicsQueue() override{ return CContext::GetHandle().GetGraphicsQueue(); }
        VkQueue GetPresentQueue() override{ return CContext::GetHandle().GetPresentQueue(); }
        VkQueue GetComputeQueue() override{ return CContext::GetHandle().GetComputeQueue(); }

        QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface, std::string s) override { return CContext::GetHandle().physicalDevice->get()->findQueueFamilies(surface, s); }
        VkSampleCountFlagBits GetMaxUsableSampleCount() override { return CContext::GetHandle().physicalDevice->get()->getMaxUsableSampleCount(); }
        SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) override { return CContext::GetHandle().physicalDevice->get()->querySwapChainSupport(surface); }
    };


    extern "C" void* CreateInstance();
    extern "C" void DestroyInstance(void *p);
}



#endif
