#include <boost/algorithm/string.hpp>

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/settings/DftSettings.h"
#include "storm-dft/settings/modules/DftGspnSettings.h"
#include "storm-dft/settings/modules/DftIOSettings.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/TransformationSettings.h"
#include "storm/utility/initialize.h"
#include "storm-cli-utilities/cli.h"
#include "storm-parsers/api/storm-parsers.h"


/*!
 * Process commandline options and start computations.
 */
template<typename ValueType>
void processOptions() {

    auto const& dftIOSettings = storm::settings::getModule<storm::settings::modules::DftIOSettings>();
    auto const& faultTreeSettings = storm::settings::getModule<storm::settings::modules::FaultTreeSettings>();
    auto const& ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
    auto const& dftGspnSettings = storm::settings::getModule<storm::settings::modules::DftGspnSettings>();
    auto const& transformationSettings = storm::settings::getModule<storm::settings::modules::TransformationSettings>();

    // Build DFT from given file
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;
    if (dftIOSettings.isDftFileSet()) {
        STORM_LOG_DEBUG("Loading DFT from Galileo file " << dftIOSettings.getDftFilename());
        dft = storm::api::loadDFTGalileoFile<ValueType>(dftIOSettings.getDftFilename());
    } else if (dftIOSettings.isDftJsonFileSet()) {
        STORM_LOG_DEBUG("Loading DFT from Json file " << dftIOSettings.getDftJsonFilename());
        dft = storm::api::loadDFTJsonFile<ValueType>(dftIOSettings.getDftJsonFilename());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model given.");
    }

    // Show statistics about DFT (number of gates, etc.)
    if (dftIOSettings.isShowDftStatisticsSet()) {
        dft->writeStatsToStream(std::cout);
        std::cout << std::endl;
    }

    // Export to json
    if (dftIOSettings.isExportToJson()) {
        storm::api::exportDFTToJsonFile<ValueType>(*dft, dftIOSettings.getExportJsonFilename());
    }

    // Check well-formedness of DFT
    auto wellFormedResult = storm::api::isWellFormed(*dft, false);
    STORM_LOG_THROW(wellFormedResult.first, storm::exceptions::UnmetRequirementException, "DFT is not well-formed: " << wellFormedResult.second);

    // Transformation to GSPN
    if (dftGspnSettings.isTransformToGspn()) {
        std::pair<std::shared_ptr<storm::gspn::GSPN>, uint64_t> pair = storm::api::transformToGSPN(*dft);
        std::shared_ptr<storm::gspn::GSPN> gspn = pair.first;
        uint64_t toplevelFailedPlace = pair.second;

        // Export
        storm::api::handleGSPNExportSettings(*gspn);

        // Transform to Jani
        // TODO analyse Jani model
        std::shared_ptr<storm::jani::Model> model = storm::api::transformToJani(*gspn, toplevelFailedPlace);
        return;
    }

    // Export to SMT
    if (dftIOSettings.isExportToSmt()) {
        storm::api::exportDFTToSMT<ValueType>(*dft, dftIOSettings.getExportSmtFilename());
        return;
    }

    bool useSMT = false;
    uint64_t solverTimeout = 10;
#ifdef STORM_HAVE_Z3
    if (faultTreeSettings.solveWithSMT()) {
        useSMT = true;
        STORM_LOG_DEBUG("Use SMT for preprocessing");
    }
#endif

    // Apply transformations
    // TODO transform later before actual analysis
    dft = storm::api::applyTransformations(*dft, faultTreeSettings.isUniqueFailedBE(), true);
    STORM_LOG_DEBUG(dft->getElementsString());


    dft->setDynamicBehaviorInfo();

    storm::api::PreprocessingResult preResults;
    // TODO: always needed?
    preResults.lowerBEBound = storm::dft::utility::FailureBoundFinder::getLeastFailureBound(*dft, useSMT, solverTimeout);
    preResults.upperBEBound = storm::dft::utility::FailureBoundFinder::getAlwaysFailedBound(*dft, useSMT, solverTimeout);
    STORM_LOG_DEBUG("BE failure bounds" << std::endl << "========================================" << std::endl <<
                                        "Lower bound: " << std::to_string(preResults.lowerBEBound) << std::endl <<
                                        "Upper bound: " << std::to_string(preResults.upperBEBound));

    // TODO: move into API call?
    preResults.fdepConflicts = storm::dft::utility::FDEPConflictFinder<ValueType>::getDependencyConflicts(*dft, useSMT, solverTimeout);

    if (preResults.fdepConflicts.empty()) {
        STORM_LOG_DEBUG("No FDEP conflicts found");
    } else {
        STORM_LOG_DEBUG("========================================" << std::endl << "FDEP CONFLICTS" << std::endl << "========================================");
    }
    for (auto pair: preResults.fdepConflicts) {
        STORM_LOG_DEBUG("Conflict between " << dft->getElement(pair.first)->name() << " and " << dft->getElement(pair.second)->name());
    }

    // Set the conflict map of the dft
    std::set<size_t> conflict_set;
    for (auto conflict : preResults.fdepConflicts) {
        conflict_set.insert(size_t(conflict.first));
        conflict_set.insert(size_t(conflict.second));
    }
    for (size_t depId : dft->getDependencies()) {
        if (!conflict_set.count(depId)) {
            dft->setDependencyNotInConflict(depId);
        }
    }


#ifdef STORM_HAVE_Z3
    if (useSMT) {
        // Solve with SMT
        STORM_LOG_DEBUG("Running DFT analysis with use of SMT");
        // Set dynamic behavior vector
        storm::api::analyzeDFTSMT(*dft, true);
    }
#endif


    // From now on we analyse the DFT via model checking

    // Set min or max
    std::string optimizationDirection = "min";
    if (dftIOSettings.isComputeMaximalValue()) {
        optimizationDirection = "max";
    }

    // Construct properties to analyse.
    // We allow multiple properties to be checked at once.
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

    // Build properties
    std::vector<std::shared_ptr<storm::logic::Formula const>> props;
    if (!properties.empty()) {
        std::string propString;
        for (size_t i = 0; i < properties.size(); ++i) {
            propString += properties[i];
            if (i + 1 < properties.size()) {
                propString += ";";
            }
        }
        props = storm::api::extractFormulasFromProperties(storm::api::parseProperties(propString));
    }


    // Set relevant event names
    std::vector<std::string> relevantEventNames;
    //Possible clash of relevantEvents and disableDC was already considered in FaultTreeSettings::check().
    if (faultTreeSettings.areRelevantEventsSet()) {
        relevantEventNames = faultTreeSettings.getRelevantEvents();
    } else if (faultTreeSettings.isDisableDC()) {
        // All events are relevant
        relevantEventNames = {"all"};
    }

    // Events from properties are relevant as well
    // Get necessary labels from properties
    std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabels;
    for (auto property : props) {
        property->gatherAtomicLabelFormulas(atomicLabels);
    }
    // Add relevant event names from properties
    for (auto atomic : atomicLabels) {
        std::string label = atomic->getLabel();
        if (label == "failed" or label == "skipped") {
            // Ignore as these label will always be added if necessary
        } else {
            // Get name of event
            if (boost::ends_with(label, "_failed")) {
                relevantEventNames.push_back(label.substr(0, label.size() - 7));
            } else if (boost::ends_with(label, "_dc")) {
                relevantEventNames.push_back(label.substr(0, label.size() - 3));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Label '" << label << "' not known.");
            }
        }
    }

    // Set relevant elements
    std::set<size_t> relevantEvents; // Per default no event (except the toplevel event) is relevant
    for (std::string const& relevantName : relevantEventNames) {
        if (relevantName == "none") {
            // Only toplevel event is relevant
            relevantEvents = {};
            break;
        } else if (relevantName == "all") {
            // All events are relevant
            relevantEvents = dft->getAllIds();
            break;
        } else {
            // Find and add corresponding event id
            relevantEvents.insert(dft->getIndex(relevantName));
        }
    }

    // Analyze DFT
    // TODO allow building of state space even without properties
    if (props.empty()) {
        STORM_LOG_WARN("No property given. No analysis will be performed.");
    } else {
        double approximationError = 0.0;
        if (faultTreeSettings.isApproximationErrorSet()) {
            approximationError = faultTreeSettings.getApproximationError();
        }
        storm::api::analyzeDFT<ValueType>(*dft, props, faultTreeSettings.useSymmetryReduction(), faultTreeSettings.useModularisation(), relevantEvents,
                                          faultTreeSettings.isAllowDCForRelevantEvents(), approximationError, faultTreeSettings.getApproximationHeuristic(),
                                          transformationSettings.isChainEliminationSet(), transformationSettings.getLabelBehavior(), true);
    }
}

/*!
 * Entry point for Storm-DFT.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successful, > 0 otherwise.
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
        
        // Start by setting some urgent options (log levels, resources, etc.)
        storm::cli::setUrgentOptions();

        storm::settings::modules::GeneralSettings const& generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
        if (generalSettings.isParametricSet()) {
            processOptions<storm::RationalFunction>();
        } else if (generalSettings.isExactSet()) {
            STORM_LOG_WARN("Exact solving over rational numbers is not implemented. Performing exact solving using rational functions instead.");
            processOptions<storm::RationalFunction>();
        } else {
            processOptions<double>();
        }

        totalTimer.stop();
        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-DFT to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-DFT to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
