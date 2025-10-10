#include "../include/sdlManager.h"
#include "../../vulkanFramework/include/application.h"

void CSDLManager::createWindow(int &windowWidth, int &windowHeight, std::string windowTitle){
    if(!SDL_Init(SDL_INIT_VIDEO)) 
        std::cout << "Could not initialize SDL." << std::endl;
    
	windowWidth = WINDOW_WIDTH;
	windowHeight = WINDOW_HEIGHT;

    m_windowWidth = WINDOW_WIDTH;
    m_windowHeight = WINDOW_HEIGHT;
    m_windowCenterX = WINDOW_WIDTH/2;
    m_windowCenterY = WINDOW_HEIGHT/2;

    window = SDL_CreateWindow(windowTitle.c_str(), windowWidth, windowHeight, SDL_WINDOW_VULKAN);
    if(window == NULL) {
        std::cout << "Could not create SDL window." << std::endl;
        //return 1;
    }

     //SDL_SetWindowMouseGrab(window, true);
    SDL_SetWindowRelativeMouseMode(window, true);

	const HWND hDesktop = GetDesktopWindow();
    RECT desktop;
    GetWindowRect(hDesktop, &desktop);
    int horizontal = desktop.right;
    int vertical = desktop.bottom;

    SDL_SetWindowPosition(window, horizontal/2, 50);
}

void CSDLManager::queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions){
	Uint32 count_instance_extensions = 0;
    const char * const *instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);
    //std::vector<const char*> requiredInstanceExtensions;
    requiredInstanceExtensions.resize(count_instance_extensions);
    for(int i = 0; i < count_instance_extensions; i++) requiredInstanceExtensions[i] = instance_extensions[i];
}

void CSDLManager::createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface){
    if(!SDL_Vulkan_CreateSurface(window, instance->getHandle(), NULL, &surface)) {
        std::cout << "Could not create a Vulkan surface." << std::endl;
        //return 1;
    }
}

/*
void CSDLManager::createFonts(int fontSize){
    if (TTF_Init() == -1) 
       std::cout << "SDL_ttf could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    m_font = TTF_OpenFont("NotoSansCJK-VF.otf.ttc", fontSize);
    if (!m_font) 
        std::cout << "Failed to load font! SDL_Error: " << SDL_GetError() << std::endl;


    //test SDL text (Test only, for this is conflict with Vulkan)
   
    // SDL_Color white = {255, 255, 255, 255};
    // SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, "hello你好", 11, white); //textSurface->pixels is RGBA

    // int width  = textSurface->w;
    // int height = textSurface->h;
    // void* pixels = textSurface->pixels; //this saves the pixel data of the rendered text
    // int pitch = textSurface->pitch;


    // these code render the textSurface into the screen. Will conflict with Vulkan renderer. So don't use this
    // SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    // SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    // SDL_FRect textRect = {20, 20, textSurface->w, textSurface->h};
    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderClear(renderer);
    // SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    // SDL_RenderPresent(renderer);
    // TTF_CloseFont(m_font);
    // TTF_Quit();
    
}*/

void CSDLManager::eventHandle(){
    int ref_diff_x = 0, ref_diff_y = 0;  
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        //std::cout << "Debug: while loop: " <<debugCount<< std::endl;
        //to create smooth experience, reinitailize this inside loop so each loop this values are properly initialized
        //if put this outside of while loop, some sample(simplemipmap) will run into problem
        ref_diff_x = 0, ref_diff_y=0;
        switch(event.type) {
            case SDL_EVENT_KEY_DOWN:
                //std::cout<<"Key Down"<<event.key.key<<std::endl;
                switch(event.key.key){ //need 6 directions and 6 turns to translate camera
                    case SDLK_UP:
                        //CApplication::mainCamera.Velocity.y = -1 * keyboard_sensitive;
                        break;
                    case SDLK_DOWN:
                        //CApplication::mainCamera.Velocity.y = 1 * keyboard_sensitive;
                        break;
                    case SDLK_W:
                        //CApplication::mainCamera.Velocity.z = 1 * keyboard_sensitive;
                        break;
                    case SDLK_S:
                        //CApplication::mainCamera.Velocity.z = -1 * keyboard_sensitive;
                        break;
                    case SDLK_LEFT:
                    case SDLK_A:
                        //CApplication::mainCamera.Velocity.x = 1 * keyboard_sensitive; 
                        break;
                    case SDLK_RIGHT:
                    case SDLK_D:
                        //CApplication::mainCamera.Velocity.x = -1 * keyboard_sensitive; 
                        break;

                    case SDLK_F:/*
                        if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) CApplication::mainCamera.cameraType = Camera::CameraType::FREE;
                        else if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE) CApplication::mainCamera.cameraType = Camera::CameraType::LOCK;
                        */
                        break;
                    case SDLK_TAB:/*
                        if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) {
                            CApplication::mainCamera.focusObjectId += 1;
				            CApplication::mainCamera.focusObjectId = CApplication::mainCamera.focusObjectId % CApplication::objects.size();
                        }
                        */
                        break;
                    case SDLK_P:
                        /*
                        if(!m_pApp->appInfo.Feature.feature_graphics_enable_controls) break;
                        if(!m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls) {
                            m_pApp->appInfo.Feature.feature_graphics_show_performance_control = !m_pApp->appInfo.Feature.feature_graphics_show_performance_control;
                            m_pApp->controlNodes[0]->bVisible = m_pApp->appInfo.Feature.feature_graphics_show_performance_control;
                        }else{
                            m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls = false;
                            for(int i = 1; i < m_pApp->controlNodes.size(); i++) m_pApp->controlNodes[i]->bVisible = false;

                            m_pApp->appInfo.Feature.feature_graphics_show_performance_control = true;
                            m_pApp->controlNodes[0]->bVisible = true;
                        }*/
                        break;
                    case SDLK_H:/*
                        if(!m_pApp->appInfo.Feature.feature_graphics_enable_controls) break;
                        m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls = !m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls;
                        m_pApp->appInfo.Feature.feature_graphics_show_performance_control = m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls;
                        for(int i = 0; i < m_pApp->controlNodes.size(); i++) m_pApp->controlNodes[i]->bVisible = m_pApp->appInfo.Feature.feature_graphics_show_all_metric_controls;
                        */
                        break;
                    case SDLK_ESCAPE:
                        bStillRunning = false;
                        break; 
                }
                bKeyDown = true;
                break;
            case SDL_EVENT_KEY_UP:
                //std::cout<<"Key Up"<<event.key.scancode<<std::endl;
                switch(event.key.key){
                    case SDLK_UP:
                        //CApplication::mainCamera.Velocity.y = 0;
                        break;
                    case SDLK_DOWN:
                        //CApplication::mainCamera.Velocity.y = 0;
                        break;
                    case SDLK_W:
                        //CApplication::mainCamera.Velocity.z = 0;
                        break;
                    case SDLK_S:
                        //CApplication::mainCamera.Velocity.z = 0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_A:
                        //CApplication::mainCamera.Velocity.x = 0;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_D:
                        //CApplication::mainCamera.Velocity.x = 0;
                        break;
                }
                bKeyDown = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                //if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) CApplication::mainCamera.cameraType = Camera::CameraType::FREE;
                //else if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE) CApplication::mainCamera.cameraType = Camera::CameraType::LOCK;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                break;
            case SDL_EVENT_MOUSE_MOTION:
                //std::cout<<"SDL_EVENT_MOUSE_MOTION: "<<event.motion.yrel<<", "<<event.motion.xrel<<std::endl;
                ref_diff_x = -mouse_sensitive*event.motion.yrel;
                ref_diff_y = mouse_sensitive*event.motion.xrel;
                break;
            case SDL_EVENT_MOUSE_WHEEL:/*
                if(event.wheel.x == 1) CApplication::mainCamera.MoveLeft(1, 2);
                else if(event.wheel.x == -1) CApplication::mainCamera.MoveRight(1, 2);
                if(event.wheel.y == 1) CApplication::mainCamera.MoveForward(1, 2);
                else if(event.wheel.y == -1) CApplication::mainCamera.MoveBackward(1, 2);
                */
                break;
            case SDL_EVENT_QUIT:
                bStillRunning = false;
                break;
            default:
                // Do nothing
                break;
        }
    }/*
    if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE){
        CApplication::mainCamera.AngularVelocity.x = ref_diff_x;
        CApplication::mainCamera.AngularVelocity.y = ref_diff_y;
    }*/

    SDL_Delay(1);//delay 1ms. otherwise it could run millians of times per second
}