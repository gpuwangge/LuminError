#ifndef H_SDLMANAGER
#define H_SDLMANAGER

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "common.h"
#include "camera.hpp"
#include "instance.h"

class CApplication;

class CSDLManager final{
public:
    CSDLManager(){};
    ~CSDLManager(){};

    CApplication *m_pApp;

    //GLFWwindow * window;
    SDL_Window* window;
    int m_windowWidth, m_windowHeight;
    int m_windowCenterX, m_windowCenterY;

    //TTF_Font* m_font;

    bool bStillRunning = true;
    bool bKeyDown = false;
    bool bMouseDown = false;

    float keyboard_sensitive = 3;
    float mouse_sensitive = 60;//0.2f;
    
    //float previous_mouse_x = 0;
    //float previous_mouse_y = 0;
    //bool bFirstPersonMouseRotate = false;

    void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle);
    void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions);
    void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface);
    //void createFonts(int fontSize);

    void eventHandle();
};

#endif
