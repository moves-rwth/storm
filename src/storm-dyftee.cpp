#include "logic/Formula.h"
#include "parser/DFTGalileoParser.h"
#include "utility/initialize.h"
#include "builder/ExplicitDFTModelBuilder.h"
#include "modelchecker/results/CheckResult.h"
#include "utility/storm.h"

/*!
 * Load DFT from filename, build corresponding Markov Automaton and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 */
template <typename ValueType>
void analyzeDFT(std::string filename, std::string property) {
    // Parsing DFT
    std::cout << "Parsing DFT file..." << std::endl;
    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    std::cout << "Built data structure" << std::endl;

    // Building Markov Automaton
    std::cout << "Building Markov Automaton..." << std::endl;
    storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft);
    std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.buildMA();
    std::cout << "Built Markov Automaton" << std::endl;

    // Model checking
    std::cout << "Model checking..." << std::endl;
    std::vector<std::shared_ptr<storm::logic::Formula>> formulas = storm::parseFormulasForExplicit(property);
    assert(formulas.size() == 1);
    std::unique_ptr<storm::modelchecker::CheckResult> resultMA(storm::verifySparseModel(model, formulas[0]));
    assert(resultMA);
    std::cout << "Result: ";
    resultMA->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
    std::cout << *resultMA << std::endl;
}

/*!
 * Entry point for the DyFTeE backend.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successfull, not 0 otherwise.
 */
int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Storm-DyFTeE should be called with a filename as argument." << std::endl;
        std::cout << "./storm-dft <filename> <--prop pctl-formula> <--parametric>" << std::endl;
        return 1;
    }

    // Parse cli arguments
    bool parametric = false;
    log4cplus::LogLevel level = log4cplus::WARN_LOG_LEVEL;
    std::string filename = argv[1];
    std::string pctlFormula = "";
    for (int i = 2; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--parametric") {
            parametric = true;
        } else if (option == "--expectedtime") {
            assert(pctlFormula.empty());
            pctlFormula = "ET=?[F \"failed\"]";
        } else if (option == "--probability") {
            assert(pctlFormula.empty());
            pctlFormula = "P=? [F \"failed\"]";
        } else if (option == "--trace") {
            level = log4cplus::TRACE_LOG_LEVEL;
        } else if (option == "--debug") {
            level = log4cplus::DEBUG_LOG_LEVEL;
        } else if (option == "--prop") {
            assert(pctlFormula.empty());
            ++i;
            assert(i < argc);
            pctlFormula = argv[i];
        } else {
            std::cout << "Option '" << option << "' not recognized." << std::endl;
            return 1;
        }
    }
    assert(!pctlFormula.empty());

    storm::utility::setUp();
    logger.setLogLevel(level);
    logger.getAppender("mainConsoleAppender")->setThreshold(level);

    std::cout << "Running " << (parametric ? "parametric " : "") << "DFT analysis on file " << filename << " with property " << pctlFormula << std::endl;

    if (parametric) {
        analyzeDFT<storm::RationalFunction>(filename, pctlFormula);
    } else {
        analyzeDFT<double>(filename, pctlFormula);
    }
}
