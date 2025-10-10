#ifndef H_MAIN
#define H_MAIN

#include "application.h"

int main(){
	//TEST_CLASS_NAME app;
	CApplication app;
	//app.m_sampleName = "CSimpleTriangle"; //TOSTRING(TEST_CLASS_NAME);
	app.run();
	return 0;

	// try {
	// 	app.run();
	// }
	// catch (const std::exception& e) {
	// 	std::cerr << e.what() << std::endl;
	// 	return EXIT_FAILURE;
	// }

	// return EXIT_SUCCESS;
}

#endif