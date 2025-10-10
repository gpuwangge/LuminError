#include "../include/logManager.h"


CLogManager::CLogManager(){}
CLogManager::~CLogManager(){  }

#ifndef ANDROID
#include <sys/stat.h>
void CLogManager::setLogFile(std::string s){
    mkdir("../logs");
    std::string fullname = "../logs/" + s;
    desktopLogManager.createLogFile(fullname);
}
#endif

void CLogManager::print(std::string s){
#ifndef ANDROID
    desktopLogManager.printLog(s);
#else
    androidLogManager.printLog(s);
#endif
}

void CLogManager::print(std::string s, int n){
#ifndef ANDROID
    desktopLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif    
}

void CLogManager::print(std::string s, int n0, int n1){
#ifndef ANDROID
    desktopLogManager.printLog(s, n0, n1);
#else
    androidLogManager.printLog(s, n0, n1);
#endif    
}

void CLogManager::print(std::string s, int n0, int n1, int n2){
#ifndef ANDROID
    desktopLogManager.printLog(s, n0, n1, n2);
#else
    androidLogManager.printLog(s, n0, n1, n2);
#endif    
}

void CLogManager::print(std::string s, float n){
#ifndef ANDROID
    desktopLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif    
}

void CLogManager::print(std::string s, float n0, float n1){
#ifndef ANDROID
    desktopLogManager.printLog(s, n0, n1);
#else
    androidLogManager.printLog(s, n0, n1);
#endif  
}

void  CLogManager::print(std::string s, glm::vec3 vec){
#ifndef ANDROID
    desktopLogManager.printLog(s, vec);
#endif
}

void CLogManager::print(std::string s0, std::string s1){
#ifndef ANDROID
    desktopLogManager.printLog(s0, s1);
#else
    androidLogManager.printLog(s0, s1);
#endif 
}

void CLogManager::print(std::string s, float *n, int size){
#ifndef ANDROID
    desktopLogManager.printLog(s, n, size);
#else
    androidLogManager.printLog(s, n, size);
#endif 
}

void CLogManager::print(std::string s, int *n, int size){
#ifndef ANDROID
    desktopLogManager.printLog(s, n, size);
#else
    androidLogManager.printLog(s, n, size);
#endif 
}

void CLogManager::print(std::string s, unsigned long int n){
#ifndef ANDROID
    desktopLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif 
}

void CLogManager::print(std::string s, unsigned int n){
#ifndef ANDROID
    desktopLogManager.printLog(s, n);
#else
    androidLogManager.printLog(s, n);
#endif 
}