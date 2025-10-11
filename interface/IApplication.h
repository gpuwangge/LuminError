#pragma once
#include "ICommon.h"

class CControlNode;
class CObject;
class CTextManager;

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
        //virtual std::vector<std::unique_ptr<CControlNode>>& GetControlNodes() = 0;
        virtual int GetControlNodeSize() = 0;
        virtual void SetControlNodeVisible(int nodeId, bool value) = 0;
        virtual void* GetInstanceHandle() = 0; //return type is VkInstance
        

        //Expose functions for Example to use
        //virtual std::vector<CObject>& GetObjects() = 0; //
        //virtual CTextManager& GetTextManager() = 0;
        virtual int GetObjectSize() = 0;
        virtual void DrawObject(int objectId) = 0;
        virtual void DrawTexts() = 0;
    };
}