#pragma once

#include "storm/storage/jani/Model.h"

#include "storm-gspn/builder/JaniGSPNBuilder.h"
#include "storm-gspn/storage/gspn/GSPN.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GSPNExportSettings.h"

namespace storm {
    /**
     *    Builds JANI model from GSPN.
     */
    storm::jani::Model* buildJani(storm::gspn::GSPN const& gspn) {
        std::shared_ptr<storm::expressions::ExpressionManager> exprManager(new storm::expressions::ExpressionManager());
        storm::builder::JaniGSPNBuilder builder(gspn, exprManager);
        return builder.build();
    }
    
    void handleGSPNExportSettings(storm::gspn::GSPN const& gspn) {
        storm::settings::modules::GSPNExportSettings const& exportSettings = storm::settings::getModule<storm::settings::modules::GSPNExportSettings>();
        if (exportSettings.isWriteToDotSet()) {
            std::ofstream fs;
            fs.open(exportSettings.getWriteToDotFilename());
            gspn.writeDotToStream(fs);
            fs.close();
        }
        
        if (exportSettings.isWriteToPnproSet()) {
            std::ofstream fs;
            fs.open(exportSettings.getWriteToPnproFilename());
            gspn.toPnpro(fs);
            fs.close();
        }
        
        if (exportSettings.isWriteToPnmlSet()) {
            std::ofstream fs;
            fs.open(exportSettings.getWriteToPnmlFilename());
            gspn.toPnml(fs);
            fs.close();
        }
        
        if (exportSettings.isDisplayStatsSet()) {
            std::cout << "============GSPN Statistics==============" << std::endl;
            gspn.writeStatsToStream(std::cout);
            std::cout << "=========================================" << std::endl;
        }
        
        if (exportSettings.isWriteStatsToFileSet()) {
            std::ofstream fs;
            fs.open(exportSettings.getWriteStatsFilename());
            gspn.writeStatsToStream(fs);
            fs.close();
        }
        
        
        
    }
    
}