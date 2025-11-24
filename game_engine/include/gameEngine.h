#pragma once
#include "IGameEngine.h"
#include "IGame.h"
#include "ISDLCore.h"
#include "IYAMLCore.h"
#include "IRendererCore.h"
#include "ILogCore.h"
#include "IResourceCore.h"

#include "camera.hpp"
#include "object.h"
#include "textManager.h"
#include "light.h"
#include "controlNode.h"

#include "Utility.h"
#include <vector>
#include "Enum.h"
#include "timer.h"

//Macro to convert the macro value to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

//added this to remove windows.h
#ifdef _WIN32
// forward declaration HMODULE
//typedef void* HMODULE;
// or better：
struct HINSTANCE__;
typedef HINSTANCE__* HMODULE;
#endif

class CSwapchain;

namespace LEGameEngine{
    class GameEngine : public LEGameEngine::IGameEngine{
    public:
        GameEngine();
        ~GameEngine();

        LuminError::IGame *gamer = NULL;
        void SetGameContent(LuminError::IGame* gameContent) override{ gamer = gameContent; }
        //CLogManager logManager;

        //VkInstance instance;//01
        //std::unique_ptr<CInstance> instance{nullptr};

        int windowWidth = 0;
        int windowHeight = 0;
        //VkSurfaceKHR surface;//03

        //bool framebufferResized = false;
        //bool needWindow = false;
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

        bool NeedToExit = false;
        bool NeedToPause = false;

        /*Clean up Functions*/
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        //Make elapseTime and deltaTime public so that user(sample) can access them
        double elapseTime = 0;
        double deltaTime = 0;

        int frameCount = 0;
        double totalInitTime = 0;

        //std::string m_exampleName = "CSimpleTriangle";

        std::vector<std::unique_ptr<CControlNode>> controlNodes;

        //StructStorageBuffer_WindowSwap storageBufferObject_WindowSwap{}; //?define here not work?
        StructStorageBuffer_Material storageBufferObject_Material{};
        StructStorageBuffer_TriangleVertex storageBufferObject_TriangleVertex{};
        StructStorageBuffer_TriangleIndex storageBufferObject_TriangleIndex{};
        StructStorageBuffer_Sphere storageBufferObject_Sphere{};

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
        void RecordGraphicsCommandBuffer_RenderpassMainscene(){ gamer->RecordGraphicsCommandBuffer_RenderpassMainscene(); } //todo: Optimize here

        //Module Related
        HMODULE handle_module_yamlcore;
        LEYAML::IYAMLCore *yamler = NULL;
        HMODULE handle_module_sdlcore;
        LESDL::ISDLCore *sdler = NULL;
        //HMODULE handle_module_game;
        //LuminError::IGame *gamer = NULL;
        HMODULE handle_module_renderercore;
        LERenderer::IRendererCore *renderer = NULL;
        HMODULE handle_module_logcore;
        LELog::ILogCore *logger = NULL;
        //std::shared_ptr<LELog::ILogCore> logger;
        HMODULE handle_module_resourcecore;
        LEResource::IResourceCore *resourcer = NULL;

        void LoadModuleAndInstance(HMODULE &handle, void* &instance, const std::string moduleName);
        void DestroyInstance(HMODULE handle, void* instance);

        AppInfo* appInfo = nullptr;

        int GetFrameCount() override { return frameCount; }
        bool GetCameraInMotion() override { return mainCamera.IsInMotion(); }
        float GetFPS() override { return (deltaTime > 0) ? (1.0f / static_cast<float>(deltaTime)) : 0.0f; }
        void PrintFPS(float interval) override {
            static float elapsed = 0.0f;
            elapsed += static_cast<float>(deltaTime);
            if (elapsed < interval) return;
            elapsed = 0.0f;

            float fps = GetFPS();
            std::cout<<"FPS: "<<fps<<std::endl;
        }

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
        
        //void SetComputeStorageBufferSize_WindowSwap(int size) override;
        //void SetComputeStorageBufferUsage_WindowSwap(int usage) override;
        void UploadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void DownloadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) override;

        void UploadComputeStorageBuffer_Material(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void UploadComputeStorageBuffer_TriangleVertex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void UploadComputeStorageBuffer_TriangleIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void UploadComputeStorageBuffer_Sphere(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;

        void SetComputeStorageBufferSize_CustomSwap(int size) override;
        void SetComputeStorageBufferUsage_CustomSwap(int usage) override;
        void UploadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) override;
        void DownloadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) override;

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

        //void CreateComputeCommandBuffers_DispatchForSwapchainImage(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;
        //void CreateComputeCommandBuffers_DispatchForSwapchainImage_(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) override;

        //VkPhysicalDevice GetPhysicalDevice() override { return CContext::GetHandle().physicalDevice->get()->getHandle();}
        // QueueFamilyIndices GetQueueFamilyIndices() override { return renderer->GetQueueFamilyIndices(); }
        // VkDevice GetLogicalDevice() override { return renderer->GetLogicalDevice(); }
        // VkPhysicalDevice GetPhysicalDevice() override { return renderer->GetPhysicalDevice(); }

        // VkQueue GetGraphicsQueue() override{ return renderer->GetGraphicsQueue(); }
        // VkQueue GetPresentQueue() override{ return renderer->GetPresentQueue(); }
        // VkQueue GetComputeQueue() override{ return renderer->GetComputeQueue(); }

        // QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface, std::string s) override { return renderer->FindQueueFamilies(surface, s); }
        // VkSampleCountFlagBits GetMaxUsableSampleCount() override { return renderer->GetMaxUsableSampleCount(); }
        // SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) override { return renderer->QuerySwapChainSupport(surface); }
    };


    extern "C" void* CreateInstance();
    extern "C" void DestroyInstance(void *p);
}


