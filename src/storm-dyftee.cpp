#include "logic/Formula.h"
#include "parser/DFTGalileoParser.h"
#include "utility/initialize.h"
#include "builder/ExplicitDFTModelBuilder.h"
#include "modelchecker/results/CheckResult.h"
#include "utility/storm.h"
#include "storage/dft/DFTIsomorphism.h"

#include <boost/lexical_cast.hpp>

/*!
 * Load DFT from filename, build corresponding Model and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 */
template <typename ValueType>
void analyzeDFT(std::string filename, std::string property, bool symred = false) {
    storm::settings::SettingsManager& manager = storm::settings::mutableManager();
    manager.setFromString("");

    // Parsing DFT
    std::cout << "Parsing DFT file..." << std::endl;
    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    std::cout << "Built data structure" << std::endl;
    std::cout << "Parse formula..." << std::endl;
    std::vector<std::shared_ptr<storm::logic::Formula>> parsedFormulas = storm::parseFormulasForExplicit(property);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas(parsedFormulas.begin(), parsedFormulas.end());
    assert(formulas.size() == 1);
    std::cout << "Parsed formula." << std::endl;
    std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
    storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
    if(symred) {
        auto colouring = dft.colourDFT();
        symmetries = dft.findSymmetries(colouring);
        std::cout << "Symmetries: " << symmetries << std::endl;
    }
        
    // Building Markov Automaton
    std::cout << "Building Model..." << std::endl;
    storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries);
    typename storm::builder::ExplicitDFTModelBuilder<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
    std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.buildModel(labeloptions);
    std::cout << "Built Model" << std::endl;

    
    model->printModelInformationToStream(std::cout);
    std::cout << "No. states (Explored): " << model->getNumberOfStates() << std::endl;
    std::cout << "No. transitions (Explored): " << model->getNumberOfTransitions() << std::endl;
    std::cout << "Bisimulation..." << std::endl;
    
    if (model->getNumberOfStates() > 500 && model->isOfType(storm::models::ModelType::Ctmc)) {
            model =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), formulas, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
    }
    
    model->printModelInformationToStream(std::cout);
    std::cout << "No. states (Bisimulation): " << model->getNumberOfStates() << std::endl;
    std::cout << "No. transitions (Bisimulation): " << model->getNumberOfTransitions() << std::endl;
    
    // Model checking
    std::cout << "Model checking..." << std::endl;
    std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formulas[0]));
    assert(result);
    std::cout << "Result: ";
    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
    std::cout << *result << std::endl;
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
    bool symred = false;
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
        } else if (option == "--timebound") {
            assert(pctlFormula.empty());
            ++i;
            assert(i < argc);
            double timeBound;
            try {
                timeBound = boost::lexical_cast<double>(argv[i]);
            } catch (boost::bad_lexical_cast e) {
                std::cerr << "The time bound '" << argv[i] << "' is not valid." << std::endl;
                return 2;
            }
            std::stringstream stream;
            stream << "P=? [F<=" << timeBound << " \"failed\"]";
            pctlFormula = stream.str();
        } else if (option == "--trace") {
            STORM_GLOBAL_LOGLEVEL_TRACE();
        } else if (option == "--debug") {
            STORM_GLOBAL_LOGLEVEL_DEBUG();
        } else if (option == "--prop") {
            assert(pctlFormula.empty());
            ++i;
            assert(i < argc);
            pctlFormula = argv[i];
        } else if (option == "--symred") {
            symred = true;
        } else {
            std::cout << "Option '" << option << "' not recognized." << std::endl;
            return 1;
        }
    }
    assert(!pctlFormula.empty());

    storm::utility::setUp();
    std::cout << "Running " << (parametric ? "parametric " : "") << "DFT analysis on file " << filename << " with property " << pctlFormula << std::endl;

    if (parametric) {
        analyzeDFT<storm::RationalFunction>(filename, pctlFormula, symred);
    } else {
        analyzeDFT<double>(filename, pctlFormula, symred);
    }
}
