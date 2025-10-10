#pragma once
#include "ICommon.h"
#include "IExample.h"

namespace LEApplication{
    class IApplication {
    public:
        virtual ~IApplication() = default;
        virtual void Run(std::string exampleName = "example001") = 0;
        virtual void Greet() = 0;
    };
}