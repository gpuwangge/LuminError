#pragma once
#include "IResourceCore.h"

namespace LEResource{
    class ResourceCore final : public IResourceCore{
    public:
        ResourceCore(){}
        ~ResourceCore(){}
        void SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) override;
        void SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_) override;

    private:
        LELog::ILogCore *logger = NULL;
        VkDevice logicalDevice = NULL;
        VkPhysicalDevice physicalDevice = NULL;
    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace