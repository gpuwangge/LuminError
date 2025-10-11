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


    HMODULE handle_application = LoadLibraryA("application.dll"); //Windows.h
    if(!handle_application) { 
        std::cerr << "Module load failed! Module Name = application.dll" << std::endl; //Windows.h
        return -1; 
    }

    using CreateAppFunc = LEApplication::IApplication*(*)();
    auto CreateInstance =  (CreateAppFunc)GetProcAddress(handle_application, "CreateInstance");
    if(!CreateInstance) { 
        std::cerr << "GetProcAddress failed! (CreateInstance)" << std::endl;
        FreeLibrary(handle_application);
        std::cout<<"FreeLibrary: Application"<<std::endl;
        return -1;
    }
    using DestroyAppFunc = void(*)(void*);
    auto DestroyInstance =  (DestroyAppFunc)GetProcAddress(handle_application, "DestroyInstance");
    if(!DestroyInstance) { 
        std::cerr << "GetProcAddress failed! (DestroyInstance)" << std::endl;
        FreeLibrary(handle_application);
        std::cout<<"FreeLibrary: Application"<<std::endl;
        return -1;
    }

    LEApplication::IApplication* instance_application = (LEApplication::IApplication*)CreateInstance();
    try {
        if(argc > 1) instance_application->Run(argv[1]);
        else instance_application->Run();
    } catch (const std::exception& e) {
        std::cerr << "Exception in Application::Run(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Application::Run()" << std::endl;
    }
    
    DestroyInstance(instance_application);
    FreeLibrary(handle_application);
    std::cout<<"- FreeLibrary: Application."<<std::endl;

    return 0;
}
