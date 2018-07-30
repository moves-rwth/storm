#include "storm-gspn/api/storm-gspn.h"

#include "storm/settings/SettingsManager.h"
#include "storm/utility/file.h"
#include "storm-gspn/settings/modules/GSPNExportSettings.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-conv/api/storm-conv.h"


namespace storm {
    namespace api {

        storm::jani::Model* buildJani(storm::gspn::GSPN const& gspn) {
            storm::builder::JaniGSPNBuilder builder(gspn);
            return builder.build();
        }

        void handleGSPNExportSettings(storm::gspn::GSPN const& gspn, std::vector<storm::jani::Property> const& properties) {
            storm::settings::modules::GSPNExportSettings const& exportSettings = storm::settings::getModule<storm::settings::modules::GSPNExportSettings>();
            if (exportSettings.isWriteToDotSet()) {
                std::ofstream fs;
                storm::utility::openFile(exportSettings.getWriteToDotFilename(), fs);
                gspn.writeDotToStream(fs);
                storm::utility::closeFile(fs);
            }

            if (exportSettings.isWriteToPnproSet()) {
                std::ofstream fs;
                storm::utility::openFile(exportSettings.getWriteToPnproFilename(), fs);
                gspn.toPnpro(fs);
                storm::utility::closeFile(fs);
            }

            if (exportSettings.isWriteToPnmlSet()) {
                std::ofstream fs;
                storm::utility::openFile(exportSettings.getWriteToPnmlFilename(), fs);
                gspn.toPnml(fs);
                storm::utility::closeFile(fs);
            }

            if (exportSettings.isWriteToJsonSet()) {
                std::ofstream fs;
                storm::utility::openFile(exportSettings.getWriteToJsonFilename(), fs);
                gspn.toJson(fs);
                storm::utility::closeFile(fs);
            }

            if (exportSettings.isDisplayStatsSet()) {
                std::cout << "============GSPN Statistics==============" << std::endl;
                gspn.writeStatsToStream(std::cout);
                std::cout << "=========================================" << std::endl;
            }

            if (exportSettings.isWriteStatsToFileSet()) {
                std::ofstream fs;
                storm::utility::openFile(exportSettings.getWriteStatsFilename(), fs);
                gspn.writeStatsToStream(fs);
                storm::utility::closeFile(fs);
            }
            
            if (exportSettings.isWriteToJaniSet()) {
                auto const& jani = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
                storm::converter::JaniConversionOptions options(jani);
    
                storm::jani::Model* model = storm::api::buildJani(gspn);
                storm::api::postprocessJani(*model, options);
                storm::api::exportJaniToFile(*model, properties, storm::settings::getModule<storm::settings::modules::GSPNExportSettings>().getWriteToJaniFilename());
                delete model;
            }
        }

    }
}
