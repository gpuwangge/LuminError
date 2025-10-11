#include "ICommon.h"
#include "ISimpleTriangle.h"

namespace LEExample{
    struct SimpleTriangle : public ISimpleTriangle {
        void Update() override{
            //std::cout<<"SimpleTriangle::Update()"<<std::endl; 
        }

        void Record() override{
            
            //for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	        //    textManager.Draw();
        }
    };

    extern "C" void* CreateInstance(){ return new SimpleTriangle();}
    extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<SimpleTriangle*>(p); std::cout<<"- Destroy Instance SimpleTriangle."<<std::endl;}}
}