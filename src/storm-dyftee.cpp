#include "logic/Formula.h"
#include "utility/initialize.h"
#include "utility/storm.h"
#include "modelchecker/DFTAnalyser.h"
#include "src/cli/cli.h"
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include <boost/lexical_cast.hpp>

#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/DFTSettings.h"
#include "src/settings/modules/CoreSettings.h"
#include "src/settings/modules/DebugSettings.h"
//#include "src/settings/modules/CounterexampleGeneratorSettings.h"
//#include "src/settings/modules/CuddSettings.h"
//#include "src/settings/modules/SylvanSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
//#include "src/settings/modules/BisimulationSettings.h"
//#include "src/settings/modules/GlpkSettings.h"
//#include "src/settings/modules/GurobiSettings.h"
//#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
//#include "src/settings/modules/ParametricSettings.h"
#include "src/settings/modules/EliminationSettings.h"

/*!
 * Load DFT from filename, build corresponding Model and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 */
template <typename ValueType>
void analyzeDFT(std::string filename, std::string property, bool symred = false, bool allowModularisation = false, bool enableDC = true) {
    std::cout << "Running DFT analysis on file " << filename << " with property " << property << std::endl;

    storm::parser::DFTGalileoParser<ValueType> parser;
    storm::storage::DFT<ValueType> dft = parser.parseDFT(filename);
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::parseFormulasForExplicit(property);
    STORM_LOG_ASSERT(formulas.size() == 1, "Wrong number of formulas.");
    
    DFTAnalyser<ValueType> analyser;
    analyser.check(dft, formulas[0], symred, allowModularisation, enableDC);
    analyser.printTimings();
    analyser.printResult();
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
        
        // Set min or max
        bool minimal = true;
        if (dftSettings.isComputeMaximalValue()) {
            STORM_LOG_THROW(!dftSettings.isComputeMinimalValue(), storm::exceptions::InvalidSettingsException, "Cannot compute minimal and maximal values at the same time.");
            minimal = false;
        }
        
        // Construct pctlFormula
        std::string pctlFormula = "";
        bool allowModular = true;
        std::string operatorType = "";
        std::string targetFormula = "";
        
        if (generalSettings.isPropertySet()) {
            STORM_LOG_THROW(!dftSettings.usePropExpectedTime() && !dftSettings.usePropProbability() && !dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
            pctlFormula = generalSettings.getProperty();
        } else if (dftSettings.usePropExpectedTime()) {
            STORM_LOG_THROW(!dftSettings.usePropProbability() && !dftSettings.usePropTimebound(), storm::exceptions::InvalidSettingsException, "More than one property given.");
            operatorType = "T";
            targetFormula = "F \"failed\"";
            allowModular = false;
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

        bool parametric = false;
#ifdef STORM_HAVE_CARL
        parametric = generalSettings.isParametricSet();
#endif
        
        // From this point on we are ready to carry out the actual computations.
        if (parametric) {
#ifdef STORM_HAVE_CARL
            analyzeDFT<storm::RationalFunction>(dftSettings.getDftFilename(), pctlFormula, dftSettings.useSymmetryReduction(), allowModular && dftSettings.useModularisation(), !dftSettings.isDisableDC() );
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameters are not supported in this build.");
#endif
        } else {
            analyzeDFT<double>(dftSettings.getDftFilename(), pctlFormula, dftSettings.useSymmetryReduction(), allowModular && dftSettings.useModularisation(), !dftSettings.isDisableDC());
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
