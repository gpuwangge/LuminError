#pragma once
#include "IResourceCore.h"

namespace LEResource{
    class ResourceCore final : public IResourceCore{
    public:
        ResourceCore(){}
        ~ResourceCore(){}
        void SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) override;


    private:
        LELog::ILogCore *logger = NULL;
    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace