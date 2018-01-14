#pragma once

#include "storm/storage/jani/Model.h"

#include "storm-gspn/builder/JaniGSPNBuilder.h"
#include "storm-gspn/storage/gspn/GSPN.h"

#include "storm/settings/SettingsManager.h"
#include "storm-gspn/settings/modules/GSPNExportSettings.h"

#include "storm/utility/file.h"

namespace storm {
    /**
     *    Builds JANI model from GSPN.
     */
    storm::jani::Model* buildJani(storm::gspn::GSPN const& gspn) {
        storm::builder::JaniGSPNBuilder builder(gspn);
        return builder.build();
    }
    
    void handleGSPNExportSettings(storm::gspn::GSPN const& gspn) {
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
        
        
        
    }
    
}
