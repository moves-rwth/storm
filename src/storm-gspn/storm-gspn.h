#pragma once

#include "storm/storage/jani/Model.h"

#include "storm-gspn/builder/JaniGSPNBuilder.h"
#include "storm-gspn/storage/gspn/Gspn.h"

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
            gspn.writeDotToStream(std::cout);
            gspn.writeDotToStream(fs);
            fs.close();
        }
        
    }
    
}