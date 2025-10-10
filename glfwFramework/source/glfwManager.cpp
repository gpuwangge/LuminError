#include "../include/glfwManager.h"
#include "../../vulkanFramework/include/application.h"

#ifndef M_PI
#define M_PI  3.14159265f
#endif

// multiplication factors for input interaction:
// //  (these are known from previous experience)
const float ANGFACT = { M_PI / 180.f };
const float SCLFACT = { 0.005f };

// active mouse buttons (or them together):
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

// minimum allowable scale factor:
const float MINSCALE = { 0.05f };


//bool CApplication::NeedToExit = false;
//Camera CApplication::mainCamera;

// bool NeedToExit = false;
// Camera mainCamera;

float CGLFWManager::keyboard_sensitive = 3;
float CGLFWManager::mouse_sensitive = 60;

CGLFWManager::CGLFWManager(){
	m_windowHeight = 0;
    m_windowHeight = 0;
}
CGLFWManager::~CGLFWManager(){
	glfwDestroyWindow(window);
    glfwTerminate();
}

void CGLFWManager::GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods) {
	//printf("controller!\n");

	static bool				UseIndexBuffer;			// true = use both vertex and index buffer, false = just use vertex buffer
	static bool				UseLighting;			// true = use lighting for display
	static bool				UseRotate;			// true = rotate-animate, false = use mouse for interaction
	static bool				Paused;				// true means don't animate
	static int				Mode;				// 0 = use colors, 1 = use textures, ...
	static bool				Verbose;			// true = write messages into a file

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_DOWN: 
			CApplication::mainCamera.Velocity.y = 1 * keyboard_sensitive;
			break;
		case GLFW_KEY_UP: 
			CApplication::mainCamera.Velocity.y = -1 * keyboard_sensitive;
			break;
		case 'w':
		case 'W':
			CApplication::mainCamera.Velocity.z = 1 * keyboard_sensitive;
			break;
		case 's':
		case 'S':
			CApplication::mainCamera.Velocity.z = -1 * keyboard_sensitive;
			break;
		case GLFW_KEY_LEFT:
		case 'a':
		case 'A':
			CApplication::mainCamera.Velocity.x = 1 * keyboard_sensitive; 
			break;
		case GLFW_KEY_RIGHT:
		case 'd':
		case 'D':
			CApplication::mainCamera.Velocity.x = -1 * keyboard_sensitive; 
			break;

		case 'f':
		case 'F':
			if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) CApplication::mainCamera.cameraType = Camera::CameraType::FREE;
        	else if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE) CApplication::mainCamera.cameraType = Camera::CameraType::LOCK;
            break;

		case GLFW_KEY_TAB:
			if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) {
				CApplication::mainCamera.focusObjectId += 1;
				CApplication::mainCamera.focusObjectId = CApplication::mainCamera.focusObjectId % CApplication::objects.size();
			}
			break;

		case GLFW_KEY_ESCAPE:
			//CApplication::NeedToExit = true;
			glfwSetWindowShouldClose(window, true);
			break;

		case 'i':
		case 'I':
			UseIndexBuffer = !UseIndexBuffer;
			break;

		case 'l':
		case 'L':
			UseLighting = !UseLighting;
			break;

		case 'm':
		case 'M':
			Mode++;
			if (Mode >= 2)
				Mode = 0;
			break;

		case 'p':
		case 'P':
			Paused = !Paused;
			break;

		case 'r':
		case 'R':
			UseRotate = !UseRotate;
			break;

		case 'v':
		case 'V':
			Verbose = !Verbose;
			break;

		default:
			//fprintf(FpDebug, "Unknown key hit: 0x%04x = '%c'\n", key, key);
			//fprintf(stderr, "Unknown key hit: 0x%04x = '%c'\n", key, key);
			//fflush(FpDebug);
			break;
		}
	}

	if (action == GLFW_RELEASE) {	
		switch (key) {
		case GLFW_KEY_DOWN:
			CApplication::mainCamera.Velocity.y = 0;
			break;
		case GLFW_KEY_UP:
			CApplication::mainCamera.Velocity.y = 0;
			break;
		case 'w':
		case 'W':
			CApplication::mainCamera.Velocity.z = 0;
			break;
		case 's':
		case 'S':
			CApplication::mainCamera.Velocity.z = 0;
			break;
		case GLFW_KEY_LEFT:
		case 'a':
		case 'A':
			CApplication::mainCamera.Velocity.x = 0;
			break;
		case GLFW_KEY_RIGHT:
		case 'd':
		case 'D':
			CApplication::mainCamera.Velocity.x = 0;
			break;
		default:
			break;
		}
	}

}

void CGLFWManager::GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos){
	static float lastX = 0.0;
	static float lastY = 0.0;
	static bool firstMouseMotion = true;

	if(firstMouseMotion){
		lastX = xpos;
		lastY = ypos;
		firstMouseMotion = false;
	}

	float deltaX = xpos - lastX;
	float deltaY = ypos - lastY;
	//std::cout <<std::fixed<< "Relative Movement: DeltaX = " << deltaX << ", DeltaY = " << deltaY << std::endl;

	lastX = xpos;
	lastY = ypos;

	if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE){
        CApplication::mainCamera.AngularVelocity.x = -mouse_sensitive*deltaY;
        CApplication::mainCamera.AngularVelocity.y = mouse_sensitive*deltaX;
    }


	/* Legacy code
	static int				Xmouse, Ymouse;			// mouse values
	static float				Xrot, Yrot;			// rotation angles in degrees
	static int				ActiveButton;			// current button that is down
	static float				Scale;				// scaling facto 

	int dx = (int)xpos - Xmouse;		// change in mouse coords
	int dy = (int)ypos - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = (int)xpos;			// new current position
	Ymouse = (int)ypos;
	*/
	
	
}

void CGLFWManager::GLFWMouseButton(GLFWwindow *window, int button, int action, int mods) {
	static int				Xmouse, Ymouse;			// mouse values
	static int				ActiveButton;			// current button that is down

	//if (Verbose)		fprintf(FpDebug, "Mouse button = %d; Action = %d\n", button, action);

	int b = 0;		// LEFT, MIDDLE, or RIGHT


					// get the proper button bit mask:

	switch (button){
	case GLFW_MOUSE_BUTTON_LEFT:
		b = LEFT;		
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		b = MIDDLE;		
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		b = RIGHT;		
		break;
	default:
		b = 0;
		//fprintf(FpDebug, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:

	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Xmouse = (int)xpos;
		Ymouse = (int)ypos;
		ActiveButton |= b;		// set the proper bit

		if(CApplication::mainCamera.cameraType == Camera::CameraType::LOCK) {
			CApplication::mainCamera.cameraType = Camera::CameraType::FREE;
			//std::cout<<"Free Mode"<<std::endl;
		}
        else if(CApplication::mainCamera.cameraType == Camera::CameraType::FREE) {
			CApplication::mainCamera.cameraType = Camera::CameraType::LOCK;
			//std::cout<<"Lock Mode"<<std::endl;
		}

	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}

void CGLFWManager::GLFWMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    //std::cout << "Scroll Event: xoffset = " << xoffset << ", yoffset = " << yoffset << std::endl;

	//vertical
    if (yoffset > 0) {
        //std::cout << "Scrolled Up" << std::endl;
        CApplication::mainCamera.MoveForward(1, 2);
    } else if (yoffset < 0) {
        //std::cout << "Scrolled Down" << std::endl;
        CApplication::mainCamera.MoveBackward(1, 2);
    }
	//horizontal
	if (xoffset > 0) {
		CApplication::mainCamera.MoveRight(1, 2);
    } else if (xoffset < 0) {
		CApplication::mainCamera.MoveLeft(1, 2);
    }

}


static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<CApplication*>(glfwGetWindowUserPointer(window));
	//app->framebufferResized = true;
}

void CGLFWManager::createWindow(int &windowWidth, int &windowHeight, std::string windowTitle){
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		glfwGetFramebufferSize(window, &m_windowWidth, &m_windowHeight);
		
		glfwSetKeyCallback(window, GLFWKeyboard);
		glfwSetCursorPosCallback(window, GLFWMouseMotion);
		glfwSetMouseButtonCallback(window, GLFWMouseButton);
		glfwSetScrollCallback(window, GLFWMouseScroll);

		windowWidth = m_windowWidth;
		windowHeight = m_windowHeight;

		//GLFW_CURSOR_NORMAL
		//GLFW_CURSOR_HIDDEN: hide curser
		//GLFW_CURSOR_DISABLED: hide curser, and refine to window
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		const HWND hDesktop = GetDesktopWindow();
		RECT desktop;
		GetWindowRect(hDesktop, &desktop);
		int horizontal = desktop.right;
    	int vertical = desktop.bottom;
		glfwSetWindowPos(window, horizontal/2, 50);
}

void CGLFWManager::queryRequiredInstanceExtensions(std::vector<const char*> &requiredInstanceExtensions){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //requiredInstanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	requiredInstanceExtensions.resize(glfwExtensionCount);
	for(int i = 0; i < glfwExtensionCount; i++)
		requiredInstanceExtensions[i] = glfwExtensions[i];
}

void CGLFWManager::createSurface(std::unique_ptr<CInstance> &instance, VkSurfaceKHR &surface) {
    if (glfwCreateWindowSurface(instance->getHandle(), window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}