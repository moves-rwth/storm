#include <iostream>
#include "src/exceptions/BaseException.h"
#include "src/parser/GspnParser.h"
#include "src/storage/gspn/GSPN.h"
#include "src/utility/macros.h"
#include "src/utility/initialize.h"

int main(const int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Error: incorrect number of parameters!" << std::endl << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "storm-gspn <path to pnml file>" << std::endl;
    }

    try {
        storm::utility::setUp();

        // Parse GSPN from xml
        storm::gspn::GSPN gspn = storm::parser::GspnParser::parse(argv[1]);

        // Construct MA

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}
