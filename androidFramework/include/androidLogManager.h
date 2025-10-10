#ifndef H_ANDROIDLOGMANAGER
#define H_ANDROIDLOGMANAGER

#include "..\\..\\framework\\include\\common.h"
#include <android/log.h>

class CAndroidLogManager {
public:
	CAndroidLogManager();
    ~CAndroidLogManager();

    void printLog(std::string s);
	void printLog(std::string s, int n);
    void printLog(std::string s, int n0, int n1);
    void printLog(std::string s, int n0, int n1, int n2);
	void printLog(std::string s, float n);
    void printLog(std::string s, float n0, float n1);
	void printLog(std::string s0, std::string s1);
    void printLog(std::string s, float *n, int size);
    void printLog(std::string s, int *n, int size);
    void printLog(std::string s, unsigned long int n);
    void printLog(std::string s, unsigned int n);
};

#endif