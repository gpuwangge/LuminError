#ifndef H_CONTEXT
#define H_CONTEXT

#include "common.h"
#include "physicalDevice.h"
#include "logManager.h"

#ifdef ANDROID
#include "..\\..\\androidFramework\\include\\androidFileManager.h"
#endif

#define PRINT CContext::GetHandle().logManager.print

//CContext is to store some commonly used static variables(physical device, logical device in the current context)

//Member functions are defined as static functions.
//Static function has no "this" pointer so it can't visit class member variable, but can visit static member variable
//Static functions can be called using: CContext::func()

//CContext dont need to instantialize like normal class. When CContext::Init() is called, it create its "only" instance.
//You can use that "only" instance to visit the "only" variables 
class CContext{
public:
    static void Init();
    static void Quit();
    static CContext& GetHandle();

    ~CContext();

    std::unique_ptr<CPhysicalDevice>* physicalDevice; //instance picks a physical device and passes it here

    //All the following contents are related to the current-context physical device
    VkPhysicalDevice GetPhysicalDevice();
    VkDevice GetLogicalDevice();
    VkQueue GetGraphicsQueue();
    VkQueue GetPresentQueue();
    VkQueue GetComputeQueue();

    CLogManager logManager;

#ifdef ANDROID
    CAndroidFileManager androidFileManager;
#endif
    
private:
    CContext();//set construct to private so no one can instanize this class.

    static std::unique_ptr<CContext> handle;
};

#endif