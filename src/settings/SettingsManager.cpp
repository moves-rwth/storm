#include "src/settings/SettingsManager.h"

#include <cstring>
#include <cctype>
#include <mutex>
#include <boost/algorithm/string.hpp>

#include "src/exceptions/OptionParserException.h"

namespace storm {
    namespace settings {

        SettingsManager::SettingsManager() {
            // Register all known settings modules.
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GeneralSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::DebugSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CounterexampleGeneratorSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CuddSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GmmxxEquationSolverSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::NativeEquationSolverSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GlpkSettings(*this)));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GurobiSettings(*this)));
        }

        SettingsManager::~SettingsManager() {
            // Intentionally left empty.
        }
        
        SettingsManager& SettingsManager::manager() {
            static SettingsManager settingsManager;
            return settingsManager;
        }
        
        void SettingsManager::setFromCommandLine(int const argc, char const * const argv[]) {
            // We convert the arguments to a vector of strings and strip off the first element since it refers to the
            // name of the program.
            std::vector<std::string> argumentVector(argc - 1);
            for (uint_fast64_t i = 1; i < argc; ++i) {
                argumentVector[i - 1] = std::string(argv[i]);
            }
            
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromString(std::string const& commandLineString) {
            std::vector<std::string> argumentVector;
            boost::split(argumentVector, commandLineString, boost::is_any_of("\t "));
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromExplodedString(std::vector<std::string> const& commandLineArguments) {
            LOG_ASSERT(false, "Not yet implemented");
        }
        
        void SettingsManager::setFromConfigurationFile(std::string const& configFilename) {
            LOG_ASSERT(false, "Not yet implemented");
        }
                
        void SettingsManager::printHelp(std::string const& moduleName) const {
            LOG_ASSERT(false, "Not yet implemented");
        }
        
        void SettingsManager::addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings) {
            LOG_ASSERT(false, "Not yet implemented");
        }

    }
}