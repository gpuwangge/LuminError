#pragma once
//#include <string>
//#include <sstream>
#include <iostream>
// #include <iomanip>
// #include "Utility.h"

namespace LEApplication{
    class IApplication;
}

namespace LEResource{
    class IResourceCore {
    public:
        virtual ~IResourceCore() = default;
        void SetApplication(LEApplication::IApplication* pApplication) {game = pApplication;}
        void Greet(){std::cout<<"resourcecore::greet!"<<std::endl;}

    protected:
        LEApplication::IApplication* game;

    }; //class

    #define EXPORT_FACTORY_FOR(ClassName) \
        extern "C" void* CreateInstance() { return new ClassName(); } \
        extern "C" void DestroyInstance(void* p) { if (p) delete static_cast<ClassName*>(p); }
} //namespace
