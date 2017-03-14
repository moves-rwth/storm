#include "storm-pgcl/parser/PgclParser.h"

#include "logic/Formula.h"
#include "utility/initialize.h"
#include "utility/storm.h"
#include "storm/cli/cli.h"
#include "storm/exceptions/BaseException.h"
#include "storm/utility/macros.h"
#include <boost/lexical_cast.hpp>
#include "storm-pgcl/builder/ProgramGraphBuilder.h"
#include "storm-pgcl/builder/JaniProgramGraphBuilder.h"
#include "storm/storage/jani/JSONExporter.h"

#include "storm/exceptions/FileIoException.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/PGCLSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"


/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("Storm-PGCL", "storm-pgcl");
    
    // Register all known settings modules.
    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
    storm::settings::addModule<storm::settings::modules::PGCLSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    storm::settings::addModule<storm::settings::modules::JaniExportSettings>();
}

void handleJani(storm::jani::Model& model) {
    if (!storm::settings::getModule<storm::settings::modules::JaniExportSettings>().isJaniFileSet()) {
        // For now, we have to have a jani file
        storm::jani::JsonExporter::toStream(model, {}, std::cout);
    } else {
        storm::jani::JsonExporter::toFile(model, {}, storm::settings::getModule<storm::settings::modules::JaniExportSettings>().getJaniFilename());
    }
}

void programGraphToDotFile(storm::ppg::ProgramGraph const& prog) {
    std::string filepath = storm::settings::getModule<storm::settings::modules::PGCLSettings>().getProgramGraphDotOutputFilename();
    std::ofstream stream;
    storm::utility::openFile(filepath, stream);
    prog.printDot(stream);
    storm::utility::closeFile(stream);
}

int main(const int argc, const char** argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-PGCL", argc, argv);
        initializeSettings();
        
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }
        
        if (!storm::settings::getModule<storm::settings::modules::PGCLSettings>().isPgclFileSet()) {
            return -1;
        }
        storm::pgcl::PgclProgram prog = storm::parser::PgclParser::parse(storm::settings::getModule<storm::settings::modules::PGCLSettings>().getPgclFilename());
        storm::ppg::ProgramGraph* progGraph = storm::builder::ProgramGraphBuilder::build(prog);
    
        progGraph->printInfo(std::cout);
        if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isProgramGraphToDotSet()) {
            programGraphToDotFile(*progGraph);
        }
        if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isToJaniSet()) {
            storm::builder::JaniProgramGraphBuilderSetting settings;
            // To disable reward detection, uncomment the following line
            // TODO add a setting for this.
            // settings.filterRewardVariables = false;
            storm::builder::JaniProgramGraphBuilder builder(*progGraph, settings);
            if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isProgramVariableRestrictionSet()) {
                // TODO More fine grained control
                storm::storage::IntegerInterval restr = storm::storage::parseIntegerInterval(storm::settings::getModule<storm::settings::modules::PGCLSettings>().getProgramVariableRestrictions());
                builder.restrictAllVariables(restr);
            }
            storm::jani::Model* model = builder.build();
            delete progGraph;
            handleJani(*model);
            delete model;
        } else {
            
        }
    }catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-PGCL to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-PGCL to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
