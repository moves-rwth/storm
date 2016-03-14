#include "logic/Formula.h"
#include "utility/initialize.h"
#include "utility/storm.h"
#include "modelchecker/DFTAnalyser.h"
#include <boost/lexical_cast.hpp>

/*!
 * Load DFT from filename, build corresponding Model and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 */
template <typename ValueType>
void analyzeDFT(std::string filename, std::string property, bool symred = false, bool allowModularisation = false, bool enableDC = true) {
    storm::settings::SettingsManager& manager = storm::settings::mutableManager();
    manager.setFromString("");

    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    std::vector<std::shared_ptr<storm::logic::Formula>> parsedFormulas = storm::parseFormulasForExplicit(property);
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas(parsedFormulas.begin(), parsedFormulas.end());
    assert(formulas.size() == 1);
    
    DFTAnalyser<ValueType> analyser;
    analyser.check(dft, formulas[0], symred, allowModularisation, enableDC);
    analyser.printTimings();
    analyser.printResult();
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
    bool minimal = true;
    bool allowModular = true;
    bool enableModularisation = false;
    bool disableDC = false;
    std::string filename = argv[1];
    std::string operatorType = "";
    std::string targetFormula = "";
    std::string pctlFormula = "";
    for (int i = 2; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--parametric") {
            parametric = true;
        } else if (option == "--expectedtime") {
            assert(targetFormula.empty());
            operatorType = "ET";
            targetFormula = "F \"failed\"";
            allowModular = false;
        } else if (option == "--probability") {
            assert(targetFormula.empty());
            operatorType = "P";;
            targetFormula = "F \"failed\"";
        } else if (option == "--timebound") {
            assert(targetFormula.empty());
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
            stream << "F<=" << timeBound << " \"failed\"";
            operatorType = "P";
            targetFormula = stream.str();
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
        } else if (option == "--modularisation") {
            enableModularisation = true;
        } else if (option == "--disabledc") {
            disableDC = true;
        } else if (option == "--min") {
            minimal = true;
        } else if (option == "--max") {
            minimal = false;
        } else {
            std::cout << "Option '" << option << "' not recognized." << std::endl;
            return 1;
        }
    }
    
    // Construct pctlFormula
    if (!targetFormula.empty()) {
        assert(pctlFormula.empty());
        pctlFormula = operatorType + (minimal ? "min" : "max") + "=?[" + targetFormula + "]";
    }
    
    assert(!pctlFormula.empty());

    storm::utility::setUp();
    std::cout << "Running " << (parametric ? "parametric " : "") << "DFT analysis on file " << filename << " with property " << pctlFormula << std::endl;

    if (parametric) {
        analyzeDFT<storm::RationalFunction>(filename, pctlFormula, symred, allowModular && enableModularisation, !disableDC );
    } else {
        analyzeDFT<double>(filename, pctlFormula, symred, allowModular && enableModularisation, !disableDC);
    }
}
