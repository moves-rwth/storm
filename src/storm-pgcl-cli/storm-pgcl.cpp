#include "storm-pgcl/parser/PgclParser.h"

#include <boost/lexical_cast.hpp>
#include "logic/Formula.h"
#include "storm-cli-utilities/cli.h"
#include "storm-pgcl/builder/JaniProgramGraphBuilder.h"
#include "storm-pgcl/builder/ProgramGraphBuilder.h"
#include "storm/exceptions/BaseException.h"
#include "storm/storage/jani/visitor/JSONExporter.h"
#include "storm/utility/macros.h"
#include "utility/initialize.h"

#include "storm/exceptions/FileIoException.h"

#include "storm-conv/api/storm-conv.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm-pgcl/settings/modules/PGCLSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/io/file.h"

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

void handleJani(storm::jani::Model& model, std::vector<storm::jani::Property>& properties) {
    auto const& jani = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
    storm::converter::JaniConversionOptions options(jani);
    storm::api::transformJani(model, properties, options);
    if (storm::settings::getModule<storm::settings::modules::PGCLSettings>().isToJaniSet()) {
        storm::api::exportJaniToFile(model, properties, storm::settings::getModule<storm::settings::modules::PGCLSettings>().getWriteToJaniFilename(),
                                     jani.isCompactJsonSet());
    } else {
        storm::api::printJaniToStream(model, properties, std::cout);
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

        // Start by setting some urgent options (log levels, resources, etc.)
        storm::cli::setUrgentOptions();

        auto pgcl = storm::settings::getModule<storm::settings::modules::PGCLSettings>();
        if (!pgcl.isPgclFileSet()) {
            return -1;
        }

        storm::pgcl::PgclProgram prog = storm::parser::PgclParser::parse(pgcl.getPgclFilename());
        storm::ppg::ProgramGraph* progGraph = storm::builder::ProgramGraphBuilder::build(prog);

        progGraph->printInfo(std::cout);
        if (pgcl.isProgramGraphToDotSet()) {
            programGraphToDotFile(*progGraph);
        }
        if (pgcl.isToJaniSet()) {
            storm::builder::JaniProgramGraphBuilderSetting settings;
            // To disable reward detection, uncomment the following line
            // TODO add a setting for this.
            // settings.filterRewardVariables = false;
            storm::builder::JaniProgramGraphBuilder builder(*progGraph, settings);
            if (pgcl.isProgramVariableRestrictionSet()) {
                // TODO More fine grained control
                storm::storage::IntegerInterval restr = storm::storage::parseIntegerInterval(pgcl.getProgramVariableRestrictions());
                builder.restrictAllVariables(restr);
            }
            storm::jani::Model* model = builder.build();

            delete progGraph;
            std::vector<storm::jani::Property> properties;
            if (pgcl.isPropertyInputSet()) {
                boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(pgcl.getPropertyInputFilter());
                properties = storm::api::parsePropertiesForSymbolicModelDescription(pgcl.getPropertyInput(), *model, propertyFilter);
            }

            handleJani(*model, properties);
            delete model;
        } else {
        }
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-PGCL to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-PGCL to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
