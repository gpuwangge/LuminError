#pragma once
#include "../IExample.h"

namespace LEExample{
    class ISimpleTriangle : public IExample {
    public:
        virtual ~ISimpleTriangle() = default;
        void Initialize() {}
        void Update() {}
        void PostUpdate() {}
        void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) {}
        void RecordGraphicsCommandBuffer_RenderpassMainscene() {}
        void RecordComputeCommandBuffer() {}

        void Record() {}
    };
}