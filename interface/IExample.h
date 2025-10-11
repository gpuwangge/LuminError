#pragma once
#include "ICommon.h"

namespace LEApplication{
    class IApplication;
}

namespace LEExample{
    class IExample{
    public:
        virtual ~IExample() = default;
        virtual void SetApplication(LEApplication::IApplication* pApplication) = 0;
        
        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void PostUpdate() = 0;
        virtual void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) = 0;
        virtual void RecordGraphicsCommandBuffer_RenderpassMainscene() = 0;
        virtual void RecordComputeCommandBuffer() = 0;

        virtual void Record() = 0; //same as RecordGraphicsCommandBuffer_RenderpassMainscene()
    };
}