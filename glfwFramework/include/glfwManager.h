#ifndef H_GLFWMANAGER
#define H_GLFWMANAGER

// Enable the WSI extensions
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../../vulkanFramework/include/common.h"
#include "../../vulkanFramework/include/instance.h"
#include "camera.hpp"

class CGLFWManager final{

public:
    CGLFWManager();
    ~CGLFWManager();

    GLFWwindow * window;
    int m_windowWidth, m_windowHeight;

    static float keyboard_sensitive;
    static float mouse_sensitive;

    //static int ref_diff_x, ref_diff_y; 

    void createWindow(int &windowWidth, int &windowHeight, std::string windowTitle);
    void queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions);
    void createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface);

    static void GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos);
	static void GLFWMouseButton(GLFWwindow *window, int button, int action, int mods);
    static void GLFWMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
};

#endif
