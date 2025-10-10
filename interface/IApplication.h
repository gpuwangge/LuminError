#pragma once
#include "ICommon.h"
#include "IExample.h"

class CControlNode;

namespace LEApplication{
    class IApplication {
    public:
        virtual ~IApplication() = default;
        virtual void Run(std::string exampleName = "example001") = 0;
        virtual void Greet() = 0;

        //virtual AppInfo* GetAppInfo() = 0;
        virtual bool Get_feature_graphics_enable_controls() = 0;
        virtual bool Get_feature_graphics_show_all_metric_controls() = 0;
        virtual bool Get_feature_graphics_show_performance_control() = 0;
        virtual void Set_feature_graphics_enable_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_all_metric_controls(bool value) = 0;
        virtual void Set_feature_graphics_show_performance_control(bool value) = 0;
        virtual std::vector<std::unique_ptr<CControlNode>>& GetControlNodes() = 0;
    };
}