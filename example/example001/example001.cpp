#include "ICommon.h"
#include "IExample001.h"

namespace LEExample{
    struct Example001 : public IExample001 {
        void Update() override{
            std::cout<<"Example001::Update()"<<std::endl; 
        }
    };

    extern "C" void* CreateInstance(){ return new Example001();}
    extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<Example001*>(p); std::cout<<"- Destroy Instance Example001."<<std::endl;}}
}