#include <iostream>
#include <src/builder/ExplicitGspnModelBuilder.h>
#include "src/exceptions/BaseException.h"
#include "src/parser/GspnParser.h"
#include "src/storage/gspn/GSPN.h"
#include "src/utility/macros.h"
#include "src/utility/initialize.h"
#include <fstream>

int main(const int argc, const char** argv) {
    if (argc != 3) {
        std::cout << "Error: incorrect number of parameters!" << std::endl << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "storm-gspn <path to pnml file> <formular>" << std::endl;
        return 1;
    }

    try {
        storm::utility::setUp();

        // Parse GSPN from xml
        auto parser = storm::parser::GspnParser();
        auto gspn = parser.parse(argv[1]);
        gspn.isValid();

        //
        //std::ofstream file;
        //file.open("/Users/thomas/Desktop/storm.dot");
        //gspn.writeDotToStream(file);
        //file.close();

        std::cout << "Parsing complete!" << std::endl;


        // Construct MA
        auto builder = storm::builder::ExplicitGspnModelBuilder<>();
        auto ma = builder.translateGspn(gspn, argv[2]);
        std::cout << "Markov Automaton: " << std::endl;
        std::cout << "number of states: " << ma.getNumberOfStates() << std::endl;
        std::cout << "number of transitions: " << ma.getNumberOfTransitions() << std::endl << std::endl;





        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
    }
}
