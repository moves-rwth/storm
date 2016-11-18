#include <iostream>
#include <list>
#include <string>

#include "storm-config.h"

#include "gtest/gtest.h"

#include "storm/settings/SettingsManager.h"

int main(int argc, char* argv[]) {
	storm::settings::initializeAll("StoRM (Functional) Testing Suite", "storm-functional-tests");
	std::cout << "StoRM (Functional) Testing Suite" << std::endl;
	
	storm::utility::setUp();
	storm::utility::setLogLevel(l3pp::LogLevel::WARN);
	
	char** filteredArguments = new char*[argc]();
	int position = 0;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--trace") != 0) {
			filteredArguments[position] = argv[i];
			++position;
		} else {
			// Handle --trace
			storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
		}
	}
	
	testing::InitGoogleTest(&position, filteredArguments);

    int result = RUN_ALL_TESTS();
    
	std::list<std::string> untestedModules;
#ifndef STORM_HAVE_GUROBI
	untestedModules.push_back("Gurobi");
#endif
#ifndef STORM_HAVE_CUDA
	untestedModules.push_back("CUDA");
#endif
#ifndef STORM_HAVE_GLPK
	untestedModules.push_back("GLPK");
#endif
#ifndef STORM_HAVE_Z3
	untestedModules.push_back("Z3");
#endif
#ifndef STORM_HAVE_MSAT
	untestedModules.push_back("MathSAT");
#endif
#ifndef STORM_HAVE_INTELTBB
	untestedModules.push_back("Intel TBB");
#endif
#ifndef STORM_HAVE_CARL
	untestedModules.push_back("Carl");
#endif
	
	if (result == 0) {
		if (untestedModules.empty()) {
			std::cout << std::endl << "ALL TESTS PASSED!" << std::endl;
		} else{
			std::cout << std::endl << "StoRM was built without the following optional dependencies: ";
			auto iter = untestedModules.begin();
			while (iter != untestedModules.end()) {
				std::cout << *iter;
				++iter;
				if (iter != untestedModules.end()) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl << "Functionality using that modules could not be tested." << std::endl << std::endl << "TESTS PASSED!" << std::endl;
		}
	} else{
		std::cout << std::endl << "TESTS FAILED!" << std::endl;
	}

    return result;
}
