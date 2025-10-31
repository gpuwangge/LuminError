#pragma once
#include "ILogCore.h"
//#include <vulkan/vulkan.h>

namespace LELog{
    class LogCore final : public ILogCore{
    public:
        LogCore(){}
        ~LogCore(){}
    };

    EXPORT_FACTORY_FOR(LogCore);
}//namespace