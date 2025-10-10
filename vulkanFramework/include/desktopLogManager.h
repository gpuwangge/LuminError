#ifndef H_DESKTOPLOGMANAGER
#define H_DESKTOPLOGMANAGER

//#pragma once
// #define HERE_I_AM(s) { debugger->writeMSG("\n***** %s *****\n", s); debugger->flush(); }
// #define REPORT(s) { debugger->printVkError( result, s );  debugger->flush(); }

#include "common.h"

struct errorcode
{
	VkResult	resultCode;
	std::string	meaning;
};

static errorcode ErrorCodes[] =
{
	{ VK_NOT_READY,				"Not Ready" },
	{ VK_TIMEOUT,				"Timeout" },
	{ VK_EVENT_SET,				"Event Set" },
	{ VK_EVENT_RESET,			"Event Reset" },
	{ VK_INCOMPLETE,			"Incomplete" },
	{ VK_ERROR_OUT_OF_HOST_MEMORY,		"Out of Host Memory" },
	{ VK_ERROR_OUT_OF_DEVICE_MEMORY,	"Out of Device Memory" },
	{ VK_ERROR_INITIALIZATION_FAILED,	"Initialization Failed" },
	{ VK_ERROR_DEVICE_LOST,			"Device Lost" },
	{ VK_ERROR_MEMORY_MAP_FAILED,		"Memory Map Failed" },
	{ VK_ERROR_LAYER_NOT_PRESENT,		"Layer Not Present" },
	{ VK_ERROR_EXTENSION_NOT_PRESENT,	"Extension Not Present" },
	{ VK_ERROR_FEATURE_NOT_PRESENT,		"Feature Not Present" },
	{ VK_ERROR_INCOMPATIBLE_DRIVER,		"Incompatible Driver" },
	{ VK_ERROR_TOO_MANY_OBJECTS,		"Too Many Objects" },
	{ VK_ERROR_FORMAT_NOT_SUPPORTED,	"Format Not Supported" },
	{ VK_ERROR_FRAGMENTED_POOL,		"Fragmented Pool" },
	{ VK_ERROR_SURFACE_LOST_KHR,		"Surface Lost" },
	{ VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,	"Native Window in Use" },
	{ VK_SUBOPTIMAL_KHR,			"Suboptimal" },
	{ VK_ERROR_OUT_OF_DATE_KHR,		"Error Out of Date" },
	{ VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,	"Incompatible Display" },
	{ VK_ERROR_VALIDATION_FAILED_EXT,	"Valuidation Failed" },
	{ VK_ERROR_INVALID_SHADER_NV,		"Invalid Shader" },
	{ VK_ERROR_OUT_OF_POOL_MEMORY_KHR,	"Out of Pool Memory" },
	{ VK_ERROR_INVALID_EXTERNAL_HANDLE,	"Invalid External Handle" },
};

class CDesktopLogManager {
	bool Verbose;			// true = write messages into a file
public:
	FILE * FpDebug;			// where to send debugging messages

	CDesktopLogManager();

	void createLogFile(const std::string&  debugFilename);

	void setVerbose(bool b) { Verbose = b; }
	bool getVerbose() {return Verbose; }

	void printLog(std::string s);
	void printLog(std::string s, int n);
	void printLog(std::string s, int n0, int n1);
	void printLog(std::string s, int n0, int n1, int n2);
	void printLog(std::string s, float n);
	void printLog(std::string s, float n0, float n1);
	void printLog(std::string s, glm::vec3 vec);
	void printLog(std::string s0, std::string s1);
	void printLog(std::string s, float *n, int size);
	void printLog(std::string s, int *n, int size);
	void printLog(std::string s, unsigned long int n);
	void printLog(std::string s, unsigned int n);

	void printVkError(VkResult result, std::string prefix);


	void flush();
};




#endif