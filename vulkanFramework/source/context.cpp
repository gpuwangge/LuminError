#include "../include/context.h"

std::unique_ptr<CContext> CContext::handle = nullptr;

void CContext::Init(){ handle.reset(new CContext);}
void CContext::Quit(){ handle.reset(); }//to nullptr
CContext& CContext::GetHandle(){ return *handle;}

VkPhysicalDevice CContext::GetPhysicalDevice(){
    return physicalDevice->get()->getHandle();
}
VkDevice CContext::GetLogicalDevice(){
    return physicalDevice->get()->getLogicalDevice();
}
VkQueue CContext::GetGraphicsQueue(){
    return physicalDevice->get()->getGraphicsQueue();
}
VkQueue CContext::GetPresentQueue(){
    return physicalDevice->get()->getPresentQueue();
}
VkQueue CContext::GetComputeQueue(){
    return physicalDevice->get()->getComputeQueue();
}


CContext::CContext(){
#ifndef ANDROID
    logManager.setLogFile("context.log");
#endif
}
CContext::~CContext(){}
