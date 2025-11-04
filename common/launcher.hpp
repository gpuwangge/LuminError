#pragma once
//If migrate to Linux, #include<dlfcn.h>
//dlopen(), dlerror(), dlsym(), and suffix .so instead of .dll, void* indead of HMODULE
//Or olternative: Boost.DLL
#include <windows.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
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
        auto game = std::make_unique<LuminError::Game>(); //will call Game's destructor at the end of main()
        game->SetApplication(instance_application);
        instance_application->SetGamer(game.get());
        //if(argc > 1) instance_application->Run(argv[1]); else 
        instance_application->Run(std::string(EXAMPLE_NAME));//need examplename to find yaml files and create log file
    } catch (const std::exception& e) {
        std::cerr << "Exception in Application::Run(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Application::Run()" << std::endl;
    }
    
    DestroyInstance(instance_application); //engine will be destroyed in main()
    FreeLibrary(handle_application);
    //std::cout<<"- FreeLibrary: Application."<<std::endl;

    return 0; //game(by smart pointer) will be destoyed in when main() ends
}
