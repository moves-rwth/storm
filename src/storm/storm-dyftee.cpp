#include "storm/logic/Formula.h"
#include "storm/utility/initialize.h"
#include "storm/utility/storm.h"
#include "storm/parser/DFTGalileoParser.h"
#include "storm/modelchecker/dft/DFTModelChecker.h"

#include "storm/modelchecker/dft/DFTASFChecker.h"
#include "storm/cli/cli.h"
#include "storm/exceptions/BaseException.h"
#include "storm/utility/macros.h"
#include "storm/builder/DftSmtBuilder.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/DFTSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
//#include "storm/settings/modules/CounterexampleGeneratorSettings.h"
//#include "storm/settings/modules/CuddSettings.h"
//#include "storm/settings/modules/SylvanSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
//#include "storm/settings/modules/BisimulationSettings.h"
//#include "storm/settings/modules/GlpkSettings.h"
//#include "storm/settings/modules/GurobiSettings.h"
//#include "storm/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
//#include "storm/settings/modules/ParametricSettings.h"
#include "storm/settings/modules/EliminationSettings.h"

#include <boost/lexical_cast.hpp>

/*!
 * Load DFT from filename, build corresponding Model and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 */
template <typename ValueType>
void analyzeDFT(std::string filename, std::string property, bool symred, bool allowModularisation, bool enableDC, double approximationError) {
    std::cout << "Running DFT analysis on file " << filename << " with property " << property << std::endl;

    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForExplicit(property);
    STORM_LOG_ASSERT(formulas.size() == 1, "Wrong number of formulas.");

    storm::modelchecker::DFTModelChecker<ValueType> modelChecker;
    modelChecker.check(dft, formulas[0], symred, allowModularisation, enableDC, approximationError);
    modelChecker.printTimings();
    modelChecker.printResult();
}

/*!
 * Analyze the DFT with use of SMT solving.
 *
 * @param filename Path to DFT file in Galileo format.
 */
template<typename ValueType>
void analyzeWithSMT(std::string filename) {
    std::cout << "Running DFT analysis on file " << filename << " with use of SMT" << std::endl;
    
    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    storm::modelchecker::DFTASFChecker asfChecker(dft);
    asfChecker.convert();
    asfChecker.toFile("test.smt2");
    //bool sat = dftSmtBuilder.check();
    //std::cout << "SMT result: " << sat << std::endl;
}

/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("StoRM-DyFTeE", "storm-dft");
    
    // Register all known settings modules.
    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::DFTSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    //storm::settings::addModule<storm::settings::modules::CounterexampleGeneratorSettings>();
    //storm::settings::addModule<storm::settings::modules::CuddSettings>();
    //storm::settings::addModule<storm::settings::modules::SylvanSettings>();
    storm::settings::addModule<storm::settings::modules::GmmxxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::MinMaxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
    //storm::settings::addModule<storm::settings::modules::BisimulationSettings>();
    //storm::settings::addModule<storm::settings::modules::GlpkSettings>();
    //storm::settings::addModule<storm::settings::modules::GurobiSettings>();
    //storm::settings::addModule<storm::settings::modules::TopologicalValueIterationEquationSolverSettings>();
    //storm::settings::addModule<storm::settings::modules::ParametricSettings>();
    storm::settings::addModule<storm::settings::modules::EliminationSettings>();
}

/*!
 * Entry point for the DyFTeE backend.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successfull, not 0 otherwise.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("StoRM-DyFTeE", argc, argv);
        initializeSettings();
        
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        
        storm::settings::modules::DFTSettings const& dftSettings = storm::settings::getModule<storm::settings::modules::DFTSettings>();
        storm::settings::modules::GeneralSettings const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
        if (!dftSettings.isDftFileSet()) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
        }
        
        bool parametric = false;
#ifdef STORM_HAVE_CARL
        parametric = generalSettings.isParametricSet();
#endif
        
#ifdef STORM_HAVE_Z3
        if (dftSettings.solveWithSMT()) {
            // Solve with SMT
            if (parametric) {
            //    analyzeWithSMT<storm::RationalFunction>(dftSettings.getDftFilename());
            } else {
                analyzeWithSMT<double>(dftSettings.getDftFilename());
            }
            storm::utility::cleanUp();
            return 0;
        }
#endif
        
        // Set min or max
        bool minimal = true;
        if (dftSettings.isComputeMaximalValue()) {
            STORM_LOG_THROW(!dftSettings.isComputeMinimalValue(), storm::exceptions::InvalidSettingsException, "Cannot compute minimal and maximal values at the same time.");
            minimal = false;
        }
        
        // Construct pctlFormula
        std::string pctlFormula = "";
        std::string operatorType = "";
        std::string targetFormula = "";
        
        if (generalSettings.isPropertySet()) {
            STORM_LOG_THROW(!dftSettings.usePropExpectedTime() && !dftSettings.usePropProbability() && !dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
            pctlFormula = generalSettings.getProperty();
        } else if (dftSettings.usePropExpectedTime()) {
            STORM_LOG_THROW(!dftSettings.usePropProbability() && !dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
            operatorType = "T";
            targetFormula = "F \"failed\"";
        } else if (dftSettings.usePropProbability()) {
            STORM_LOG_THROW(!dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
            operatorType = "P";;
            targetFormula = "F \"failed\"";
        } else {
            STORM_LOG_THROW(dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "No property given.");
            std::stringstream stream;
            stream << "F<=" << dftSettings.getPropTimebound() << " \"failed\"";
            operatorType = "P";
            targetFormula = stream.str();
        }
        
        if (!targetFormula.empty()) {
            STORM_LOG_ASSERT(pctlFormula.empty(), "Pctl formula not empty.");
            pctlFormula = operatorType + (minimal ? "min" : "max") + "=?[" + targetFormula + "]";
        }
        
        STORM_LOG_ASSERT(!pctlFormula.empty(), "Pctl formula empty.");

        double approximationError = 0.0;
        if (dftSettings.isApproximationErrorSet()) {
            approximationError = dftSettings.getApproximationError();
        }

        // From this point on we are ready to carry out the actual computations.
        if (parametric) {
#ifdef STORM_HAVE_CARL
            analyzeDFT<storm::RationalFunction>(dftSettings.getDftFilename(), pctlFormula, dftSettings.useSymmetryReduction(), dftSettings.useModularisation(), !dftSettings.isDisableDC(), approximationError);
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameters are not supported in this build.");
#endif
        } else {
            analyzeDFT<double>(dftSettings.getDftFilename(), pctlFormula, dftSettings.useSymmetryReduction(), dftSettings.useModularisation(), !dftSettings.isDisableDC(), approximationError);
        }
        
        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM-DyFTeE to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM-DyFTeE to terminate. The message of this exception is: " << exception.what());
    }
}
