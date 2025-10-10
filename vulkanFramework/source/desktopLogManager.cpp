#include "../include/desktopLogManager.h"

CDesktopLogManager::CDesktopLogManager() {
	Verbose = true;
}

void CDesktopLogManager::createLogFile(const std::string&  debugFilename){
#ifdef _WIN32
	errno_t err = fopen_s(&FpDebug, debugFilename.c_str(), "w");
	if (err != 0)
	{
		fprintf(stderr, "Cannot open debug print file '%s'\n", debugFilename.c_str());
		FpDebug = stderr;
	}
#else
	FpDebug = fopen(DEBUGFILE, "w");
	if (FpDebug == NULL)
	{
		fprintf(stderr, "Cannot open debug print file '%s'\n", DEBUGFILE);
		FpDebug = stderr;
	}
#endif
	//fprintf(FpDebug, "Width = %d ; Height = %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
}

void CDesktopLogManager::printLog(std::string s) {
	if(Verbose) fprintf(FpDebug, "%s", s.c_str());
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, int n) {
	if(Verbose) fprintf(FpDebug, s.c_str(), n);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, int n0, int n1) {
	if(Verbose) fprintf(FpDebug, s.c_str(), n0, n1);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, int n0, int n1, int n2) {
	if(Verbose) fprintf(FpDebug, s.c_str(), n0, n1, n2);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, float n) {
	if(Verbose) fprintf(FpDebug, s.c_str(), n);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, float n0, float n1){
	if(Verbose) fprintf(FpDebug, s.c_str(), n0, n1);
	if(Verbose) fprintf(FpDebug, "\n");
}

void  CDesktopLogManager::printLog(std::string s, glm::vec3 vec){
	if(Verbose) fprintf(FpDebug, s.c_str(), vec.x, vec.y, vec.z);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s0, std::string s1) {
	if(Verbose) fprintf(FpDebug, s0.c_str(), s1.c_str());
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, float *n, int size){
	if(Verbose) fprintf(FpDebug, "%s", s.c_str());
	for(int i = 0; i < size; i++) fprintf(FpDebug, "%f, ", n[i]);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, int *n, int size){
	if(Verbose) fprintf(FpDebug, "%s", s.c_str());
	for(int i = 0; i < size; i++) fprintf(FpDebug, "%d, ", n[i]);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, unsigned long int n){
	if(Verbose) fprintf(FpDebug, s.c_str(), n);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printLog(std::string s, unsigned int n){
	if(Verbose) fprintf(FpDebug, s.c_str(), n);
	if(Verbose) fprintf(FpDebug, "\n");
}

void CDesktopLogManager::printVkError(VkResult result, std::string prefix){
	if (Verbose  &&  result == VK_SUCCESS)
	{
		fprintf(FpDebug, "%s: %s\n", prefix.c_str(), "Successful");
		fflush(FpDebug);
		return;
	}

	const int numErrorCodes = sizeof(ErrorCodes) / sizeof(struct errorcode);
	std::string meaning = "";
	for (int i = 0; i < numErrorCodes; i++)
	{
		if (result == ErrorCodes[i].resultCode)
		{
			meaning = ErrorCodes[i].meaning;
			break;
		}
	}

	fprintf(FpDebug, "\n%s: %s\n", prefix.c_str(), meaning.c_str());
	fflush(FpDebug);
}


void CDesktopLogManager::flush() {
	fflush(FpDebug);
}
