#include "storm-dft/settings/DftSettings.h"

#include "storm-dft/settings/modules/DftIOSettings.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/EigenEquationSolverSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"
#include "storm/settings/modules/GSPNSettings.h"
#include "storm/settings/modules/GSPNExportSettings.h"


namespace storm {
    namespace settings {
        void initializeDftSettings(std::string const& name, std::string const& executableName) {
            storm::settings::mutableManager().setName(name, executableName);
        
            // Register relevant settings modules.
            storm::settings::addModule<storm::settings::modules::GeneralSettings>();
            storm::settings::addModule<storm::settings::modules::DftIOSettings>();
            storm::settings::addModule<storm::settings::modules::FaultTreeSettings>();
            storm::settings::addModule<storm::settings::modules::IOSettings>();
            storm::settings::addModule<storm::settings::modules::CoreSettings>();

            storm::settings::addModule<storm::settings::modules::DebugSettings>();
            storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
            storm::settings::addModule<storm::settings::modules::GmmxxEquationSolverSettings>();
            storm::settings::addModule<storm::settings::modules::EigenEquationSolverSettings>();
            storm::settings::addModule<storm::settings::modules::EliminationSettings>();
            storm::settings::addModule<storm::settings::modules::MinMaxEquationSolverSettings>();
            // storm::settings::addModule<storm::settings::modules::BisimulationSettings>();
            storm::settings::addModule<storm::settings::modules::ResourceSettings>();

            // For translation into JANI via GSPN.
            storm::settings::addModule<storm::settings::modules::JaniExportSettings>();
            storm::settings::addModule<storm::settings::modules::GSPNSettings>();
            storm::settings::addModule<storm::settings::modules::GSPNExportSettings>();
        }
    
    }
}
