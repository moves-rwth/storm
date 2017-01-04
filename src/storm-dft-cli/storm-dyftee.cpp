#include "storm/logic/Formula.h"
#include "storm/utility/initialize.h"
#include "storm/utility/storm.h"
#include "storm/cli/cli.h"
#include "storm/exceptions/BaseException.h"

#include "storm/logic/Formula.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/ResourceSettings.h"

#include "storm-dft/parser/DFTGalileoParser.h"
#include "storm-dft/parser/DFTJsonParser.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/DFTASFChecker.h"
#include "storm-dft/transformations/DftToGspnTransformator.h"


#include "storm-dft/settings/modules/DFTSettings.h"

#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm-gspn/storm-gspn.h"
#include "storm/settings/modules/GSPNSettings.h"
#include "storm/settings/modules/GSPNExportSettings.h"


#include <boost/lexical_cast.hpp>
#include <memory>

/*!
 * Load DFT from filename, build corresponding Model and check against given property.
 *
 * @param filename Path to DFT file in Galileo format.
 * @param property PCTC formula capturing the property to check.
 * @param symred Flag whether symmetry reduction should be used.
 * @param allowModularisation Flag whether modularisation should be applied if possible.
 * @param enableDC Flag whether Don't Care propagation should be used.
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
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
    
    // For translation into JANI via GSPN.
    storm::settings::addModule<storm::settings::modules::GSPNSettings>();
    storm::settings::addModule<storm::settings::modules::GSPNExportSettings>();
    storm::settings::addModule<storm::settings::modules::JaniExportSettings>();
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
        if (!dftSettings.isDftFileSet() && !dftSettings.isDftJsonFileSet()) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
        }

        
        if (dftSettings.isTransformToGspn()) {
            std::shared_ptr<storm::storage::DFT<double>> dft;
            if (dftSettings.isDftJsonFileSet()) {
                storm::parser::DFTJsonParser<double> parser;
                dft = std::make_shared<storm::storage::DFT<double>>(parser.parseJson(dftSettings.getDftJsonFilename()));
            } else {
                storm::parser::DFTGalileoParser<double> parser(true, false);
                dft = std::make_shared<storm::storage::DFT<double>>(parser.parseDFT(dftSettings.getDftFilename()));
            }
            storm::transformations::dft::DftToGspnTransformator<double> gspnTransformator(*dft);
            gspnTransformator.transform();
            storm::gspn::GSPN* gspn = gspnTransformator.obtainGSPN();
            uint64_t toplevelFailedPlace = gspnTransformator.toplevelFailedPlaceId();
            
            storm::handleGSPNExportSettings(*gspn);
            
            std::shared_ptr<storm::expressions::ExpressionManager> exprManager(new storm::expressions::ExpressionManager());
            storm::builder::JaniGSPNBuilder builder(*gspn, exprManager);
            storm::jani::Model* model =  builder.build();
            storm::jani::Variable const& topfailedVar = builder.getPlaceVariable(toplevelFailedPlace);
            

            storm::expressions::Expression targetExpression = exprManager->integer(1) == topfailedVar.getExpressionVariable().getExpression();
            auto evtlFormula = std::make_shared<storm::logic::AtomicExpressionFormula>(targetExpression);
            auto tbFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), evtlFormula, 0.0, 10.0);
            auto tbUntil = std::make_shared<storm::logic::ProbabilityOperatorFormula>(tbFormula);
            
            auto evFormula = std::make_shared<storm::logic::EventuallyFormula>(evtlFormula, storm::logic::FormulaContext::Time);
            auto rewFormula = std::make_shared<storm::logic::TimeOperatorFormula>(evFormula, storm::logic::OperatorInformation(), storm::logic::RewardMeasureType::Expectation);
            
            storm::settings::modules::JaniExportSettings const& janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            if (janiSettings.isJaniFileSet()) {
                storm::exportJaniModel(*model, {storm::jani::Property("time-bounded", tbUntil), storm::jani::Property("mttf", rewFormula)}, janiSettings.getJaniFilename());
            }
            
            delete model;
            delete gspn;
            storm::utility::cleanUp();
            return 0;
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
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM-DyFTeE to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
