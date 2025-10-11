#include "ICommon.h"
#include "ISimpleTriangle.h"
#include "IApplication.h"
#include "../../application/include/application.h"
#include "../../application/include/textManager.h"

namespace LEExample{
    struct SimpleTriangle : public ISimpleTriangle {
        LEApplication::IApplication* game;
        void SetApplication(LEApplication::IApplication* pApplication) override {game = pApplication;}

        void Update() override{
            //std::cout<<"SimpleTriangle::Update()"<<std::endl; 
        }

        void Record() override{
            
            //for(int i = 0; i < objects.size(); i++) objects[i].Draw();
	        //    textManager.Draw();
            CTextManager test = game->GetTextManager();
            //test.Draw(); //ISSUE: cant find Draw's implementation
        }
    };

    extern "C" void* CreateInstance(){ return new SimpleTriangle();}
    extern "C" void DestroyInstance(void *p){ if(p) {delete static_cast<SimpleTriangle*>(p); std::cout<<"- Destroy Instance SimpleTriangle."<<std::endl;}}
}