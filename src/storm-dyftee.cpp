#include "logic/Formula.h"
#include "parser/DFTGalileoParser.h"
#include "utility/initialize.h"
#include "builder/ExplicitDFTModelBuilder.h"
#include "modelchecker/results/CheckResult.h"
#include "utility/storm.h"

#define VALUE_TYPE double

/*
 * Entry point for the DyFTeE backend.
 */
int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout <<  "Storm-DyFTeE should be called with a filename as argument." << std::endl;
        return 1;
    }

    storm::utility::setUp();
    log4cplus::LogLevel level = log4cplus::TRACE_LOG_LEVEL;
    logger.setLogLevel(level);
    logger.getAppender("mainConsoleAppender")->setThreshold(level);

    std::cout << "Parsing DFT file..." << std::endl;
    storm::parser::DFTGalileoParser<VALUE_TYPE> parser;
    storm::storage::DFT<VALUE_TYPE> dft = parser.parseDFT(argv[1]);

    std::cout << "Built data structure" << std::endl;

    std::cout << "Building CTMC..." << std::endl;
    storm::builder::ExplicitDFTModelBuilder<VALUE_TYPE> builder(dft);
    std::shared_ptr<storm::models::sparse::Model<VALUE_TYPE>> model = builder.buildCTMC();
    std::cout << "Built CTMC" << std::endl;

    std::cout << "Model checking..." << std::endl;
    // Construct PCTL forumla
    std::string inputFormula;
    if (argc < 3) {
        // Set explicit reachability formula
        inputFormula = "Pmax=?[true U \"failed\"]";
    } else {
        // Read formula from input
        inputFormula = argv[2];
    }
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = storm::parseFormulasForExplicit(inputFormula);
    assert(formulas.size() == 1);

    // Verify the model
    std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formulas[0]));
    assert(result);
    std::cout << "Result (initial states): ";
    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
    std::cout << *result << std::endl;

    std::cout << "Checked model" << std::endl;
}
