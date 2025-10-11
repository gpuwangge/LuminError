#include "ICommon.h"
#include "ISimpleTriangle.h"
#include "IApplication.h"

namespace LEExample{
    struct SimpleTriangle : public ISimpleTriangle {
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) override {game = pApplication;}

        void Record() override{
            for(int i = 0; i < game->GetObjectSize(); i++) game->DrawObject(i);
            game->DrawTexts();

            //game->Greet();//works
            //game->GetTextManager().Draw();//compile error, unable to find out Draw()
            
        }
    };

    extern "C" void* CreateInstance(){ return new SimpleTriangle();}
    extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<SimpleTriangle*>(p); std::cout<<"- Destroy Instance SimpleTriangle."<<std::endl;}}
}