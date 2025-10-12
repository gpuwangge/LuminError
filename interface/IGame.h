#pragma once
#include "IApplication.h"

namespace LuminError{
    class IGame{
    public:
        virtual ~IGame() = default;
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        
        virtual void Initialize() {};
        virtual void Update() {};
        virtual void PostUpdate() {};
        virtual void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) {};
        virtual void RecordGraphicsCommandBuffer_RenderpassMainscene() {};
        virtual void RecordComputeCommandBuffer() {};

        virtual void Record() {}; //same as RecordGraphicsCommandBuffer_RenderpassMainscene()
    };

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
}