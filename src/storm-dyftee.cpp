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
    storm::utility::initialize::setUp();
    
    storm::parser::DFTGalileoParser parser;
    storm::storage::DFT dft = parser.parseDFT(argv[1]);
    dft.printElements();
    dft.printSpareModules();
    storm::builder::ExplicitDFTModelBuilder builder(dft);
    builder.buildCtmc();
}

