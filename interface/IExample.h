#pragma once
#include "ICommon.h"

namespace LEExample{
    class IExample{
    public:
        virtual ~IExample() = default;
        virtual void Update() = 0;
    };
}