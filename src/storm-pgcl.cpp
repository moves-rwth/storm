#include "src/parser/PgclParser.h"

#include "logic/Formula.h"
#include "utility/initialize.h"
#include "utility/storm.h"
#include "src/cli/cli.h"
#include "src/exceptions/BaseException.h"
#include "src/utility/macros.h"
#include <boost/lexical_cast.hpp>

#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/PGCLSettings.h"
#include "src/settings/modules/CoreSettings.h"
#include "src/settings/modules/DebugSettings.h"
//#include "src/settings/modules/CounterexampleGeneratorSettings.h"
//#include "src/settings/modules/CuddSettings.h"
//#include "src/settings/modules/SylvanSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
//#include "src/settings/modules/BisimulationSettings.h"
//#include "src/settings/modules/GlpkSettings.h"
//#include "src/settings/modules/GurobiSettings.h"
//#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
//#include "src/settings/modules/ParametricSettings.h"
#include "src/settings/modules/EliminationSettings.h"

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
    //storm::settings::addModule<storm::settings::modules::CounterexampleGeneratorSettings>();
    //storm::settings::addModule<storm::settings::modules::CuddSettings>();
    //storm::settings::addModule<storm::settings::modules::SylvanSettings>();
    storm::settings::addModule<storm::settings::modules::GmmxxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
    //storm::settings::addModule<storm::settings::modules::BisimulationSettings>();
    //storm::settings::addModule<storm::settings::modules::GlpkSettings>();
    //storm::settings::addModule<storm::settings::modules::GurobiSettings>();
    //storm::settings::addModule<storm::settings::modules::TopologicalValueIterationEquationSolverSettings>();
    //storm::settings::addModule<storm::settings::modules::ParametricSettings>();
    storm::settings::addModule<storm::settings::modules::EliminationSettings>();
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
        
        if(storm::settings::getModule<storm::settings::modules::PGCLSettings>().isPgclFileSet()) {
            storm::pgcl::PgclProgram prog = storm::parser::PgclParser::parse(storm::settings::getModule<storm::settings::modules::PGCLSettings>().getPgclFilename());
            std::cout << prog << std::endl;
        }
        
        
        
    }catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused StoRM-PGCL to terminate. The message of the exception is: " << exception.what());
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused StoRM-PGCL to terminate. The message of this exception is: " << exception.what());
    }
}