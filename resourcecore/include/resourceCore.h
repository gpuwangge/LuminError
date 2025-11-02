#pragma once
#include "IResourceCore.h"
//#include <fstream>
//#include "Utility.h"

namespace LEResource{
    class ResourceCore final : public IResourceCore{
    public:
        ResourceCore(){}
        ~ResourceCore(){}


    private:

    };

    EXPORT_FACTORY_FOR(ResourceCore);
}//namespace