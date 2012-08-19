#include <iostream>

#include "gtest/gtest.h"

#include <pantheios/pantheios.hpp>
#include <pantheios/backends/bec.file.h>
#include <pantheios/frontends/fe.simple.h>

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = "mrmc-cpp-tests";

int main(int argc, char** argv) {
	// Logging init
	pantheios_be_file_setFilePath("log.tests.all");
	pantheios::log_INFORMATIONAL("MRMC-Cpp Test Suite started.");
	pantheios_fe_simple_setSeverityCeiling(PANTHEIOS_SEV_DEBUG);
	std::cout << "MRMC Testing Suite" << std::endl;
	
	testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
