#pragma once
#include "IApplication.h"

namespace LEExample{
    class IExample{
    public:
        virtual ~IExample() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        
        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void PostUpdate() = 0;
        virtual void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) = 0;
        virtual void RecordGraphicsCommandBuffer_RenderpassMainscene() = 0;
        virtual void RecordComputeCommandBuffer() = 0;

        virtual void Record() = 0; //same as RecordGraphicsCommandBuffer_RenderpassMainscene()
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}