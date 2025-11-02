#include "resourceCore.h"
#include "IApplication.h"

namespace LEResource{
void ResourceCore::SetApplication(LEApplication::IApplication* pApplication, LELog::ILogCore *logger_) {
    game = pApplication;
    logger = logger_;
    //std::cout<<"ResourceCore::SetApplication() Test"<<std::endl;
    //logger->Log("ResourceCore::SetApplication() Test");
}

void ResourceCore::SetDevice(VkDevice logicalDevice_, VkPhysicalDevice physicalDevice_, VkQueue graphicsQueue_){
    logicalDevice = logicalDevice_;
    physicalDevice = physicalDevice_;
    graphicsQueue = graphicsQueue_;
}

}//namespace