#include <iostream>

#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/settings/SettingsManager.h"

int main(int argc, char* argv[]) {
	storm::settings::initializeAll("StoRM (Performance) Testing Suite", "storm-performance-tests");
	std::cout << "StoRM (Performance) Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}
