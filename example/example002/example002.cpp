#include "ICommon.h"
#include "IExample002.h"

namespace LEExample{
    struct Example002 : public IExample002 {
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) override {game = pApplication;}
        
        void Update() override{
            std::cout<<"Example002::Update()"<<std::endl;
        }
    };

    extern "C" void* CreateInstance(){ return new Example002();}
    extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<Example002*>(p); std::cout<<"- Destroy Instance Example002."<<std::endl;}}
}