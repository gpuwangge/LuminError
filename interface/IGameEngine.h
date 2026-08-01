#pragma once
#include <string>
#include <vector>
#include "Utility.h"
#include <vulkan/vulkan.h> //todo: replace with forward declaration
#include "TypeRaytracing.h"

class CControlNode;
class CObject;
class CTextManager;
class Vertex3D;
class Vertex2D;

namespace LuminError{
    class IGame;
}

namespace LEGameEngine{
    class IGameEngine {
    public:
        virtual ~IGameEngine() = default;
        virtual void Run(std::string exampleName = "SimpleTriangle") = 0;

        virtual void SetGameContent(LuminError::IGame* gameContent) = 0;

        virtual int GetFrameCount() = 0;
        virtual bool GetCameraInMotion() = 0;
        virtual float GetFPS() = 0;
        virtual void PrintFPS(float interval) = 0;

        virtual int GetRTLightSize() = 0;
        virtual int GetMaterialSize() = 0;

        //Expose functions for SDL Core to use
        virtual bool Get_feature_graphics_enable_controls() = 0;
        virtual bool Get_feature_graphics_show_all_metric_controls() = 0;
        virtual bool Get_feature_graphics_show_performance_control() = 0;
        virtual void Set_feature_graphics_enable_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_all_metric_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_performance_control(bool value) = 0;
        
        virtual int Get_feature_raytracing_pipeline_render_mode() = 0;
        virtual int Get_feature_raytracing_pipeline_interactive_render_mode() = 0;
        virtual int Get_feature_raytracing_pipeline_sampler_per_pixel() = 0;
        virtual int Get_feature_raytracing_pipeline_maximum_bounce() = 0;
        virtual int Get_feature_raytracing_pipeline_maximum_path() = 0;
        virtual bool Get_feature_raytracing_pipeline_accumulate() = 0;
        virtual bool Get_feature_raytracing_pipeline_enableNEE() = 0;
        virtual bool Get_feature_raytracing_pipeline_use_sky() = 0;
        virtual float Get_feature_raytracing_pipeline_maximum_Radiance() = 0;
        virtual int Get_feature_raytracing_pipeline_debug_mode() = 0;
        virtual bool Get_feature_raytracing_pipeline_softShadowEnable() = 0;
        virtual int Get_feature_raytracing_pipeline_softShadowSampleNumber() = 0;
        virtual int Get_feature_raytracing_pipeline_maxReflectionDepth() = 0;
        virtual int Get_feature_raytracing_pipeline_maxRefractionDepth() = 0;
        
        virtual int GetControlNodeSize() = 0;
        virtual void SetControlNodeVisible(int nodeId, bool value) = 0;
        virtual void* GetInstanceHandle() = 0; //return type is VkInstance
        virtual void SetMainCameraVelocityX(float value) = 0;
        virtual void SetMainCameraVelocityY(float value) = 0;
        virtual void SetMainCameraVelocityZ(float value) = 0;
        virtual void SetMainCameraAngularVelocityX(float value) = 0;
        virtual void SetMainCameraAngularVelocityY(float value) = 0;
        virtual void SetMainCameraAngularVelocityZ(float value) = 0;
        virtual void SetMainCameraType(int type) = 0;
        virtual int GetMainCameraType() = 0;
        virtual void SetMainCameraFocusObjectId(int objectId) = 0;
        virtual int GetMainCameraFocusObjectId() = 0;
        virtual void MoveMainCameraLeft(float distance, float speed) = 0;
        virtual void MoveMainCameraRight(float distance, float speed) = 0;
        virtual void MoveMainCameraForward(float distance, float speed) = 0;
        virtual void MoveMainCameraBackward(float distance, float speed) = 0;
        
        //Expose functions for Example(SimpleTriangle) to use
        virtual int GetObjectSize() = 0;
        virtual void DrawObject(int objectId) = 0;
        virtual void DrawTexts() = 0;
        virtual void DrawObjects() = 0;
        virtual void DrawObjects(int startObjectId, int endObjectId) = 0;

        //Expose functions for Example(BasicTriangles) to use
        virtual int GetCurrentFrame() = 0;
        virtual double GetElapseTime() = 0;
        virtual void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage = false) = 0;
        virtual void SetGraphicsCustomSize(int size) = 0;
        virtual void SetGraphicsCustomBinding(void* VkDescriptorSetLayoutBinding) = 0;
        virtual void UploadGraphicsCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) = 0;
        virtual void SetObjectVelocity(int objectId, float vx, float vy, float vz) = 0;
        virtual void SetObjectVelocity(int objectId, glm::vec3 v) = 0;

        //Expose functions for Example(Furmark) to use
        virtual int GetWindowWidth() = 0;
        virtual int GetWindowHeight() = 0;

        //Expose functions for Example(GemmCompute) to use
        //virtual void SetRenderMode(int mode) = 0;
        
        // virtual void SetComputeStorageBufferSize_WindowSwap(int size) = 0;
        // virtual void SetComputeStorageBufferUsage_WindowSwap(int usage) = 0;
        virtual void UploadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void DownloadComputeStorageBuffer_WindowSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) = 0;
        virtual void UploadComputeStorageBuffer_Material(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadComputeStorageBuffer_TriangleVertexAttribute(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadComputeStorageBuffer_TriangleVertexIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadComputeStorageBuffer_TriangleReorderIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadComputeStorageBuffer_BVHNode(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadComputeStorageBuffer_Sphere(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void SetComputeStorageBufferSize_CustomSwap(int size) = 0;
        virtual void SetComputeStorageBufferUsage_CustomSwap(int usage) = 0;
        virtual void UploadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void DownloadComputeStorageBuffer_CustomSwap(uint32_t currentFrame, void* storageBufferObject, int dataSize) = 0;
        
        //virtual void UploadRaytracingStorageBuffer_TriangleVertexAttribute(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        //virtual void UploadRaytracingStorageBuffer_TriangleVertexIndex(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;
        virtual void UploadRaytracingStorageBuffer_GeometryInfo(uint32_t currentFrame, const void* storageBufferObject, size_t dataSize) = 0;

        //virtual CWxjBuffer& GetRaytracingVertexBuffer() = 0;
        // virtual VkDeviceAddress GetRaytracingVertexBufferAddress() = 0;
        // virtual VkDeviceAddress GetRaytracingIndexBufferAddress() = 0;
        // virtual uint32_t GetTriangleVertexCount() = 0;
        // virtual uint32_t GetTriangleIndexCount() = 0;
        // virtual uint32_t GetTriangleVertexStride() = 0;
        virtual RtMesh& GetRtMesh(int index) = 0;
        virtual int GetRtMeshSize() = 0;
        virtual RtSphere& GetRtSphere(int index) = 0;
        virtual int GetRtSphereSize() = 0;

        virtual void ComputeDispatch(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) = 0;
        virtual void RayTrace(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) = 0;
        virtual void DeviceWaitIdle() = 0;
        virtual void SetPause(bool value) = 0;
        virtual void LogContext(std::string s, float *n, int size) = 0;
        virtual void LogContext(std::string s) = 0;
        virtual void LogContext(std::string s, float n) = 0;
        virtual void LogContext(std::string s, int n1, int n2) = 0;

        //Expose functions for Example(MultiCubes) to use
        virtual void SetObjectAngularVelocity(int objectId, float vx, float vy, float vz) = 0;

        //Expose functions for Example(MultiPhongLightings) to use
        virtual int GetLightSize() = 0;
        virtual glm::vec3 GetLightPosition(int lightId) = 0;
        virtual void SetLightPosition(int lightId, float px, float py, float pz) = 0;
        virtual void SetLightPosition(int lightId, glm::vec3 p) = 0;
        virtual void SetObjectPosition(int objectId, float px, float py, float pz) = 0;
        virtual void SetObjectPosition(int objectId, glm::vec3 p) = 0;

        //Expose functions for Example(MultiPhongShadows) to use
        virtual void SetLightCameraPosition(int lightCameraId, glm::vec3 p) = 0;
        virtual void DrawObject(int objectId, int pipelineId) = 0;
        virtual void PushConstantToCommand(void* pcData, int pipelineId) = 0;
        virtual void CmdSetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
        
        //Expose functions for Example(SimpleComputeStorageImage) to use
        virtual void SetSwapchainImageSize(int size) = 0;
        //virtual void EnableComputeSwapChainImage(bool enable) = 0;
        //virtual void CreateComputeCommandBuffers_DispatchForSwapchainImage(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) = 0;
        //virtual void CreateComputeCommandBuffers_DispatchForSwapchainImage_(int numWorkGroupsX, int numWorkGroupsY, int numWorkGroupsZ) = 0;
        
        //Expose functions for Example(SimpleDepthImage) to use
        virtual void SetObjectScaleRectangleXY(int objectId, float x0, float y0, float x1, float y1) = 0;
        virtual void CmdNextSubpass() = 0;
        virtual int GetCustomObjectSize() = 0;

        //Expose functions for Example(SimpleEnvironmentmap) to use
        virtual glm::vec3 GetObjectPosition(int objectId) = 0;
        virtual glm::vec3 GetMainCameraPosition() = 0;

        virtual glm::vec3 GetObjectScale(int objectId) = 0;
        virtual int GetObjectModelID(int objectId) = 0;
        virtual int GetObjectMaterialID(int objectId) = 0;
        virtual glm::vec3 GetObjectRotation(int objectId) = 0;

        //Expose functions for Example(SimpleParticles) to use
        virtual double GetDeltaTime() = 0;
        virtual void DrawParticlesFromStorageBuffer(int objectId, uint32_t particleCount) = 0;
        virtual void SetComputeCustomSize(int size) = 0;
        virtual void SetComputeCustomBinding(void* VkDescriptorSetLayoutBinding) = 0;
        virtual void UploadComputeCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) = 0;

        //Expose functions for Example(SimpleRayPipeline) to use
        virtual void SetRaytracingCustomSize(int size) = 0;
        virtual void SetRaytracingCustomBinding(void* VkDescriptorSetLayoutBinding) = 0;
        virtual void UploadRaytracingCustomUniformBuffer(uint32_t currentFrame, const void* customUniformBufferObject, size_t dataSize) = 0;

        //Expose functions for Example(SimpleShadowMap16Shadows) to use
        virtual void SetLightCameraFocusObjectId(int lightCameraId, int objectId) = 0;
        virtual int GetLightCameraFocusObjectId(int lightCameraId) = 0;
        virtual void SetMainCameraSensitivity(float sensitivity) = 0;
        
        //Expose functions for Example(SimpleUniformBuffer) to use
        virtual void DrawObject(int objectId, int pipelineId, int numVertex) = 0;
        
        //Expose functions for Example(SimpleVertexBuffer) to use
        virtual void CreateCustomModel2D(std::vector<Vertex2D> &vertices2D) = 0;

        //Expose for renderer core to get context
        // virtual QueueFamilyIndices GetQueueFamilyIndices() = 0;
        // virtual VkDevice GetLogicalDevice() = 0;
        // virtual VkPhysicalDevice GetPhysicalDevice() = 0;

        // virtual VkQueue GetGraphicsQueue() = 0;
        // virtual VkQueue GetPresentQueue() = 0;
        // virtual VkQueue GetComputeQueue() = 0;

        // virtual QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface, std::string s) = 0;
        // virtual VkSampleCountFlagBits GetMaxUsableSampleCount() = 0;
        // virtual SwapChainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface) = 0;


    };

}