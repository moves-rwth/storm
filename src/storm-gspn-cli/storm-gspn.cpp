#include "storm-gspn/builder/ExplicitGspnModelBuilder.h"
#include "storm-gspn/parser/GspnParser.h"
#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm-gspn/storage/gspn/GspnBuilder.h"
#include "storm-gspn/builder/JaniGSPNBuilder.h"
#include "storm-gspn/storm-gspn.h"

#include "storm/exceptions/BaseException.h"
#include "storm/exceptions/WrongFormatException.h"

#include "storm/utility/macros.h"
#include "storm/utility/initialize.h"

#include "utility/storm.h"
#include "storm/cli/cli.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/JSONExporter.h"
#include <fstream>
#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "storm/exceptions/FileIoException.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/GSPNSettings.h"
#include "storm/settings/modules/GSPNExportSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"
#include "storm/settings/modules/ResourceSettings.h"

/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("StoRM-GSPN", "storm-gspn");
    
    // Register all known settings modules.
    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::GSPNSettings>();
    storm::settings::addModule<storm::settings::modules::GSPNExportSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    storm::settings::addModule<storm::settings::modules::JaniExportSettings>();
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
}


std::unordered_map<std::string, uint64_t> parseCapacitiesList(std::string const& filename) {
    std::unordered_map<std::string, uint64_t> map;
    
    std::ifstream ifs;
    ifs.open(filename);
    
    std::string line;
    while( std::getline(ifs, line) ) {
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of("\t "));
        STORM_LOG_THROW(strs.size() == 2, storm::exceptions::WrongFormatException, "Expect key value pairs");
        std::cout << std::stoll(strs[1]) << std::endl;
        map[strs[0]] = std::stoll(strs[1]);
    }
    return map;
    
}


int main(const int argc, const char **argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("StoRM-GSPN", argc, argv);
        initializeSettings();
        
        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }

        // parse gspn from file
        if (!storm::settings::getModule<storm::settings::modules::GSPNSettings>().isGspnFileSet()) {
            return -1;
        }
        
        auto parser = storm::parser::GspnParser();
        auto gspn = parser.parse(storm::settings::getModule<storm::settings::modules::GSPNSettings>().getGspnFilename());

        if (!gspn->isValid()) {
            STORM_LOG_ERROR("The gspn is not valid.");
        }
        
        if(storm::settings::getModule<storm::settings::modules::GSPNSettings>().isCapacitiesFileSet()) {
            auto capacities = parseCapacitiesList(storm::settings::getModule<storm::settings::modules::GSPNSettings>().getCapacitiesFilename());
            gspn->setCapacities(capacities);
        }

        storm::handleGSPNExportSettings(*gspn);
        
        if(storm::settings::getModule<storm::settings::modules::JaniExportSettings>().isJaniFileSet()) {
            storm::jani::Model* model = storm::buildJani(*gspn);
            storm::exportJaniModel(*model, {}, storm::settings::getModule<storm::settings::modules::JaniExportSettings>().getJaniFilename());
            delete model;
        }

        delete gspn;
        return 0;
        
//
//        // construct ma
//        auto builder = storm::builder::ExplicitGspnModelBuilder<>();
//        auto ma = builder.translateGspn(gspn, formula);
//
//        // write gspn into output file
//        if (!outputFile.empty()) {
//            std::ofstream file;
//            file.open(outputFile);
//            if (outputType == "pnml") {
//                gspn.toPnml(file);
//            }
//            if (outputType == "pnpro") {
//                gspn.toPnpro(file);
//            }
//            if (outputType == "dot") {
//                gspn.writeDotToStream(file);
//            }
//            if (outputType == "ma") {
//                ma.writeDotToStream(file);
//            }
//            file.close();
//        }

        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
