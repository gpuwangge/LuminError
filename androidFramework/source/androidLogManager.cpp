#include "../include/androidLogManager.h"

#define LOG_TAG "VULKAN_PLATFORM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

CAndroidLogManager::CAndroidLogManager(){}
CAndroidLogManager::~CAndroidLogManager(){}


void CAndroidLogManager::printLog(std::string s){
    LOGV("%s", s.c_str());
}

void CAndroidLogManager::printLog(std::string s, int n){
    LOGV(s.c_str(), n);
}

void CAndroidLogManager::printLog(std::string s, int n0, int n1){
    LOGV(s.c_str(), n0, n1);
}

void CAndroidLogManager::printLog(std::string s, int n0, int n1, int n2){
    LOGV(s.c_str(), n0, n1, n2);
}

void CAndroidLogManager::printLog(std::string s, float n){
    LOGV(s.c_str(), n);
}

void CAndroidLogManager::printLog(std::string s, float n0, float n1){
    LOGV(s.c_str(), n0, n1);
}

void CAndroidLogManager::printLog(std::string s0, std::string s1){
    LOGV(s0.c_str(), s1.c_str());
}


void CAndroidLogManager::printLog(std::string s, float *n, int size){
    LOGV("%s", s.c_str());
    std::string nstr;
    for(int i = 0; i < size; i++)
        nstr += std::to_string(n[i]) + ", ";
    LOGV("%s", nstr.c_str());
}

void CAndroidLogManager::printLog(std::string s, int *n, int size){
    LOGV("%s", s.c_str());
    std::string nstr;
    for(int i = 0; i < size; i++)
        nstr += std::to_string(n[i]) + ", ";
    LOGV("%s", nstr.c_str());
}

void CAndroidLogManager::printLog(std::string s, unsigned long int n){
    LOGV(s.c_str(), n);
}

void CAndroidLogManager::printLog(std::string s, unsigned int n){
    LOGV(s.c_str(), n);
}