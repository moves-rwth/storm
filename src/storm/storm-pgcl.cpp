#include "src/storm/parser/PgclParser.h"

#include "logic/Formula.h"
#include "utility/initialize.h"
#include "utility/storm.h"
#include "src/storm/cli/cli.h"
#include "src/storm/exceptions/BaseException.h"
#include "src/storm/utility/macros.h"
#include <boost/lexical_cast.hpp>
#include "src/storm/builder/ProgramGraphBuilder.h"
#include "src/storm/builder/JaniProgramGraphBuilder.h"
#include "src/storm/storage/jani/JSONExporter.h"

#include "src/storm/exceptions/FileIoException.h"

#include "src/storm/settings/modules/GeneralSettings.h"
#include "src/storm/settings/modules/PGCLSettings.h"
#include "src/storm/settings/modules/CoreSettings.h"
#include "src/storm/settings/modules/DebugSettings.h"
#include "src/storm/settings/modules/JaniExportSettings.h"


/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("StoRM-PGCL", "storm-pgcl");
    
    // Register all known settings modules.
    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
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
    std::ofstream ofs;
    ofs.open(filepath, std::ofstream::out );
    if (ofs.is_open()) {
        prog.printDot(ofs);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Cannot open " << filepath);
    }
}

int main(const int argc, const char** argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("StoRM-PGCL", argc, argv);
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
            storm::builder::JaniProgramGraphBuilder builder(*progGraph);
            if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isProgramVariableRestrictionSet()) {
                // TODO More fine grained control
                storm::storage::IntegerInterval restr = storm::storage::parseIntegerInterval(storm::settings::getModule<storm::settings::modules::PGCLSettings>().getProgramVariableRestrictions());
                builder.restrictAllVariables(restr);
            }
            builder.restrictAllVariables(0, 120);
            storm::jani::Model* model = builder.build();
            delete progGraph;
            handleJani(*model);
            delete model;
        } else {
            
        }
        
        
        
        
    }catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM-PGCL to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM-PGCL to terminate. The message of this exception is: " << exception.what());
    }
}
