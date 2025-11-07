#pragma once
//If migrate to Linux, #include<dlfcn.h>
//dlopen(), dlerror(), dlsym(), and suffix .so instead of .dll, void* indead of HMODULE
//Or olternative: Boost.DLL
#include <windows.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    HMODULE handle_gameEngine= LoadLibraryA("gameengine.dll"); //Windows.h
    if(!handle_gameEngine) { 
        std::cerr << "Module load failed! Module Name = gameengine.dll" << std::endl; //Windows.h
        return -1; 
    }

    using CreateGameEngineFunc = LEGameEngine::IGameEngine*(*)();
    auto CreateInstance =  (CreateGameEngineFunc)GetProcAddress(handle_gameEngine, "CreateInstance");
    if(!CreateInstance) { 
        std::cerr << "GetProcAddress failed! (CreateInstance)" << std::endl;
        FreeLibrary(handle_gameEngine);
        std::cout<<"FreeLibrary: handle_gameEngine"<<std::endl;
        return -1;
    }
    using DestroyAppFunc = void(*)(void*);
    auto DestroyInstance =  (DestroyAppFunc)GetProcAddress(handle_gameEngine, "DestroyInstance");
    if(!DestroyInstance) { 
        std::cerr << "GetProcAddress failed! (DestroyInstance)" << std::endl;
        FreeLibrary(handle_gameEngine);
        std::cout<<"FreeLibrary: Application"<<std::endl;
        return -1;
    }

    LEGameEngine::IGameEngine* gameEngine = (LEGameEngine::IGameEngine*)CreateInstance();
    try {
        auto gameContent = std::make_unique<LuminError::Game>(); //will call Game's destructor at the end of main(). Need complete declaration of Game.
        gameContent->SetGameEngine(gameEngine);
        gameEngine->SetGameContent(gameContent.get());
        //if(argc > 1) instance_application->Run(argv[1]); else 
        gameEngine->Run(std::string(EXAMPLE_NAME));//need examplename to find yaml files and create log file
    } catch (const std::exception& e) {
        std::cerr << "Exception in Application::Run(): " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in Application::Run()" << std::endl;
    }
    
    DestroyInstance(gameEngine); //gameEngine will be destroyed in main()
    FreeLibrary(handle_gameEngine);
    //std::cout<<"- FreeLibrary: Application."<<std::endl;

    return 0; //game(by smart pointer) will be destoyed in when main() ends
}
