#include "logic/Formula.h"
#include "storm-figaro/api/storm-figaro.h"

#include "utility/initialize.h"
#include "storm-cli-utilities/cli.h"
#include "storm/exceptions/BaseException.h"
#include "storm/utility/macros.h"
#include <boost/lexical_cast.hpp>
#include "storm/storage/jani/JSONExporter.h"

#include "storm/exceptions/FileIoException.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/ModelCheckerSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm-figaro/settings/FIGAROSettings.h"
#include "storm-figaro/settings/modules/FIGAROIOSettings.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/io/file.h"
#include "storm/settings/modules/BuildSettings.h"

#include "storm/settings/modules/EigenEquationSolverSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/modules/MultiplierSettings.h"
#include "storm/settings/modules/TopologicalEquationSolverSettings.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/HintSettings.h"
#include "storm/settings/modules/TransformationSettings.h"
#include "storm/settings/modules/TimeBoundedSolverSettings.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"


/*!
 * Process commandline options and start computations.
 */
template<typename ValueType>
void processOptions() {

    auto figarooptions = storm::settings::getModule<storm::settings::modules::FIGAROIOSettings>();
    auto ioSettings =  storm::settings::getModule<storm::settings::modules::IOSettings>();
    std::shared_ptr<storm::figaro::FigaroProgram> figaromodel = storm::figaro::api::loadFigaroProgram();

    //get properties form the xml file and command line
    std::vector<std::string> properties;
    if (ioSettings.isPropertySet()) {
        properties.push_back(ioSettings.getProperty());
    }
    if (figarooptions.isxmlFileSet()) {
        STORM_LOG_DEBUG("Loading Properties from .xml file " << figarooptions.getxmlFilename());

        properties = (storm::figaro::api::parseXmlProperties(figarooptions.getxmlFilename(), properties));
    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No settings file .xml given.");
    }

    //    Now build these properties
    std::vector<std::shared_ptr<storm::logic::Formula const>> props = storm::figaro::api::buildProperties(properties);

    //Now we have our figaro model as sparse model and properties as vector of properties
    //build model from Figaro file
    //@TODO: We need to parse figarofile with the definitive figaroAPI
    // Construct properties to analyse.
    // We allow multiple properties to be checked at once.

    storm::utility::Stopwatch singleModelCheckingTimer;
    double approximationError = 0.0;
//    std::shared_ptr<storm::models::sparse::Model<ValueType>> model; //merge this

    if (figarooptions.isApproximationErrorSet()) {
        approximationError = figarooptions.getApproximationError();
    }

    storm::figaro::api::analyzeFigaro<ValueType>(*figaromodel, props, approximationError,
                                                       figarooptions.getApproximationHeuristic(), true);

    //convert this model to CTMC

//    if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
//        if (model->isConvertibleToCtmc()) {
//            STORM_LOG_WARN_COND(false, "MA is convertible to a CTMC, consider using a CTMC instead.");
//            model = model->convertToCtmc();
//        }
//        else{
//        model = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(
//                model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(),
//                storm::transformer::EliminationLabelBehavior::MergeLabels);
//    }
////    }
//
//    model->printModelInformationToStream(std::cout);
//    std::ofstream stream;
//    if (figarooptions.isrslttxtFileSet()) {
//        storm::utility::openFile(figarooptions.getrlsttxtFilename(), stream);
////        storm::utility::openFile("hello.txt", stream);
//        model->printModelInformationToStream(stream);
//
//    }
//
//    // Check the model
//    STORM_LOG_DEBUG("Model checking...");
////    modelCheckingTimer.start();
//    std::vector<ValueType> results;
//
//
//    for (auto property : props) {
//        singleModelCheckingTimer.reset();
//        singleModelCheckingTimer.start();
//        STORM_PRINT_AND_LOG("Model checking property " << *property << " ..." << std::endl);
//        stream << "Model checking property " << *property << " ..." << std::endl;
//        std::unique_ptr<storm::modelchecker::CheckResult> result(
//                storm::api::verifyWithSparseEngine<ValueType>(model,
//                                                              storm::api::createTask<ValueType>(property,
//                                                                                                true)));
//
//        if (result) {
//            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
//            ValueType resultValue = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
//            results.push_back(resultValue);
//            STORM_PRINT_AND_LOG("Result (initial states): " << resultValue << std::endl);
//            stream << "Result (initial states): " << resultValue << std::endl;
//        } else {
//            STORM_LOG_WARN(
//                    "The property '" << *property << "' could not be checked with the current settings.");
//            results.push_back(-storm::utility::one<ValueType>());
//            stream << "The property '" << *property << "' could not be checked with the current settings.";
//        }
//
//        singleModelCheckingTimer.stop();
//        STORM_PRINT_AND_LOG("Time for model checking: " << singleModelCheckingTimer << "." << std::endl);
//        stream << "Time for model checking: " << singleModelCheckingTimer << "." << std::endl;
//    }
//    if (figarooptions.isrslttxtFileSet()) {
//        storm::utility::closeFile(stream);
//    }


//    model->printModelInformationToStream(std::cout);

    //        storm::pgcl::PgclProgram prog = storm::parser::PgclParser::parse(pgcl.getPgclFilename());
    //        storm::ppg::ProgramGraph* progGraph = storm::builder::ProgramGraphBuilder::build(prog);

    //        progGraph->printInfo(std::cout);
    //        if (pgcl.isProgramGraphToDotSet()) {
    //            programGraphToDotFile(*progGraph);
    //        }
    //        if (pgcl.isToJaniSet()) {
    //            storm::builder::JaniProgramGraphBuilderSetting settings;
    //            // To disable reward detection, uncomment the following line
    //            // TODO add a setting for this.
    //            // settings.filterRewardVariables = false;
    //            storm::builder::JaniProgramGraphBuilder builder(*progGraph, settings);
    //            if (pgcl.isProgramVariableRestrictionSet()) {
    //                // TODO More fine grained control
    //                storm::storage::IntegerInterval restr = storm::storage::parseIntegerInterval(pgcl.getProgramVariableRestrictions());
    //                builder.restrictAllVariables(restr);
    //            }
    //            storm::jani::Model* model = builder.build();
    //
    //            delete progGraph;
    //            std::vector<storm::jani::Property> properties;
    //            if (pgcl.isPropertyInputSet()) {
    //                boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(pgcl.getPropertyInputFilter());
    //                properties = storm::api::parsePropertiesForSymbolicModelDescription(pgcl.getPropertyInput(), *model, propertyFilter);
    //            }
    //
    //            handleJani(*model, properties);
    //            delete model;
    //        } else {
    //
    //        }
}


//void handleJani(storm::jani::Model& model, std::vector<storm::jani::Property>& properties) {
//    auto const& jani = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
//    storm::converter::JaniConversionOptions options(jani);
//    storm::api::transformJani(model, properties, options);
//    if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isToJaniSet()) {
//        storm::api::exportJaniToFile(model, properties, storm::settings::getModule<storm::settings::modules::PGCLSettings>().getWriteToJaniFilename(), jani.isCompactJsonSet());
//    } else {
//        storm::api::printJaniToStream(model, properties, std::cout);
//    }
//}
//
//void programGraphToDotFile(storm::ppg::ProgramGraph const& prog) {
//    std::string filepath = storm::settings::getModule<storm::settings::modules::PGCLSettings>().getProgramGraphDotOutputFilename();
//    std::ofstream stream;
//    storm::utility::openFile(filepath, stream);
//    prog.printDot(stream);
//    storm::utility::closeFile(stream);
//}

int main(const int argc, const char **argv) {

    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-FIGARO", argc, argv);
        storm::settings::initializeFigaroSettings("Storm-FIGARO", "storm-figaro");
        storm::utility::Stopwatch totalTimer(true);
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        auto figarooptions = storm::settings::getModule<storm::settings::modules::FIGAROIOSettings>();
        if (!figarooptions.isfigaroFileSet()) {
            std::cout
                    << "Please input dummy figaro file. We will later replace this file with valid figaro models for definitive API";
            return -1;
        }
        if (!figarooptions.isxmlFileSet()) {
            std::cout
                    << "Please input dummy xml file. We will later replace this file with valid xml information for definitive API";
            return -1;
        }

        // Start by setting some urgent options (log levels, resources, etc.)
        storm::cli::setUrgentOptions();


        processOptions<double>();
        totalTimer.stop();
        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;

    } catch (storm::exceptions::BaseException const &exception) {
        STORM_LOG_ERROR("An exception caused Storm-FIGARO to terminate. The message of the exception is: "
                                << exception.what());
        return 1;
    } catch (std::exception const &exception) {
        STORM_LOG_ERROR(
                "An unexpected exception occurred and caused Storm-FIGARO to terminate. The message of this exception is: "
                        << exception.what());
        return 2;
    }
}


