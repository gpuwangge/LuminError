#ifndef H_MAIN
#define H_MAIN

int main(){
	TEST_CLASS_NAME app;
	app.m_sampleName = TOSTRING(TEST_CLASS_NAME);

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif