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
        
        class InternalOptionMemento;
        
        /*!
         * Provides the central API for the registration of command line options and parsing the options from the
         * command line. Since this class is a singleton, the only instance is accessible via a call to the manager()
         * function.
         */
        class SettingsManager {
		public:
            // Declare the memento class as a friend so it can manipulate the internal state.
            friend class InternalOptionMemento;
			
            /*!
             * This function parses the given command line arguments and sets all registered options accordingly. If the
             * command line cannot be matched using the known options, an exception is thrown.
             *
             * @param argc The number of command line arguments.
             * @param argv The command line arguments.
             */
			static void setFromCommandLine(int const argc, char const * const argv[]);
            
            /*!
             * This function parses the given command line arguments (represented by one big string) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineString The command line arguments as one string.
             */
            static void setFromString(std::string const& commandLineString);
            
            /*!
             * This function parses the given command line arguments (represented by several strings) and sets all
             * registered options accordingly. If the command line cannot be matched using the known options, an
             * exception is thrown.
             *
             * @param commandLineArguments The command line arguments.
             */
            static void setFromExplodedString(std::vector<std::string> const& commandLineArguments);
            
            /*!
             * This function parses the given file and sets all registered options accordingly. If the settings cannot
             * be matched using the known options, an exception is thrown.
             */
            static void setFromConfigurationFile(std::string const& configFilename);
            
            /*!
             * Retrieves the general settings.
             *
             * @return An object that allows accessing the general settings.
             */
            storm::settings::modules::GeneralSettings const& getGeneralSettings() const;

            /*!
             * Retrieves the debug settings.
             *
             * @return An object that allows accessing the debug settings.
             */
            storm::settings::modules::DebugSettings const& getDebugSettings() const;

            /*!
             * Retrieves the counterexample generator settings.
             *
             * @return An object that allows accessing the counterexample generator settings.
             */
            storm::settings::modules::CounterexampleGeneratorSettings const& getCounterexampleGeneratorSettings() const;

            /*!
             * Retrieves the CUDD settings.
             *
             * @return An object that allows accessing the CUDD settings.
             */
            storm::settings::modules::CuddSettings const& getCuddSettings() const;

            /*!
             * Retrieves the settings of the gmm++-based equation solver.
             *
             * @return An object that allows accessing the settings of the gmm++-based equation solver.
             */
            storm::settings::modules::GmmxxEquationSolverSettings const& getGmmxxEquationSolverSettings() const;

            /*!
             * Retrieves the settings of the native equation solver.
             *
             * @return An object that allows accessing the settings of the native equation solver.
             */
            storm::settings::modules::NativeEquationSolverSettings const& getNativeEquationSolverSettings() const;

            /*!
             * Retrieves the settings of glpk.
             *
             * @return An object that allows accessing the settings of glpk.
             */
            storm::settings::modules::GlpkSettings const& getGlpkSettings() const;

            /*!
             * Retrieves the settings of Gurobi.
             *
             * @return An object that allows accessing the settings of Gurobi.
             */
            storm::settings::modules::GurobiSettings const& getGurobiSettings() const;
            
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
 			
            // An object that provides access to the general settings.
            storm::settings::modules::GeneralSettings generalSettings;
            
            // An object that provides access to the debug settings.
            storm::settings::modules::DebugSettings debugSettings;

            // An object that provides access to the counterexample generator settings.
            storm::settings::modules::CounterexampleGeneratorSettings counterexampleGeneratorSettings;

            // An object that provides access to the CUDD settings.
            storm::settings::modules::CuddSettings cuddSettings;

            // An object that provides access to the gmm++ settings.
            storm::settings::modules::GmmxxEquationSolverSettings gmmxxEquationSolverSettings;

            // An object that provides access to the native equation solver settings.
            storm::settings::modules::NativeEquationSolverSettings nativeEquationSolverSettings;

            // An object that provides access to the glpk settings.
            storm::settings::modules::GlpkSettings glpkSettings;

            // An object that provides access to the Gurobi settings.
            storm::settings::modules::GurobiSettings gurobiSettings;
            
            // The single instance of the manager class. Since it's static, it will automatically be distructed upon termination.
 			static SettingsManager settingsManager;
            
            // A set of all known (i.e. registered) module names.
            std::set<std::string> moduleNames;

            // A mapping from all known option names to the options that match it. All options for one option name need
            // to be compatible in the sense that calling isCompatible(...) pairwise on all options must always return true.
            std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> options;
            
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
             * Registers a new module with the given name and options. If the module could not be successfully registered,
             * an exception is thrown.
             *
             * @param moduleName The name of the module to register.
             * @param options The set of options that the module registers.
             */
            void registerModule(std::string const& moduleName, std::vector<std::shared_ptr<Option>> const& options);
            
            /*!
             * Retrieves the only existing instance of a settings manager.
             *
             * @return The only existing instance of a settings manager
             */
            static SettingsManager& manager();
            
			// Helper functions
			stringPair_t splitOptionString(std::string const& option);
			bool hasAssignment(std::string const& option);
			void handleAssignment(std::string const& longOptionName, std::vector<std::string> arguments);
			std::vector<std::string> argvToStringArray(int const argc, char const * const argv[]);
			std::vector<bool> scanForOptions(std::vector<std::string> const& arguments);
			bool checkArgumentSyntaxForOption(std::string const& argvString);
        };
        
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_SETTINGSMANAGER_H_ */