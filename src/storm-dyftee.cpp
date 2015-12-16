#include "logic/Formula.h"
#include "parser/DFTGalileoParser.h"
#include "utility/initialize.h"
#include "builder/ExplicitDFTModelBuilder.h"
#include "modelchecker/results/CheckResult.h"
#include "utility/storm.h"

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
    std::shared_ptr<storm::models::sparse::Model<double>> model = builder.buildCTMC();
    std::cout << "Built CTMC" << std::endl;

    std::cout << "Model checking..." << std::endl;
    //TODO Matthias: Construct formula, do not fix
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = storm::parseFormulasForExplicit("Pmax=?[true U \"failed\"]");
    assert(formulas.size() == 1);
    // Verify the model, if a formula was given.
    std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formulas[0]));
    assert(result);
    std::cout << "Result (initial states): ";
    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
    std::cout << *result << std::endl;

    std::cout << "Checked model" << std::endl;
}
