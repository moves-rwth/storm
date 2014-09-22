#ifndef STORM_SETTINGS_SETTINGSMANAGER_H_
#define STORM_SETTINGS_SETTINGSMANAGER_H_

#include <iostream>
#include <sstream>
#include <list>
#include <set>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBase.h"
#include "src/settings/Argument.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"
#include "src/settings/modules/ModuleSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/DebugSettings.h"
#include "src/settings/modules/CounterexampleGeneratorSettings.h"
#include "src/settings/modules/CuddSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/settings/modules/GlpkSettings.h"
#include "src/settings/modules/GurobiSettings.h"

#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/OptionParserException.h"

namespace storm {
    namespace settings {
        
        /*!
         * Provides the central API for the registration of command line options and parsing the options from the
         * command line. Since this class is a singleton, the only instance is accessible via a call to the manager()
         * function.
         */
        class SettingsManager {
		public:
            /*!
             * This function parses the given command line arguments and sets all registered options accordingly. If the
             * command line cannot be matched using the known options, an exception is thrown.
             *
             * @param argc The number of command line arguments.
             * @param argv The command line arguments.
             */
			void setFromCommandLine(int const argc, char const * const argv[]);
            
            /*!
             * This function parses the given command line arguments (represented by one big string) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineString The command line arguments as one string.
             */
            void setFromString(std::string const& commandLineString);
            
            /*!
             * This function parses the given command line arguments (represented by several strings) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineArguments The command line arguments.
             */
            void setFromExplodedString(std::vector<std::string> const& commandLineArguments);
            
            /*!
             * This function parses the given file and sets all registered options accordingly. If the settings cannot
             * be matched using the known options, an exception is thrown.
             */
            void setFromConfigurationFile(std::string const& configFilename);
            
            /*!
             * This function prints a help message to the standard output. Optionally, a module name can be given. If
             * it is present, name must correspond to a known module. Then, only the help text for this module is
             * printed.
             *
             * @return moduleName The name of the module for which to show the help or "all" if the full help text is to
             * be printed.
             */
            void printHelp(std::string const& moduleName = "all") const;
            
            /*!
             * Retrieves the only existing instance of a settings manager.
             *
             * @return The only existing instance of a settings manager
             */
            static SettingsManager& manager();
            
            /*!
             * Adds a new module with the given name. If the module could not be successfully added, an exception is
             * thrown.
             *
             * @param moduleSettings The settings of the module to add.
             */
            void addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings);
            
            /*!
             * Retrieves the settings of the module with the given name.
             *
             * @param moduleName The name of the module for which to retrieve the settings.
             * @return An object that provides access to the settings of the module.
             */
            modules::ModuleSettings const& getModule(std::string const& moduleName) const;
            
        private:
			/*!
			 * Constructs a new manager. This constructor is private to forbid instantiation of this class. The only
             * way to create a new instance is by calling the static manager() method.
			 */
            SettingsManager();
			
			/*!
			 * This destructor is private, since we need to forbid explicit destruction of the manager.
			 */
			virtual ~SettingsManager();
            
            // The registered modules.
            std::unordered_map<std::string, std::unique_ptr<modules::ModuleSettings>> modules;

            // Mappings from all known option names to the options that match it. All options for one option name need
            // to be compatible in the sense that calling isCompatible(...) pairwise on all options must always return true.
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> longNameToOptions;
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> shortNameToOptions;
            
            // A mapping of module names to the corresponding options.
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> moduleOptions;
            
            /*!
             * Adds the given option to the known options.
             *
             * @param option The option to add.
             */
            void addOption(std::shared_ptr<Option> const& option);
            
            /*!
             * Sets the arguments of the options matching the given name from the provided strings.
             *
             * @param optionName The name of the options for which to set the arguments.
             * @param optionMap The mapping from option names to options.
             * @param argumentCache The arguments of the option as string values.
             */
            void setOptionsArguments(std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap, std::vector<std::string> const& argumentCache);
        };
        
        /*!
         * Retrieves the settings manager.
         *
         * @return The only settings manager.
         */
        SettingsManager& manager() {
            return SettingsManager::manager();
        }
        
        /*!
         * Retrieves the general settings.
         *
         * @return An object that allows accessing the general settings.
         */
        storm::settings::modules::GeneralSettings const& generalSettings() {
            return dynamic_cast<storm::settings::modules::GeneralSettings const&>(manager().getModule(storm::settings::modules::GeneralSettings::moduleName));
        }
        
        /*!
         * Retrieves the debug settings.
         *
         * @return An object that allows accessing the debug settings.
         */
        storm::settings::modules::DebugSettings const& debugSettings()  {
            return dynamic_cast<storm::settings::modules::DebugSettings const&>(manager().getModule(storm::settings::modules::DebugSettings::moduleName));
        }
        
        /*!
         * Retrieves the counterexample generator settings.
         *
         * @return An object that allows accessing the counterexample generator settings.
         */
        storm::settings::modules::CounterexampleGeneratorSettings const& counterexampleGeneratorSettings() {
            return dynamic_cast<storm::settings::modules::CounterexampleGeneratorSettings const&>(manager().getModule(storm::settings::modules::CounterexampleGeneratorSettings::moduleName));
        }
        
        /*!
         * Retrieves the CUDD settings.
         *
         * @return An object that allows accessing the CUDD settings.
         */
        storm::settings::modules::CuddSettings const& cuddSettings() {
            return dynamic_cast<storm::settings::modules::CuddSettings const&>(manager().getModule(storm::settings::modules::CuddSettings::moduleName));
        }
        
        /*!
         * Retrieves the settings of the gmm++-based equation solver.
         *
         * @return An object that allows accessing the settings of the gmm++-based equation solver.
         */
        storm::settings::modules::GmmxxEquationSolverSettings const& gmmxxEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::GmmxxEquationSolverSettings const&>(manager().getModule(storm::settings::modules::GmmxxEquationSolverSettings::moduleName));
        }
        
        /*!
         * Retrieves the settings of the native equation solver.
         *
         * @return An object that allows accessing the settings of the native equation solver.
         */
        storm::settings::modules::NativeEquationSolverSettings const& nativeEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::NativeEquationSolverSettings const&>(manager().getModule(storm::settings::modules::NativeEquationSolverSettings::moduleName));
        }
        
        /*!
         * Retrieves the settings of glpk.
         *
         * @return An object that allows accessing the settings of glpk.
         */
        storm::settings::modules::GlpkSettings const& glpkSettings() {
            return dynamic_cast<storm::settings::modules::GlpkSettings const&>(manager().getModule(storm::settings::modules::GlpkSettings::moduleName));
        }
        
        /*!
         * Retrieves the settings of Gurobi.
         *
         * @return An object that allows accessing the settings of Gurobi.
         */
        storm::settings::modules::GurobiSettings const& gurobiSettings() {
            return dynamic_cast<storm::settings::modules::GurobiSettings const&>(manager().getModule(storm::settings::modules::GurobiSettings::moduleName));
        }
        
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_SETTINGSMANAGER_H_ */