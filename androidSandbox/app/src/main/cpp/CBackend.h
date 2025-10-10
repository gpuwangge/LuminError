#include "..\\..\\..\\..\\..\\samples\\android\\androidSample.h"

#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

namespace c_backend {
#define LOG_TAG "CBackend"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define C_CHECK(x)                           \
  do {                                        \
    VkResult err = x;                         \
    if (err) {                                \
      LOGE("Detected C error: %d", err); \
      abort();                                \
    }                                         \
  } while (0)

struct ANativeWindowDeleter {
  void operator()(ANativeWindow *window) { ANativeWindow_release(window); }
};

class CBackend {
 public:
  void init();
  void render();
  void cleanup();
  void reset(ANativeWindow *newWindow, AAssetManager *newManager);
  bool initialized = false;

 private:
    TEST_CLASS_NAME sample;
    std::unique_ptr<ANativeWindow, ANativeWindowDeleter> window;
};

void CBackend::init() {
    //__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "The value of 1 + 1 is %d", 1+1);
    initialized = true;
    //CContext::Init();
    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    std::vector<const char*> requiredInstanceExtensions;
    requiredInstanceExtensions.push_back("VK_KHR_surface");
    requiredInstanceExtensions.push_back("VK_KHR_android_surface");
    sample.instance = std::make_unique<CInstance>(requiredValidationLayers,requiredInstanceExtensions);

    assert(window != nullptr);  // window not initialized
    const VkAndroidSurfaceCreateInfoKHR create_info{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .window = window.get()};
    C_CHECK(vkCreateAndroidSurfaceKHR(sample.instance->getHandle(), &create_info,nullptr /* pAllocator */, &sample.surface));

    VkQueueFlagBits requiredQueueFamilies = VK_QUEUE_GRAPHICS_BIT; //VK_QUEUE_GRAPHICS_BIT & VK_QUEUE_COMPUTE_BIT
    const std::vector<const char*>  requireDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    sample.instance->findAllPhysicalDevices();

    CContext::GetHandle().physicalDevice = sample.instance->pickSuitablePhysicalDevice(sample.surface, requireDeviceExtensions, requiredQueueFamilies);

    CContext::GetHandle().physicalDevice->get()->createLogicalDevices(sample.surface, requiredValidationLayers, requireDeviceExtensions);

    //Change this values will change fov if sample test sets perspective matrix
    sample.windowWidth = 1080;
    sample.windowHeight = 2400;
    //TODO: how to get window size here

    //TODO: because app run in full-screen, this setting doesn't matter(confirm?)
    sample.swapchain.createSwapchainImages(sample.surface, sample.windowWidth, sample.windowHeight);
    sample.swapchain.createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);

    sample.initialize();
}


void CBackend::reset(ANativeWindow *newWindow, AAssetManager *newManager) {
    window.reset(newWindow);
    CContext::Init();
    CContext::GetHandle().androidFileManager.assetManager = newManager;
  //if (initialized) {
  //    app.createSurface();
  //  recreateSwapChain();
  //}
}

// from android samples
/* return current time in milliseconds */
static double now_ms(void) {
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}

void CBackend::render() {//mainloop() equivalent
    //Count FPS
    static double startTime = now_ms(); // start time
    static int frameCount = 0;
    double currentTime = now_ms(); // finish time
    double elapseTime = currentTime - startTime; // time your code took to exec in ms
    if(elapseTime > 1000) {
        __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "ElapseTime: %fms, frameCount %d", elapseTime, frameCount);
        frameCount = 0;
        startTime = currentTime;
    }else frameCount++;


    sample.UpdateRecordRender();

    //sample.update();
    //if(sample.renderProcess.bCreateGraphicsPipeline) {
    //    sample.renderer.preRecordGraphicsCommandBuffer(sample.swapchain);
    //    sample.recordGraphicsCommandBuffer();
    //    sample.renderer.postRecordGraphicsCommandBuffer(sample.swapchain);
    //}
    //if(sample.renderProcess.bCreateComputePipeline){
    //    sample.renderer.preRecordComputeCommandBuffer();
    //    sample.recordComputeCommandBuffer();
    //    sample.renderer.postRecordComputeCommandBuffer();
    //}
    //sample.postUpdate();

    //sample.renderer.Update(); //update currentFrame
    //__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "currentFrame:  %d", sample.renderer.currentFrame);


    if (sample.NeedToExit) std::terminate(); //TODO: is there a better exit function?

}

void CBackend::cleanup() {
    vkDeviceWaitIdle(CContext::GetHandle().GetLogicalDevice());
    sample.CleanUp();
    initialized = false;
}

}  // namespace c_backend