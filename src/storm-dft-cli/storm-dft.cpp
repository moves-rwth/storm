#include "storm-dft/settings/DftSettings.h"

#include "storm-dft/settings/modules/DftIOSettings.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/ResourceSettings.h"

#include "storm/utility/initialize.h"
#include "storm/api/storm.h"
#include "storm-cli-utilities/cli.h"

#include "storm-dft/parser/DFTGalileoParser.h"
#include "storm-dft/parser/DFTJsonParser.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/DFTASFChecker.h"
#include "storm-dft/transformations/DftToGspnTransformator.h"
#include "storm-dft/storage/dft/DftJsonExporter.h"

#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm-gspn/storm-gspn.h"

#include <boost/lexical_cast.hpp>
#include <memory>

template<typename ValueType>
std::shared_ptr<storm::storage::DFT<ValueType>> loadDFT() {
    storm::settings::modules::DftIOSettings const& dftIOSettings = storm::settings::getModule<storm::settings::modules::DftIOSettings>();

    // Build DFT from given file.
    if (dftIOSettings.isDftJsonFileSet()) {
        storm::parser::DFTJsonParser<ValueType> parser;
        STORM_LOG_DEBUG("Loading DFT from file " << dftIOSettings.getDftJsonFilename());
        return std::make_shared<storm::storage::DFT<ValueType>>(parser.parseJson(dftIOSettings.getDftJsonFilename()));
    } else {
        storm::parser::DFTGalileoParser<ValueType> parser;
        STORM_LOG_DEBUG("Loading DFT from file " << dftIOSettings.getDftFilename());
        return std::make_shared<storm::storage::DFT<ValueType>>(parser.parseDFT(dftIOSettings.getDftFilename()));
    }
}

/*!
 * Analyse the given DFT according to the given properties.
 * We first load the DFT from the given file, then build the corresponding model and last check against the given properties.
 *
 * @param properties PCTL formulas capturing the properties to check.
 * @param symred Flag whether symmetry reduction should be used.
 * @param allowModularisation Flag whether modularisation should be applied if possible.
 * @param enableDC Flag whether Don't Care propagation should be used.
 * @param approximationError Allowed approximation error.
 */
template <typename ValueType>
void analyzeDFT(std::vector<std::string> const& properties, bool symred, bool allowModularisation, bool enableDC, double approximationError) {
    std::shared_ptr<storm::storage::DFT<ValueType>> dft = loadDFT<ValueType>();

    // Build properties
    std::string propString = properties[0];
    for (size_t i = 1; i < properties.size(); ++i) {
        propString += ";" + properties[i];
    }
    std::vector<std::shared_ptr<storm::logic::Formula const>> props = storm::api::extractFormulasFromProperties(storm::api::parseProperties(propString));
    STORM_LOG_ASSERT(props.size() > 0, "No properties found.");

    // Check model
    storm::modelchecker::DFTModelChecker<ValueType> modelChecker;
    modelChecker.check(*dft, props, symred, allowModularisation, enableDC, approximationError);
    modelChecker.printTimings();
    modelChecker.printResults();
}

/*!
 * Analyze the DFT with use of SMT solving.
 *
 * @param filename Path to DFT file in Galileo format.
 */
template<typename ValueType>
void analyzeWithSMT(std::shared_ptr<storm::storage::DFT<ValueType>> dft) {
    STORM_LOG_DEBUG("Running DFT analysis with use of SMT");
    storm::modelchecker::DFTASFChecker asfChecker(*dft);
    asfChecker.convert();
    asfChecker.toFile("test.smt2");
    //bool sat = dftSmtBuilder.check();
    //std::cout << "SMT result: " << sat << std::endl;
}

void processOptions() {
        // Start by setting some urgent options (log levels, resources, etc.)
        storm::cli::setUrgentOptions();

        storm::settings::modules::DftIOSettings const& dftIOSettings = storm::settings::getModule<storm::settings::modules::DftIOSettings>();
        storm::settings::modules::FaultTreeSettings const& faultTreeSettings = storm::settings::getModule<storm::settings::modules::FaultTreeSettings>();
        storm::settings::modules::GeneralSettings const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
        storm::settings::modules::IOSettings const& ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
        if (!dftIOSettings.isDftFileSet() && !dftIOSettings.isDftJsonFileSet()) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model.");
        }

        if (dftIOSettings.isExportToJson()) {
            STORM_LOG_THROW(dftIOSettings.isDftFileSet(), storm::exceptions::InvalidSettingsException, "No input model in Galileo format given.");
            std::shared_ptr<storm::storage::DFT<double>> dft = loadDFT<double>();
            // Export to json
            storm::storage::DftJsonExporter<double>::toFile(*dft, dftIOSettings.getExportJsonFilename());
            return;
        }

        
        if (dftIOSettings.isTransformToGspn()) {
            std::shared_ptr<storm::storage::DFT<double>> dft = loadDFT<double>();
            // Transform to GSPN
            storm::transformations::dft::DftToGspnTransformator<double> gspnTransformator(*dft);
            gspnTransformator.transform();
            storm::gspn::GSPN* gspn = gspnTransformator.obtainGSPN();
            uint64_t toplevelFailedPlace = gspnTransformator.toplevelFailedPlaceId();
            
            storm::handleGSPNExportSettings(*gspn);

            std::shared_ptr<storm::expressions::ExpressionManager> const& exprManager = gspn->getExpressionManager();
            storm::builder::JaniGSPNBuilder builder(*gspn);
            storm::jani::Model* model =  builder.build();
            storm::jani::Variable const& topfailedVar = builder.getPlaceVariable(toplevelFailedPlace);
            
            storm::expressions::Expression targetExpression = exprManager->integer(1) == topfailedVar.getExpressionVariable().getExpression();
            auto evtlFormula = std::make_shared<storm::logic::AtomicExpressionFormula>(targetExpression);
            auto tbFormula = std::make_shared<storm::logic::BoundedUntilFormula>(std::make_shared<storm::logic::BooleanLiteralFormula>(true), evtlFormula, storm::logic::TimeBound(false, exprManager->integer(0)), storm::logic::TimeBound(false, exprManager->integer(10)), storm::logic::TimeBoundReference(storm::logic::TimeBoundType::Time));
            auto tbUntil = std::make_shared<storm::logic::ProbabilityOperatorFormula>(tbFormula);
            
            auto evFormula = std::make_shared<storm::logic::EventuallyFormula>(evtlFormula, storm::logic::FormulaContext::Time);
            auto rewFormula = std::make_shared<storm::logic::TimeOperatorFormula>(evFormula, storm::logic::OperatorInformation(), storm::logic::RewardMeasureType::Expectation);
            
            storm::settings::modules::JaniExportSettings const& janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            if (janiSettings.isJaniFileSet()) {
                storm::api::exportJaniModel(*model, {storm::jani::Property("time-bounded", tbUntil), storm::jani::Property("mttf", rewFormula)}, janiSettings.getJaniFilename());
            }
            
            delete model;
            delete gspn;
            return;
        }
        
        bool parametric = false;
#ifdef STORM_HAVE_CARL
        parametric = generalSettings.isParametricSet();
#endif
        
#ifdef STORM_HAVE_Z3
        if (faultTreeSettings.solveWithSMT()) {
            // Solve with SMT
            if (parametric) {
            //    std::shared_ptr<storm::storage::DFT<double>> dft = loadDFT<double>();
            //    analyzeWithSMT<storm::RationalFunction>(dftSettings.getDftFilename());
            } else {
                std::shared_ptr<storm::storage::DFT<double>> dft = loadDFT<double>();
                analyzeWithSMT<double>(dft);
            }
            return;
        }
#endif
        
        // Set min or max
        std::string optimizationDirection = "min";
        if (dftIOSettings.isComputeMaximalValue()) {
            STORM_LOG_THROW(!dftIOSettings.isComputeMinimalValue(), storm::exceptions::InvalidSettingsException, "Cannot compute minimal and maximal values at the same time.");
            optimizationDirection = "max";
        }
        
        // Construct properties to check for
        std::vector<std::string> properties;
        
        if (ioSettings.isPropertySet()) {
            properties.push_back(ioSettings.getProperty());
        }
        if (dftIOSettings.usePropExpectedTime()) {
            properties.push_back("T" + optimizationDirection + "=? [F \"failed\"]");
        }
        if (dftIOSettings.usePropProbability()) {
            properties.push_back("P" + optimizationDirection + "=? [F \"failed\"]");
        }
        if (dftIOSettings.usePropTimebound()) {
            std::stringstream stream;
            stream << "P" << optimizationDirection << "=? [F<=" << dftIOSettings.getPropTimebound() << " \"failed\"]";
            properties.push_back(stream.str());
        }
        if (dftIOSettings.usePropTimepoints()) {
            for (double timepoint : dftIOSettings.getPropTimepoints()) {
                std::stringstream stream;
                stream << "P" << optimizationDirection << "=? [F<=" << timepoint << " \"failed\"]";
                properties.push_back(stream.str());
            }
        }

        if (properties.empty()) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No property given.");
        }

        // Set possible approximation error
        double approximationError = 0.0;
        if (faultTreeSettings.isApproximationErrorSet()) {
            approximationError = faultTreeSettings.getApproximationError();
        }

        // From this point on we are ready to carry out the actual computations.
        if (parametric) {
#ifdef STORM_HAVE_CARL
            analyzeDFT<storm::RationalFunction>(properties, faultTreeSettings.useSymmetryReduction(), faultTreeSettings.useModularisation(), !faultTreeSettings.isDisableDC(), approximationError);
#else
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameters are not supported in this build.");
#endif
        } else {
            analyzeDFT<double>(properties, faultTreeSettings.useSymmetryReduction(), faultTreeSettings.useModularisation(), !faultTreeSettings.isDisableDC(), approximationError);
        }
}

/*!
 * Entry point for the DyFTeE backend.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successfull, not 0 otherwise.
 */
/*!
 * Main entry point of the executable storm-pars.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-dft", argc, argv);
        storm::settings::initializeDftSettings("Storm-dft", "storm-dft");

        storm::utility::Stopwatch totalTimer(true);
        if (!storm::cli::parseOptions(argc, argv)) {
            return -1;
        }

        processOptions();

        totalTimer.stop();
        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-DyFTeE to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-DyFTeE to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
