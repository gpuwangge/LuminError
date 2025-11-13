#pragma once

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

#include "ISDLCore.h"

#include <memory>
#include <vector>
#include <iostream>

namespace LESDL{
    class SDLCore final : public ISDLCore{
    public:
        SDLCore(){}
        ~SDLCore(){}

        SDL_Window* window;
        int m_windowWidth, m_windowHeight;
        int m_windowCenterX, m_windowCenterY;

        //TTF_Font* m_font;

        bool bStillRunning = true;
        bool bKeyDown = false;
        bool bMouseDown = false;

        float keyboard_sensitive = 3;
        float mouse_move_sensitive = 60;//0.2f;
        float mouse_wheel_sensitive = 1;
        
        //float previous_mouse_x = 0;
        //float previous_mouse_y = 0;
        //bool bFirstPersonMouseRotate = false;

        bool IsRunning() override {return bStillRunning;}
        void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle) override;
        void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions) override;
        void createSurface(VkInstance instance, VkSurfaceKHR &surface) override;
        void eventHandle() override;

        void SetMouseSensibility(float value) override { mouse_move_sensitive = value; mouse_wheel_sensitive = value/60.0f; }
        void SetKeyboardSensibility(float value) override {keyboard_sensitive = value; }
        
    };
    EXPORT_FACTORY_FOR(SDLCore);
}

