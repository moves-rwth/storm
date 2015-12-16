#include "parser/DFTGalileoParser.h"
#include "utility/initialize.h"
#include "builder/ExplicitDFTModelBuilder.h"


/*
 * Entry point for the DyFTeE backend.
 */
int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout <<  "Storm-DyFTeE should be called with a filename as argument." << std::endl;
    }

    storm::utility::setUp();
    log4cplus::LogLevel level = log4cplus::TRACE_LOG_LEVEL;
    logger.setLogLevel(level);
    logger.getAppender("mainConsoleAppender")->setThreshold(level);

    std::cout << "Parsing DFT file..." << std::endl;
    storm::parser::DFTGalileoParser parser;
    storm::storage::DFT dft = parser.parseDFT(argv[1]);

    std::cout << "Built data structure" << std::endl;

    std::cout << "Building CTMC..." << std::endl;
    storm::builder::ExplicitDFTModelBuilder<double> builder(dft);
    builder.buildCTMC();
    std::cout << "Built CTMC" << std::endl;

    std::cout << "Model checking..." << std::endl;
    //TODO Matthias: check CTMC
    std::cout << "Checked model" << std::endl;
}
