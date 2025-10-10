#include "IApplication.h"

//If migrate to Linux, #include<dlfcn.h>
//dlopen(), dlerror(), dlsym(), and suffix .so instead of .dll, void* indead of HMODULE
//Or olternative: Boost.DLL
#include <windows.h>

#include <iostream>


int main(int argc, char* argv[]) {
    // std::cout << "Number of args: " << argc << "\n";
    // for (int i = 0; i < argc; ++i) {
    //     std::cout << "Arg " << i << ": " << argv[i] << "\n";
    // }


    HMODULE dll = LoadLibraryA("application.dll"); //Windows.h
    if(!dll) { 
        std::cerr << "DLL load failed! DLL Name = application.dll" << std::endl; //Windows.h
        return -1; 
    }

    using CreateAppFunc = LEApplication::IApplication*(*)();
    auto CreateInstance =  (CreateAppFunc)GetProcAddress(dll, "CreateInstance");
    if(!CreateInstance) { 
        std::cerr << "GetProcAddress failed! (CreateInstance)" << std::endl;
        FreeLibrary(dll);
        return -1;
    }
    using DestroyAppFunc = void(*)(void*);
    auto DestroyInstance =  (DestroyAppFunc)GetProcAddress(dll, "DestroyInstance");
    if(!DestroyInstance) { 
        std::cerr << "GetProcAddress failed! (DestroyInstance)" << std::endl;
        FreeLibrary(dll);
        return -1;
    }

    LEApplication::IApplication* p = (LEApplication::IApplication*)CreateInstance();
    try {
        if(argc > 1) p->Run(argv[1]);
        else p->Run();
    } catch (const std::exception& e) {
        std::cerr << "Exception in Application::Run(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Application::Run()" << std::endl;
    }
    
    DestroyInstance(p);
    FreeLibrary(dll);

    return 0;
}
