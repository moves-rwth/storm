#include "FIGAROSettings.h"
//

#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/ModelCheckerSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm-figaro/settings/modules/FIGAROIOSettings.h"
#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm-conv/api/storm-conv.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/models/sparse/MarkovAutomaton.h"


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
//
namespace storm {

    namespace settings {
        
void initializeFigaroSettings(std::string const& name, std::string const& executableName) {
    storm::settings::mutableManager().setName("Storm-FIGARO", "storm-figaro");
    
        // Register all known settings modules.
    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    storm::settings::addModule<storm::settings::modules::FIGAROIOSettings>();
    storm::settings::addModule<storm::settings::modules::ModelCheckerSettings>();
    storm::settings::addModule<storm::settings::modules::BuildSettings>();
    storm::settings::addModule<storm::settings::modules::IOSettings>();
    
    
    storm::settings::addModule<storm::settings::modules::HintSettings>();
    storm::settings::addModule<storm::settings::modules::TransformationSettings>();
    storm::settings::addModule<storm::settings::modules::EigenEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::TopologicalEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::EliminationSettings>();
    storm::settings::addModule<storm::settings::modules::MinMaxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::MultiplierSettings>();
    storm::settings::addModule<storm::settings::modules::TimeBoundedSolverSettings>();
    
}
   }
}

