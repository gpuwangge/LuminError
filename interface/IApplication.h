#pragma once
#include <string>
#include <vector>

class CControlNode;
class CObject;
class CTextManager;
class Vertex3D;

namespace LEApplication{
    class IApplication {
    public:
        virtual ~IApplication() = default;
        virtual void Run(std::string exampleName = "SimpleTriangle") = 0;
        virtual void Greet() = 0;

        //Expose functions for SDL Core to use
        virtual bool Get_feature_graphics_enable_controls() = 0;
        virtual bool Get_feature_graphics_show_all_metric_controls() = 0;
        virtual bool Get_feature_graphics_show_performance_control() = 0;
        virtual void Set_feature_graphics_enable_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_all_metric_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_performance_control(bool value) = 0;
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

        //Expose functions for Example(BasicTriangles) to use
        virtual int GetCurrentFrame() = 0;
        virtual double GetElapseTime() = 0;
        virtual void CreateCustomModel3D(std::vector<Vertex3D> &vertices3D, std::vector<uint32_t> &indices3D, bool isTextboxImage = false) = 0;
        virtual void SetGraphicsCustomSize(int size) = 0;
        virtual void SetGraphicsCustomBinding(void* VkDescriptorSetLayoutBinding) = 0;
        virtual void UpdateGraphicsCustomUniformBuffer(uint32_t currentFrame, void* customUniformBufferObject, size_t dataSize) = 0;
        virtual void SetObjectVelocity(int objectId, float vx, float vy, float vz) = 0;

        //Expose functions for Example(Furmark) to use
        virtual int GetWindowWidth() = 0;
        virtual int GetWindowHeight() = 0;

    };

    // #define EXPORT_APPLICATION_FACTORY_FOR(ClassName) \
    //     extern "C" void* CreateInstance() { return new ClassName(); } \
    //     extern "C" void DestroyInstance(void* p) { \
    //         if (p){ \
    //             static_cast<ClassName*>(p)->DestroyInstance(static_cast<ClassName*>(p)->handle_module_sdlcore,static_cast<ClassName*>(p)->instance_sdlcore); \
    //             static_cast<ClassName*>(p)->DestroyInstance(static_cast<ClassName*>(p)->handle_module_game,static_cast<ClassName*>(p)->instance_game); \
    //             delete static_cast<ClassName*>(p); \
    //         } \
    //     }
}