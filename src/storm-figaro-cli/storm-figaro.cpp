
#include "logic/Formula.h"
#include "storm-figaro/api/storm-figaro.cpp"
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


template<typename ValueType>
std::vector<std::string> parseproperties(std::string filepath){
    auto ioSettings =  storm::settings::getModule<storm::settings::modules::IOSettings>();
    std::vector<std::string> properties;
    std::ofstream contents;
    storm::utility::openFile(filepath, contents);
        //    ParseXML file and interpret properties
    storm::utility::closeFile(contents);
    //I set only two prperties here
    if (ioSettings.isPropertySet()) {
        properties.push_back(ioSettings.getProperty());
    }
        //TODO use the following xml support
        //#ifdef STORM_HAVE_XERCES
        //#include <xercesc/parsers/XercesDOMParser.hpp>
        //#include <xercesc/util/XMLString.hpp>
       
    int missiontime = 100;
    properties.push_back("Pmax=?  [F<="+ std::to_string(missiontime) + " \"failed\"]");
    
    std::stringstream stream;
    stream << "Pmax=? [F [" << missiontime<<"," << missiontime<< "] \"failed\"]";
    properties.push_back(stream.str());
    return properties;
}
template<typename ValueType>
void processOptions() {
    
 
    auto figarooptions = storm::settings::getModule<storm::settings::modules::FIGAROIOSettings>();
    
    //build model from Figaro file
    //@TODO We need to parse figarofile with the definitive figaroAPI
    // Construct properties to analyse.
    // We allow multiple properties to be checked at once.
    
    std::shared_ptr<storm::models::sparse::Model<ValueType>> model = storm::figaro::checkfigaro<ValueType>();
    
    //convert this model to CTMC
   
    if (model->isOfType(storm::models::ModelType::MarkovAutomaton))
        {
//        if (model->isConvertibleToCtmc()) {
//            STORM_LOG_WARN_COND(false, "MA is convertible to a CTMC, consider using a CTMC instead.");
//            model = model->convertToCtmc();
//        }
//        else{
        model = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), storm::transformer::EliminationLabelBehavior::MergeLabels);
        }
//    }
    
    model->printModelInformationToStream(std::cout);
    std::ofstream stream;
    if (figarooptions.isrslttxtFileSet())
        {
        storm::utility::openFile(figarooptions.getrlsttxtFilename(), stream);
//        storm::utility::openFile("hello.txt", stream);
        model->printModelInformationToStream(stream);

        }
   //get properties form the xml file and command line
    std::vector<std::string> properties;
    if (figarooptions.isxmlFileSet()) {
        STORM_LOG_DEBUG("Loading Properties from .xml file " << figarooptions.getxmlFilename());

        properties = parseproperties<ValueType>(figarooptions.getxmlFilename());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No input model given.");
    }
//    Now build these properties
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
    //Now we have our figaro model as sparse model and properties as vector of properties
    // Check the model
    STORM_LOG_DEBUG("Model checking...");
//    modelCheckingTimer.start();
    std::vector<ValueType> results;
    storm::utility::Stopwatch singleModelCheckingTimer;
    
    for (auto property : props) {
        singleModelCheckingTimer.reset();
        singleModelCheckingTimer.start();
        STORM_PRINT_AND_LOG("Model checking property " << *property << " ..." << std::endl);
        stream<<"Model checking property " << *property << " ..." << std::endl;
        std::unique_ptr<storm::modelchecker::CheckResult> result(
                                                                 storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(property,true)));
//
        if (result) {
            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
            ValueType resultValue = result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
            results.push_back(resultValue);
            STORM_PRINT_AND_LOG("Result (initial states): " << resultValue << std::endl);
            stream<<"Result (initial states): " << resultValue << std::endl;
        } else {
            STORM_LOG_WARN("The property '" << *property << "' could not be checked with the current settings.");
            results.push_back(-storm::utility::one<ValueType>());
            stream<<"The property '" << *property << "' could not be checked with the current settings.";
        }

        singleModelCheckingTimer.stop();
            STORM_PRINT_AND_LOG("Time for model checking: " << singleModelCheckingTimer << "." << std::endl);
        stream<<"Time for model checking: " << singleModelCheckingTimer << "." << std::endl
        ;
    }
    if (figarooptions.isrslttxtFileSet())
        {
                storm::utility::closeFile(stream);
        }
    
    
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

int main(const int argc, const char** argv) {
    
    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-FIGARO", argc, argv);
         storm::settings::initializeFigaroSettings("Storm-FIARO", "storm-figaro");
        storm::utility::Stopwatch totalTimer(true);
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        auto figarooptions = storm::settings::getModule<storm::settings::modules::FIGAROIOSettings>();
        if (!figarooptions.isfigaroFileSet()) {
            std::cout<<"Please input dummy figaro file. We will later replace this file with valid figaro models for definitive API";
            return -1;
        }
        if (!figarooptions.isxmlFileSet()) {
            std::cout<<"Please input dummy xml file. We will later replace this file with valid xml information for definitive API";
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
        
    }catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-FIGARO to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-FIGARO to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}

