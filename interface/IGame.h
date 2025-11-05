#pragma once
#include "IGameEngine.h"
namespace LuminError{
    class IGame{
    public:
        virtual ~IGame() = default; //define virtual destructor, so when game is deleted, will call ~Game(), then ~IGame()
        LEGameEngine::IGameEngine* GameEngine = NULL;
        void SetGameEngine(LEGameEngine::IGameEngine* pGameEngine) { GameEngine = pGameEngine; }
        //std::weak_ptr<LEGameEngine::IGameEngine> engine; //weak_ptr: IGame only use engine, not responsible for engine's destruction
        //void SetApplication(std::weak_ptr<LEGameEngine::IGameEngine> pApplication) { engine = pApplication;}

        virtual void PreInitialize() {}
        virtual void Initialize() {}
        virtual void PostInitialize() {}
        virtual void Update() {}
        virtual void PostUpdate() {}
        virtual void RecordGraphicsCommandBuffer_RenderpassShadowmap(int renderpassIndex) {}
        virtual void RecordGraphicsCommandBuffer_RenderpassMainscene() { Record(); } //GameEngine will call this function. But the Game can override Record() instead and use this default function
        virtual void RecordComputeCommandBuffer() {}

        virtual void Record() {}; //same as RecordGraphicsCommandBuffer_RenderpassMainscene()

    };
}//namespace