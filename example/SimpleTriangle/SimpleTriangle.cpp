#include "ISimpleTriangle.h"
#include <iostream>

namespace LEExample{
    struct SimpleTriangle : public ISimpleTriangle {
        void Record() override{
            for(int i = 0; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts();

            //game->Greet();//works
            //game->GetTextManager().Draw();//compile error, unable to find out Draw()
            
        }
    };

    //extern "C" void* CreateInstance(){ return new SimpleTriangle();}
    //extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<SimpleTriangle*>(p); std::cout<<"- Destroy Instance SimpleTriangle."<<std::endl;}}
    EXPORT_FACTORY_FOR(SimpleTriangle)
}